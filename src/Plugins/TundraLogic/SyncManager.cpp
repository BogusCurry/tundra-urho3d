﻿// For conditions of distribution and use, see copyright notice in LICENSE

#include "StableHeaders.h"
#include <kNet.h>

#include "SyncManager.h"
#include "TundraLogic.h"
#include "Client.h"
#include "Server.h"
#include "UserConnection.h"
#include "Framework.h"
#include "SceneAPI.h"

#include "TundraMessages.h"
#include "MsgEntityAction.h"
#include "TundraLogicUtils.h"
#include "Scene/Scene.h"
#include "Entity.h"
#include "CoreStringUtils.h"
#include "Camera.h"
#include "AttributeMetadata.h"
#include "LoggingFunctions.h"
#include "Profiler.h"
#include "Placeable.h"

#include <StringUtils.h>

#include <cstring>


// Used to print EC mismatch warnings only once per EC.
static std::set<u32> mismatchingComponentTypes;

static size_t oldAttrDataBufferSize = 16 * 1024;

namespace Tundra
{

bool SyncManager::WriteComponentFullUpdate(kNet::DataSerializer& ds, ComponentPtr comp)
{
    // Component identification
    ds.AddVLE<kNet::VLE8_16_32>(comp->Id() & UniqueIdGenerator::LAST_REPLICATED_ID);
    ds.AddVLE<kNet::VLE8_16_32>(comp->TypeId());
    ds.AddString(comp->Name().CString());
    
    // Create a nested dataserializer for the attributes, so we can survive unknown or incompatible components
    kNet::DataSerializer attrDs(attrDataBuffer_, NUMELEMS(attrDataBuffer_));

    // Static-structured attributes
    unsigned numStaticAttrs = comp->NumStaticAttributes();
    const AttributeVector& attrs = comp->Attributes();
    for (uint i = 0; i < numStaticAttrs; ++i)
        attrs[i]->ToBinary(attrDs);
    
    // Dynamic-structured attributes (use EOF to detect so do not need to send their amount)
    for (unsigned i = numStaticAttrs; i < attrs.Size(); ++i)
    {
        if (attrs[i] && attrs[i]->IsDynamic())
        {
            attrDs.Add<u8>((u8)i); // Index
            attrDs.Add<u8>((u8)attrs[i]->TypeId());
            attrDs.AddString(attrs[i]->Name().CString());
            attrs[i]->ToBinary(attrDs);
        }
    }

    if (!ValidateAttributeBuffer(false, attrDs, comp))
        return false;
    
    // Add the attribute array to the main serializer
    ds.AddVLE<kNet::VLE8_16_32>((u32)attrDs.BytesFilled());
    ds.AddArray<u8>((unsigned char*)attrDataBuffer_, (u32)attrDs.BytesFilled());
    return true;
}

bool SyncManager::ValidateAttributeBuffer(bool fatal, kNet::DataSerializer& ds, ComponentPtr &comp, size_t maxBytes)
{
    if (maxBytes == 0)
        maxBytes = oldAttrDataBufferSize;

    /** This is left here until both server and clients have the bigger buffer size.
        Server will be upgraded to a bigger buffer, but it still cant send bigger buffers
        than the old clients are able to handle. This below code will stop that from happening.
        If we don't increase the buffer on the server, the result will be a trashed stack around
        the buffers. Now we can still recover from it on the server without shutting the server down. */
    /// @todo Remove check after a couple of releases?!
    if (ds.BytesFilled() > maxBytes)
    {
        // Exceeded the new bigger buffer as well. This will corrupt the buffers and is fatal!
        if (ds.BytesFilled() > NUMELEMS(attrDataBuffer_))
            fatal = true;

        String entityIdentifier = (comp->ParentEntity() ? comp->ParentEntity()->ToString() : "");
        String ex = String("SyncManager::ValidateAttributeBuffer: Buffer overflow while processing ") + entityIdentifier + " Component id="
            + String(comp->Id()) + " typeid=" + String(comp->TypeId()) + ". Written " + String(ds.BytesFilled()) + " bytes to buffer of " + String(maxBytes) + " bytes.";
           
        if (fatal)
            /// \todo Better way to handle this?
            throw std::runtime_error(ex.CString());
        else
            LogError(ex);
        return false;
    }

    return true;
}

SyncManager::SyncManager(TundraLogic* owner) :
    Object(owner->GetContext()),
    owner_(owner),
    framework_(owner->GetFramework()),
    updatePeriod_(1.0f / 20.0f),
    updateAcc_(0.0),
    maxLinExtrapTime_(3.0f),
    noClientPhysicsHandoff_(false),
    componentTypeSender_(0)
{
    if (framework_->HasCommandLineParameter("--noclientphysics"))
        noClientPhysicsHandoff_ = true;
    
    GetClientExtrapolationTime();

    // Connect to network messages from the server
    serverConnection_ = owner_->Client()->ServerUserConnection();
    serverConnection_->NetworkMessageReceived.Connect(this, &SyncManager::HandleNetworkMessage);
    
    // Connect to SceneAPI's PlaceholderComponentTypeRegistered signal
    framework_->Scene()->PlaceholderComponentTypeRegistered.Connect(this, &SyncManager::OnPlaceholderComponentTypeRegistered);
  
}

SyncManager::~SyncManager()
{
}

void SyncManager::SendCameraUpdateRequest(UserConnectionPtr conn, bool enabled)
{
    const int maxMessageSizeBytes = 1400;

    kNet::DataSerializer ds(maxMessageSizeBytes);

    ds.Add<u8>(enabled);

    conn->Send(cCameraOrientationRequest, true, true, ds);
}

void SyncManager::SetUpdatePeriod(float period)
{
    // Allow max 100fps
    if (period < 0.01f)
        period = 0.01f;
    updatePeriod_ = period;
    
    GetClientExtrapolationTime();
}

void SyncManager::GetClientExtrapolationTime()
{
    StringVector extrapTimeParam = framework_->CommandLineParameters("--clientextrapolationtime");
    if (extrapTimeParam.Size() > 0)
    {
        float newExtrapTime = ToFloat(extrapTimeParam.Front());
        if (newExtrapTime >= 0.0f)
        {
            // First update period is always interpolation, and extrapolation time is in addition to that
            maxLinExtrapTime_ = 1.0f + newExtrapTime / 1000.0f / updatePeriod_;
        }
    }
}

SceneSyncState* SyncManager::SceneState(u32 connectionId) const
{
    if (!owner_->IsServer())
        return 0;
    return SceneState(owner_->Server()->UserConnectionById(connectionId));
}

SceneSyncState* SyncManager::SceneState(const UserConnectionPtr &connection) const
{
    if (!owner_->IsServer())
        return 0;
    if (!connection)
        return 0;
    return connection->syncState.Get();
}

void SyncManager::RegisterToScene(ScenePtr scene)
{
    // Disconnect from previous scene if not expired
    ScenePtr previous = scene_.Lock();
    if (previous)
    {
        previous->AttributeChanged.Disconnect(this, &SyncManager::OnAttributeChanged);
        previous->AttributeAdded.Disconnect(this, &SyncManager::OnAttributeAdded);
        previous->AttributeRemoved.Disconnect(this, &SyncManager::OnAttributeRemoved);
        previous->ComponentAdded.Disconnect(this, &SyncManager::OnComponentAdded);
        previous->ComponentRemoved.Disconnect(this, &SyncManager::OnComponentRemoved);
        previous->EntityCreated.Disconnect(this, &SyncManager::OnEntityCreated);
        previous->EntityRemoved.Disconnect(this, &SyncManager::OnEntityRemoved);
        previous->ActionTriggered.Disconnect(this, &SyncManager::OnActionTriggered);
        previous->EntityTemporaryStateToggled.Disconnect(this, &SyncManager::OnEntityPropertiesChanged);
        previous->EntityParentChanged.Disconnect(this, &SyncManager::OnEntityParentChanged);

        /// \todo make sure to disconnect all signals to previous scene
        //disconnect(previous.Get(), 0, this, 0);
    }
    
    serverConnection_->syncState->Clear();
    serverConnection_->syncState->SetParentScene(SceneWeakPtr(scene));
    scene_.Reset();
    componentTypesFromServer_.clear();
    
    if (!scene)
    {
        LogError("SyncManager::RegisterToScene: Null scene, cannot replicate");
        return;
    }
    
    scene_ = scene;
    Scene* sceneptr = scene.Get();
    sceneptr->AttributeChanged.Connect(this, &SyncManager::OnAttributeChanged);
    sceneptr->AttributeAdded.Connect(this, &SyncManager::OnAttributeAdded);
    sceneptr->AttributeRemoved.Connect(this, &SyncManager::OnAttributeRemoved);
    sceneptr->ComponentAdded.Connect(this, &SyncManager::OnComponentAdded);
    sceneptr->ComponentRemoved.Connect(this, &SyncManager::OnComponentRemoved);
    sceneptr->EntityCreated.Connect(this, &SyncManager::OnEntityCreated);
    sceneptr->EntityRemoved.Connect(this, &SyncManager::OnEntityRemoved);
    sceneptr->ActionTriggered.Connect(this, &SyncManager::OnActionTriggered);
    sceneptr->EntityTemporaryStateToggled.Connect(this, &SyncManager::OnEntityPropertiesChanged);
    sceneptr->EntityParentChanged.Connect(this, &SyncManager::OnEntityParentChanged);
}

void SyncManager::HandleNetworkMessage(UserConnection* user, kNet::packet_id_t packetId, kNet::message_id_t messageId, const char* data, size_t numBytes)
{
    if (!user || !scene_.Get())
        return;

    try
    {
        switch(messageId)
        {
        case cCameraOrientationUpdate:
            HandleCameraOrientation(user, data, numBytes);
            break;
        case cCreateEntityMessage:
            HandleCreateEntity(user, data, numBytes);
            break;
        case cCreateComponentsMessage:
            HandleCreateComponents(user, data, numBytes);
            break;
        case cCreateAttributesMessage:
            HandleCreateAttributes(user, data, numBytes);
            break;
        case cEditAttributesMessage:
            HandleEditAttributes(user, data, numBytes);
            break;
        case cRemoveAttributesMessage:
            HandleRemoveAttributes(user, data, numBytes);
            break;
        case cRemoveComponentsMessage:
            HandleRemoveComponents(user, data, numBytes);
            break;
        case cRemoveEntityMessage:
            HandleRemoveEntity(user, data, numBytes);
            break;
        case cCreateEntityReplyMessage:
            HandleCreateEntityReply(user, data, numBytes);
            break;
        case cCreateComponentsReplyMessage:
            HandleCreateComponentsReply(user, data, numBytes);
            break;
        case cRigidBodyUpdateMessage:
            HandleRigidBodyChanges(user, packetId, data, numBytes);
            break;
        case cEditEntityPropertiesMessage:
            HandleEditEntityProperties(user, data, numBytes);
            break;
        case cSetEntityParentMessage:
            HandleSetEntityParent(user, data, numBytes);
            break;
        case cEntityActionMessage:
            {
                MsgEntityAction msg(data, numBytes);
                HandleEntityAction(user, msg);
            }
            break;
        case cRegisterComponentTypeMessage:
            HandleRegisterComponentType(user, data, numBytes);
            break;
        }
    }
    catch (kNet::NetException& e)
    {
        LogError("Exception while handling scene sync network message " + String(messageId) + ": " + String(e.what()));
        user->Disconnect();
    }
}

void SyncManager::NewUserConnected(const UserConnectionPtr &user)
{
    PROFILE(SyncManager_NewUserConnected);

    ScenePtr scene = scene_.Lock();
    if (!scene)
    {
        LogWarning("SyncManager: Cannot handle new user connection message - No scene set!");
        return;
    }

    // Connect to actions sent to specifically to this user
    user->ActionTriggered.Connect(this, &SyncManager::OnUserActionTriggered);
    
    // Connect to network messages from this user
    user->NetworkMessageReceived.Connect(this, &SyncManager::HandleNetworkMessage);

    // Mark all entities in the sync state as new so we will send them
    user->syncState = SharedPtr<SceneSyncState>(new SceneSyncState(user.Get(), user->ConnectionId(), owner_->IsServer()));
    user->syncState->SetParentScene(scene_);

    if (owner_->IsServer())
        SceneStateCreated.Emit(user.Get(), user->syncState.Get());

    for(auto iter = scene->Begin(); iter != scene->End(); ++iter)
    {
        EntityPtr entity = iter->second_;
        if (entity->IsLocal())
            continue;
        entity_id_t id = entity->Id();
        user->syncState->MarkEntityDirty(id);
    }
}

void SyncManager::OnAttributeChanged(IComponent* comp, IAttribute* attr, AttributeChange::Type change)
{
    assert(comp && attr);
    if (!comp || !attr)
        return;

    bool isServer = owner_->IsServer();
    
    // Client: Check for stopping interpolation, if we change a currently interpolating variable ourselves
    if (!isServer) // Since the server never interpolates attributes, we don't need to do this check on the server at all.
    {
        ScenePtr scene = scene_.Lock();
        if (scene && !scene->IsInterpolating())
        {
            if (attr->Metadata() && attr->Metadata()->interpolation == AttributeMetadata::Interpolate)
                // Note: it does not matter if the attribute was not actually interpolating
                scene->EndAttributeInterpolation(attr);
        }
    }
    
    // Is this change even supposed to go to the network?
    if (change != AttributeChange::Replicate || comp->IsLocal())
        return;

    Entity* entity = comp->ParentEntity();
    if (!entity || entity->IsLocal())
        return; // This is a local entity, don't take it to network.
    
    if (isServer)
    {
        // For each client connected to this server, mark this attribute dirty, so it will be updated to the
        // clients on the next network sync iteration.
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState)
                (*i)->syncState->MarkAttributeDirty(entity->Id(), comp->Id(), attr->Index());
    }
    else
    {
        // As a client, mark the attribute dirty so we will push the new value to server on the next
        // network sync iteration.
        serverConnection_->syncState->MarkAttributeDirty(entity->Id(), comp->Id(), attr->Index());
    }
}

void SyncManager::OnAttributeAdded(IComponent* comp, IAttribute* attr, AttributeChange::Type /*change*/)
{
    assert(comp && attr);
    if (!comp || !attr)
        return;

    bool isServer = owner_->IsServer();
    
    // We do not allow to create attributes in local or disconnected signaling mode in a replicated component.
    // Always replicate the creation, because the client & server must have their attribute count in sync to 
    // be able to send attribute bitmasks
    if (comp->IsLocal())
        return;
    Entity* entity = comp->ParentEntity();
    if ((!entity) || (entity->IsLocal()))
        return;
    
    if (isServer)
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState) (*i)->syncState->MarkAttributeCreated(entity->Id(), comp->Id(), attr->Index());
    }
    else
    {
        serverConnection_->syncState->MarkAttributeCreated(entity->Id(), comp->Id(), attr->Index());
    }
}

void SyncManager::OnAttributeRemoved(IComponent* comp, IAttribute* attr, AttributeChange::Type /*change*/)
{
    assert(comp && attr);
    if (!comp || !attr)
        return;

    bool isServer = owner_->IsServer();
    
    // We do not allow to remove attributes in local or disconnected signaling mode in a replicated component.
    // Always replicate the removeal, because the client & server must have their attribute count in sync to
    // be able to send attribute bitmasks
    if (comp->IsLocal())
        return;
    Entity* entity = comp->ParentEntity();
    if ((!entity) || (entity->IsLocal()))
        return;
    
    if (isServer)
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState) (*i)->syncState->MarkAttributeRemoved(entity->Id(), comp->Id(), attr->Index());
    }
    else
    {
        serverConnection_->syncState->MarkAttributeRemoved(entity->Id(), comp->Id(), attr->Index());
    }
}

void SyncManager::OnComponentAdded(Entity* entity, IComponent* comp, AttributeChange::Type change)
{
    assert(entity && comp);
    if (!entity || !comp)
        return;

    if ((change != AttributeChange::Replicate) || (comp->IsLocal()))
        return;
    if (entity->IsLocal())
        return;
    
    if (owner_->IsServer())
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState) (*i)->syncState->MarkComponentDirty(entity->Id(), comp->Id());
    }
    else
    {
        serverConnection_->syncState->MarkComponentDirty(entity->Id(), comp->Id());
    }
}

void SyncManager::OnComponentRemoved(Entity* entity, IComponent* comp, AttributeChange::Type change)
{
    assert(entity && comp);
    if (!entity || !comp)
        return;
    if ((change != AttributeChange::Replicate) || (comp->IsLocal()))
        return;
    if (entity->IsLocal())
        return;
    
    if (owner_->IsServer())
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState) (*i)->syncState->MarkComponentRemoved(entity->Id(), comp->Id());
    }
    else
    {
        serverConnection_->syncState->MarkComponentRemoved(entity->Id(), comp->Id());
    }
}

void SyncManager::OnEntityCreated(Entity* entity, AttributeChange::Type change)
{
    assert(entity);
    if (!entity)
        return;
    if ((change != AttributeChange::Replicate) || (entity->IsLocal()))
        return;

    if (owner_->IsServer())
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
        {
            if ((*i)->syncState)
            {
                (*i)->syncState->MarkEntityDirty(entity->Id());
                if ((*i)->syncState->entities[entity->Id()].removed)
                {
                    LogWarning("An entity with ID " + String(entity->Id()) + " is queued to be deleted, but a new entity \"" + 
                        entity->Name() + "\" is to be added to the scene!");
                }
            }
        }
    }
    else
    {
        serverConnection_->syncState->MarkEntityDirty(entity->Id());
    }
}

void SyncManager::OnEntityRemoved(Entity* entity, AttributeChange::Type change)
{
    assert(entity);
    if (!entity)
        return;
    if (change != AttributeChange::Replicate)
        return;
    if (entity->IsLocal())
        return;
    
    if (owner_->IsServer())
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState) (*i)->syncState->MarkEntityRemoved(entity->Id());
    }
    else
    {
        serverConnection_->syncState->MarkEntityRemoved(entity->Id());
    }
}

void SyncManager::OnActionTriggered(Entity *entity, const String &action, const StringVector &params, EntityAction::ExecTypeField type)
{
    // If we are the server and the local script on this machine has requested a script to be executed on the server, it
    // means we just execute the action locally here, without sending to network.
    bool isServer = owner_->IsServer();
    if (isServer && (type & EntityAction::Server) != 0)
        entity->Exec(EntityAction::Local, action, params);

    // Craft EntityAction message.
    MsgEntityAction msg;
    msg.entityId = entity->Id();
    // msg.executionType will be set below depending are we server or client.
    msg.name = StringToBuffer(action.CString());
    for(unsigned int i = 0; i < params.Size(); ++i)
    {
        MsgEntityAction::S_parameters p = { StringToBuffer(params[i]) };
        msg.parameters.push_back(p);
    }

    if (!isServer && ((type & EntityAction::Server) != 0 || (type & EntityAction::Peers) != 0) && owner_->Client()->MessageConnection())
    {
        // send without Local flag
        msg.executionType = (u8)(type & ~EntityAction::Local);
        owner_->Client()->MessageConnection()->Send(msg);
    }

    if (isServer && (type & EntityAction::Peers) != 0)
    {
        msg.executionType = (u8)EntityAction::Local; // Propagate as local actions.
        // On server, queue the actions and send after entity sync
        /// \todo Making copy is inefficient, consider storing pointers
        foreach(UserConnectionPtr c, owner_->Server()->UserConnections())
        {
            if (c->properties["authenticated"].GetBool() == true)
                c->syncState->queuedActions.push_back(msg);
        }
    }
}

void SyncManager::OnUserActionTriggered(UserConnection* user, Entity *entity, const String &action, const StringVector &params)
{
    assert(user && entity);
    if (!entity || !user)
        return;
    bool isServer = owner_->IsServer();
    if (!isServer)
        return; // Should never happen
    if (user->properties["authenticated"].GetBool() != true)
        return; // Not yet authenticated, do not receive actions
    
    // Craft EntityAction message.
    MsgEntityAction msg;
    msg.entityId = entity->Id();
    msg.name = StringToBuffer(action);
    msg.executionType = (u8)EntityAction::Local; // Propagate as local action.
    for(unsigned int i = 0; i < params.Size(); ++i)
    {
        MsgEntityAction::S_parameters p = { StringToBuffer(params[i]) };
        msg.parameters.push_back(p);
    }
    user->Send(msg);
}

void SyncManager::OnEntityPropertiesChanged(Entity* entity, AttributeChange::Type change)
{
    assert(entity);
    if (!entity)
        return;
    if ((change != AttributeChange::Replicate) || (entity->IsLocal()))
        return;

    if (owner_->IsServer())
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
        {
            if ((*i)->syncState)
                (*i)->syncState->MarkEntityDirty(entity->Id(), true);
        }
    }
    else
    {
        serverConnection_->syncState->MarkEntityDirty(entity->Id(), true);
    }
}

void SyncManager::OnEntityParentChanged(Entity* entity, Entity* newParent, AttributeChange::Type change)
{
    assert(entity);
    if (!entity)
        return;
    if ((change != AttributeChange::Replicate) || (entity->IsLocal()))
        return;
    if (newParent && newParent->IsLocal())
    {
        LogError("Replicated entity " + String(entity->Id()) + " is parented to a local entity, can not replicate parenting properly over the network");
        return;
    }

    if (owner_->IsServer())
    {
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
        {
            if ((*i)->syncState)
                (*i)->syncState->MarkEntityDirty(entity->Id(), false, true);
        }
    }
    else
    {
        serverConnection_->syncState->MarkEntityDirty(entity->Id(), false, true);
    }
}

void SyncManager::OnPlaceholderComponentTypeRegistered(u32 typeId, const String& typeName, AttributeChange::Type change)
{
    UNREFERENCED_PARAM(typeName)
    if (change == AttributeChange::Default)
        change = AttributeChange::Replicate;
    if (change != AttributeChange::Replicate)
        return;
    ReplicateComponentType(typeId);
}

void SyncManager::ReplicateComponentType(u32 typeId, UserConnection* connection)
{
    SceneAPI* sceneAPI = framework_->Scene();
    const SceneAPI::PlaceholderComponentTypeMap& descs = sceneAPI->PlaceholderComponentTypes();
    auto it = descs.Find(typeId);
    if (it == descs.End())
    {
        LogWarning("SyncManager::SendComponentTypeDescription: unknown component type " + String(typeId));
        return;
    }

    const ComponentDesc& desc = it->second_;

    kNet::DataSerializer ds(64 * 1024);
    ds.AddVLE<kNet::VLE8_16_32>(desc.typeId);
    ds.AddString(desc.typeName.CString());
    ds.AddVLE<kNet::VLE8_16_32>(desc.attributes.Size());
    for (unsigned int i = 0; i < desc.attributes.Size(); ++i)
    {
        const AttributeDesc& attrDesc = desc.attributes[i];
        ds.Add<u8>((u8)sceneAPI->AttributeTypeIdForTypeName(attrDesc.typeName));
        /// \todo Use UTF-8 encoding
        ds.AddString(attrDesc.id.CString());
        ds.AddString(attrDesc.name.CString());
    }

    if (!connection)
    {
        if (owner_->IsServer())
        {
            UserConnectionList users = owner_->Server()->AuthenticatedUsers();
            for(auto i = users.Begin(); i != users.End(); ++i)
            {
                if ((*i)->ProtocolVersion() >= ProtocolCustomComponents && (*i).Get() != componentTypeSender_)
                    (*i)->Send(cRegisterComponentTypeMessage, true, true, ds);
            }
        }
        else if (serverConnection_ && serverConnection_->ProtocolVersion() >= ProtocolCustomComponents)
            Urho3D::StaticCast<UserConnection>(serverConnection_)->Send(cRegisterComponentTypeMessage, true, true, ds);
    }
    else
    {
        if (connection->ProtocolVersion() >= ProtocolCustomComponents)
            connection->Send(cRegisterComponentTypeMessage, true, true, ds);
    }
}

/// Interpolates from (pos0, vel0) to (pos1, vel1) with a C1 curve (continuous in position and velocity)
float3 HermiteInterpolate(const float3 &pos0, const float3 &vel0, const float3 &pos1, const float3 &vel1, float t)
{
    float tt = t*t;
    float ttt = tt*t;
    float h1 = 2*ttt - 3*tt + 1;
    float h2 = 1 - h1;
    float h3 = ttt - 2*tt + t;
    float h4 = ttt - tt;

    return h1 * pos0 + h2 * pos1 + h3 * vel0 + h4 * vel1;
}

/// Returns the tangent vector (derivative) of the Hermite curve. Note that the differential is w.r.t. timesteps along the curve from t=[0,1] 
/// and not in "wallclock" time.
float3 HermiteDerivative(const float3 &pos0, const float3 &vel0, const float3 &pos1, const float3 &vel1, float t)
{
    float tt = t*t;
    float h1 = 6*(tt - t);
    float h2 = -h1;
    float h3 = 3*tt - 4*t + 1;
    float h4 = 3*tt - 2*t;

    return h1 * pos0 + h2 * pos1 + h3 * vel0 + h4 * vel1;
}

/// \todo Uncomment and fix once RigidBody component implemented
void SyncManager::InterpolateRigidBodies(f64 /*frametime*/, SceneSyncState* /*state*/)
{
//    ScenePtr scene = scene_.Lock();
//    if (!scene)
//        return;
//
//    for(std::map<entity_id_t, RigidBodyInterpolationState>::iterator iter = state->entityInterpolations.begin(); 
//        iter != state->entityInterpolations.end();)
//    {
//        EntityPtr e = scene->EntityById(iter->first);
//        shared_ptr<EC_Placeable> placeable = e ? e->GetComponent<EC_Placeable>() : shared_ptr<EC_Placeable>();
//        if (!placeable.get())
//        {
//            std::map<entity_id_t, RigidBodyInterpolationState>::iterator del = iter++;
//            state->entityInterpolations.erase(del);
//            continue;
//        }
//
//        shared_ptr<EC_RigidBody> rigidBody = e->GetComponent<EC_RigidBody>();
//
//        RigidBodyInterpolationState &r = iter->second;
//        if (!r.interpolatorActive)
//        {
//            ++iter;
//            continue;
//        }
//
//        const float interpPeriod = updatePeriod_; // Time in seconds how long interpolating the Hermite spline from [0,1] should take.
//
//        // Test: Uncomment to only interpolate.
////        r.interpTime = std::min(1.0f, r.interpTime + (float)frametime / interpPeriod);
//        r.interpTime += (float)frametime / interpPeriod;
//
//        // Objects without a rigidbody, or with mass 0 never extrapolate (objects with mass 0 are stationary for Bullet).
//        const bool isNewtonian = rigidBody && rigidBody->mass.Get() > 0;
//
//        float3 pos;
//        if (r.interpTime < 1.0f) // Interpolating between two messages from server.
//        {
//            if (isNewtonian)
//                pos = HermiteInterpolate(r.interpStart.pos, r.interpStart.vel * interpPeriod, r.interpEnd.pos, r.interpEnd.vel * interpPeriod, r.interpTime);
//            else
//                pos = HermiteInterpolate(r.interpStart.pos, float3::zero, r.interpEnd.pos, float3::zero, r.interpTime);
//        }
//        else // Linear extrapolation if server has not sent an update.
//        {
//            if (isNewtonian && maxLinExtrapTime_ > 1.0f)
//                pos = r.interpEnd.pos + r.interpEnd.vel * (r.interpTime-1.f) * interpPeriod;
//            else
//                pos = r.interpEnd.pos;
//        }
//        ///\todo Orientation is only interpolated, and capped to end result. Also extrapolate orientation.
//        Quat rot = Quat::Slerp(r.interpStart.rot, r.interpEnd.rot, Clamp01(r.interpTime));
//        float3 scale = float3::Lerp(r.interpStart.scale, r.interpEnd.scale, Clamp01(r.interpTime));
//        
//        Transform t;
//        t.SetPos(pos);
//        t.SetOrientation(rot);
//        t.SetScale(scale);
//        placeable->transform.Set(t, AttributeChange::LocalOnly);
//
//        // Local simulation steps:
//        // One fixed update interval: interpolate
//        // Two subsequent update intervals: linear extrapolation
//        // All subsequente update intervals: local physics extrapolation.
//        if (r.interpTime >= maxLinExtrapTime_) // Hand-off to client-side physics?
//        {
//            if (rigidBody)
//            {
//                if (!noClientPhysicsHandoff_)
//                {
//                    bool objectIsInRest = (r.interpEnd.vel.LengthSq() < 1e-4f && r.interpEnd.angVel.LengthSq() < 1e-4f);
//                    // Now the local client-side physics will take over the simulation of this rigid body, but only if the object
//                    // is moving. This is because the client shouldn't wake up the object (locally) if it's stationary, but wait for the
//                    // server-side signal for that event.
//                    rigidBody->SetClientExtrapolating(objectIsInRest == false);
//                    // Give starting parameters for the simulation.
//                    rigidBody->linearVelocity.Set(r.interpEnd.vel, AttributeChange::LocalOnly);
//                    rigidBody->angularVelocity.Set(r.interpEnd.angVel, AttributeChange::LocalOnly);
//                }
//            }
//            r.interpolatorActive = false;
//            ++iter;
//            
//            // Could remove the interpolation structure here, as inter/extrapolation it is no longer active. However, it is currently
//            // used to store most recently received entity position  & velocity data.
//            //iter = state->entityInterpolations.erase(iter); // Finished interpolation.
//        }
//        else // Interpolation or linear extrapolation.
//        {
//            if (rigidBody)
//            {
//                // Ensure that the local side physics is not driving the position of this entity.
//                rigidBody->SetClientExtrapolating(false);
//
//                // Setting these is rather redundant, since Bullet doesn't simulate the entity using these variables. However, other
//                // (locally simulated) objects can collide to this entity, in which case it's good to have the proper velocities for bullet,
//                // so that the collision response simulates the appropriate forces/velocities in play.
//                float3 curVel = float3::Lerp(r.interpStart.vel, r.interpEnd.vel, Clamp01(r.interpTime));
//                // Test: To set continous velocity based on the Hermite curve, use the following:
// //               float3 curVel = HermiteDerivative(r.interpStart.pos, r.interpStart.vel*interpPeriod, r.interpEnd.pos, r.interpEnd.vel*interpPeriod, r.interpTime);
//
//                rigidBody->linearVelocity.Set(curVel, AttributeChange::LocalOnly);
//
//                ///\todo Setup angular velocity.
//                rigidBody->angularVelocity.Set(float3::zero, AttributeChange::LocalOnly);
//            }
//            ++iter;
//        }
//    }
}

void SyncManager::Update(f64 frametime)
{
    PROFILE(SyncManager_Update);

    // For the client, smoothly update all rigid bodies by interpolating.
    if (!owner_->IsServer())
        InterpolateRigidBodies(frametime, serverConnection_->syncState.Get());

    // Check if it is yet time to perform a network update tick.
    updateAcc_ += (float)frametime;
    if (updateAcc_ < updatePeriod_)
        return;

    // If multiple updates passed, update still just once.
    updateAcc_ = fmod(updateAcc_, updatePeriod_);
    
    ScenePtr scene = scene_.Lock();
    if (!scene)
        return;
    
    if (owner_->IsServer())
    {
        // If we are server, process all authenticated users

        // Then send out changes to other attributes via the generic sync mechanism.
        UserConnectionList& users = owner_->Server()->UserConnections();
        for(auto i = users.Begin(); i != users.End(); ++i)
            if ((*i)->syncState)
            {
                // As of now only native clients understand the optimized rigid body sync message.
                // This may change with future protocol versions
                if (dynamic_cast<KNetUserConnection*>(i->Get()))
                { 
                    // First send out all changes to rigid bodies.
                    // After processing this function, the bits related to rigid body states have been cleared,
                    // so the generic sync will not double-replicate the rigid body positions and velocities.
                    ReplicateRigidBodyChanges((*i).Get());
                }
                ProcessSyncState((*i).Get());
            }
    }
    else
    {
        // If we are client and the connection is current, process just the server sync state
        if (Urho3D::StaticCast<KNetUserConnection>(serverConnection_)->connection)
            ProcessSyncState(serverConnection_.Get());
    }
}

/// \todo Uncomment and fix after RigidBody component implemented
void SyncManager::ReplicateRigidBodyChanges(UserConnection* /*user*/)
{
//    PROFILE(SyncManager_ReplicateRigidBodyChanges);
//    
//    ScenePtr scene = scene_.Lock();
//    if (!scene)
//        return;
//
//    const int maxMessageSizeBytes = 1400;
//    kNet::DataSerializer ds(maxMessageSizeBytes);
//    bool msgReliable = false;
//    SceneSyncState* state = user->syncState.Get();
//
//    QHashIterator<entity_id_t, EntitySyncState*> iter(state->dirtyQueue);
//    while (iter.hasNext())
//    {
//        iter.next();
//
//        const int maxRigidBodyMessageSizeBits = 350; // An update for a single rigid body can take at most this many bits. (conservative bound)
//        // If we filled up this message, send it out and start crafting anothero one.
//        if (maxMessageSizeBytes * 8 - (int)ds.BitsFilled() <= maxRigidBodyMessageSizeBits)
//        {
//            user->Send(cRigidBodyUpdateMessage, msgReliable, true, ds);
//            ds = kNet::DataSerializer(maxMessageSizeBytes);
//            msgReliable = false;
//        }
//
//        EntitySyncState &ess = *iter.value();
//
//        if (ess.isNew || ess.removed)
//            continue; // Newly created and removed entities are handled through the traditional sync mechanism.
//
//        EntityPtr e = ess.weak.lock();
//        shared_ptr<EC_Placeable> placeable = (e.get() ? e->GetComponent<EC_Placeable>() : shared_ptr<EC_Placeable>());
//        if (!placeable.get())
//            continue;
//
//        std::map<component_id_t, ComponentSyncState>::iterator placeableComp = ess.components.find(placeable->Id());
//
//        bool transformDirty = false;
//        if (placeableComp != ess.components.end())
//        {
//            ComponentSyncState &pss = placeableComp->second;
//            if (!pss.isNew && !pss.removed) // Newly created and deleted components are handled through the traditional sync mechanism.
//            {
//                transformDirty = (pss.dirtyAttributes[0] & 1) != 0; // The Transform of an EC_Placeable is the first attibute in the component.
//                pss.dirtyAttributes[0] &= ~1;
//            }
//        }
//        bool velocityDirty = false;
//        bool angularVelocityDirty = false;
//        
//        shared_ptr<EC_RigidBody> rigidBody = e->GetComponent<EC_RigidBody>();
//        if (rigidBody)
//        {
//            std::map<component_id_t, ComponentSyncState>::iterator rigidBodyComp = ess.components.find(rigidBody->Id());
//            if (rigidBodyComp != ess.components.end())
//            {
//                ComponentSyncState &rss = rigidBodyComp->second;
//                if (!rss.isNew && !rss.removed) // Newly created and deleted components are handled through the traditional sync mechanism.
//                {
//                    velocityDirty = (rss.dirtyAttributes[1] & (1 << 5)) != 0;
//                    angularVelocityDirty = (rss.dirtyAttributes[1] & (1 << 6)) != 0;
//
//                    rss.dirtyAttributes[1] &= ~(1 << 5);
//                    rss.dirtyAttributes[1] &= ~(1 << 6);
//
//                    velocityDirty = velocityDirty && (rigidBody->linearVelocity.Get().DistanceSq(ess.linearVelocity) >= 1e-2f);
//                    angularVelocityDirty = angularVelocityDirty && (rigidBody->angularVelocity.Get().DistanceSq(ess.angularVelocity) >= 1e-1f);
//
//                    // If the object enters rest, force an update, and force the update to be sent as reliable, so that the client
//                    // is guaranteed to receive the message, and will put the object to rest, instead of extrapolating it away indefinitely.
//                    if (rigidBody->linearVelocity.Get().IsZero(1e-4f) && !ess.linearVelocity.IsZero(1e-4f))
//                    {
//                        velocityDirty = true;
//                        msgReliable = true;
//                    }
//                    if (rigidBody->angularVelocity.Get().IsZero(1e-4f) && !ess.angularVelocity.IsZero(1e-4f))
//                    {
//                        angularVelocityDirty = true;
//                        msgReliable = true;
//                    }
//                }
//            }
//        }
//
//        if (!transformDirty && !velocityDirty && !angularVelocityDirty)
//            continue;
//
//        const Transform &t = placeable->transform.Get();
//
//        float timeSinceLastSend = kNet::Clock::SecondsSinceF(ess.lastNetworkSendTime);
//        const float3 predictedClientSidePosition = ess.transform.pos + timeSinceLastSend * ess.linearVelocity;
//        float error = t.pos.DistanceSq(predictedClientSidePosition);
//        UNREFERENCED_PARAM(error)
//        // TEST: To have the server estimate how far the client has simulated, use this.
////        bool posChanged = transformDirty && (timeSinceLastSend > 0.2f || t.pos.DistanceSq(/*ess.transform.pos*/ predictedClientSidePosition) > 5e-5f);
//        bool posChanged = transformDirty && t.pos.DistanceSq(ess.transform.pos) > 1e-3f;
//        bool rotChanged = transformDirty && (t.rot.DistanceSq(ess.transform.rot) > 1e-1f);
//        bool scaleChanged = transformDirty && (t.scale.DistanceSq(ess.transform.scale) > 1e-3f);
//
//        // Detect whether to send compact or full states for each variable.
//        // 0 - don't send, 1 - send compact, 2 - send full.
//        int posSendType = posChanged ? (t.pos.Abs().MaxElement() >= 1023.f ? 2 : 1) : 0;
//        int rotSendType;
//        int scaleSendType;
//        int velSendType;
//        int angVelSendType;
//
//        float3x3 rot;
//        if (rotChanged)
//        {
//            rot = t.Orientation3x3();
//            float3 fwd = rot.Col(2);
//            float3 up = rot.Col(1);
//            float3 planeNormal = float3::unitY.Cross(rot.Col(2));
//            float d = planeNormal.Dot(rot.Col(1));
//
//            if (up.Dot(float3::unitY) >= 0.999f)
//                rotSendType = 1; // Looking upright, 1 DOF.
//            else if (Abs(d) <= 0.001f && Abs(fwd.Dot(float3::unitY)) < 0.95f && up.Dot(float3::unitY) > 0.f)
//                rotSendType = 2; // No roll, i.e. 2 DOF. Use this only if not looking too close towards the +Y axis, due to precision issues, and only when object +Y is towards world up.
//            else
//                rotSendType = 3; // Full 3 DOF
//        }
//        else
//            rotSendType = 0;
//
//        if (scaleChanged)
//        {
//            float3 s = t.scale.Abs();
//            scaleSendType = (s.MaxElement() - s.MinElement() <= 1e-3f) ? 1 : 2; // Uniform scale only?
//        }
//        else
//            scaleSendType = 0;
//
//        const float3 &linearVel = rigidBody ? rigidBody->linearVelocity.Get() : float3::zero;
//        const float3 angVel = rigidBody ? DegToRad(rigidBody->angularVelocity.Get()) : float3::zero;
//
//        velSendType = velocityDirty ? (linearVel.LengthSq() >= 64.f ? 2 : 1) : 0;
//        angVelSendType = angularVelocityDirty ? 1 : 0;
//
//        if (posSendType == 0 && rotSendType == 0 && scaleSendType == 0 && velSendType == 0 && angVelSendType == 0)
//            continue;
//
//        size_t bitIdx = ds.BitsFilled();
//        UNREFERENCED_PARAM(bitIdx)
//        ds.AddVLE<kNet::VLE8_16_32>(ess.id); // Sends max. 32 bits.
//
//        ds.AddArithmeticEncoded(8, posSendType, 3, rotSendType, 4, scaleSendType, 3, velSendType, 3, angVelSendType, 2); // Sends fixed 8 bits.
//        if (posSendType == 1) // Sends fixed 57 bits.
//        {
//            ds.AddSignedFixedPoint(11, 8, t.pos.x);
//            ds.AddSignedFixedPoint(11, 8, t.pos.y);
//            ds.AddSignedFixedPoint(11, 8, t.pos.z);
//        }
//        else if (posSendType == 2) // Sends fixed 96 bits.
//        {
//            ds.Add<float>(t.pos.x);
//            ds.Add<float>(t.pos.y);
//            ds.Add<float>(t.pos.z);
//        }        
//
//        if (rotSendType == 1) // Orientation with 1 DOF, only yaw.
//        {
//            // The transform is looking straight forward, i.e. the +y vector of the transform local space points straight towards +y in world space.
//            // Therefore the forward vector has y == 0, so send (x,z) as a 2D vector.
//            ds.AddNormalizedVector2D(rot.Col(2).x, rot.Col(2).z, 8);  // Sends fixed 8 bits.
//        }
//        else if (rotSendType == 2) // Orientation with 2 DOF, yaw and pitch.
//        {
//            float3 forward = rot.Col(2);
//            forward.Normalize();
//            ds.AddNormalizedVector3D(forward.x, forward.y, forward.z, 9, 8); // Sends fixed 17 bits.
//        }
//        else if (rotSendType == 3) // Orientation with 3 DOF, full yaw, pitch and roll.
//        {
//            Quat o = t.Orientation();
//
//            float3 axis;
//            float angle;
//            o.ToAxisAngle(axis, angle);
//            if (angle >= 3.141592654f) // Remove the quaternion double cover representation by constraining angle to [0, pi].
//            {
//                axis = -axis;
//                angle = 2.f * 3.141592654f - angle;
//            }
//
//            // Sends 10-31 bits.
//            u32 quantizedAngle = ds.AddQuantizedFloat(0, 3.141592654f, 10, angle);
//            if (quantizedAngle != 0)
//                ds.AddNormalizedVector3D(axis.x, axis.y, axis.z, 11, 10);
//        }
//
//        if (scaleSendType == 1) // Sends fixed 32 bytes.
//        {
//            ds.Add<float>(t.scale.x);
//        }
//        else if (scaleSendType == 2) // Sends fixed 96 bits.
//        {
//            ds.Add<float>(t.scale.x);
//            ds.Add<float>(t.scale.y);
//            ds.Add<float>(t.scale.z);
//        }
//
//        if (velSendType == 1) // Sends fixed 32 bits.
//        {
//            ds.AddVector3D(linearVel.x, linearVel.y, linearVel.z, 11, 10, 3, 8);
//            ess.linearVelocity = linearVel;
//        }
//        else if (velSendType == 2) // Sends fixed 39 bits.
//        {
//            ds.AddVector3D(linearVel.x, linearVel.y, linearVel.z, 11, 10, 10, 8);
//            ess.linearVelocity = linearVel;
//        }
//
//        if (angVelSendType == 1)
//        {
//            Quat o = Quat::FromEulerZYX(angVel.z, angVel.y, angVel.x);
//
//            float3 axis;
//            float angle;
//            o.ToAxisAngle(axis, angle);
//            if (angle >= 3.141592654f) // Remove the quaternion double cover representation by constraining angle to [0, pi].
//            {
//                axis = -axis;
//                angle = 2.f * 3.141592654f - angle;
//            }
//             // Sends at most 31 bits.
//            u32 quantizedAngle = ds.AddQuantizedFloat(0, 3.141592654f, 10, angle);
//            if (quantizedAngle != 0)
//                ds.AddNormalizedVector3D(axis.x, axis.y, axis.z, 11, 10);
//
//            ess.angularVelocity = angVel;
//        }
//        if (posSendType != 0)
//            ess.transform.pos = t.pos;
//        if (rotSendType != 0)
//            ess.transform.rot = t.rot;
//        if (scaleSendType != 0)
//            ess.transform.scale = t.scale;
//
////        std::cout << "pos: " << posSendType << ", rot: " << rotSendType << ", scale: " << scaleSendType << ", vel: " << velSendType << ", angvel: " << angVelSendType << std::endl;
//
//        size_t bitsEnd = ds.BitsFilled();
//        UNREFERENCED_PARAM(bitsEnd)
//        ess.lastNetworkSendTime = kNet::Clock::Tick();
//    }
//    if (ds.BytesFilled() > 0)
//        user->Send(cRigidBodyUpdateMessage, msgReliable, true, ds);
}

void SyncManager::HandleRigidBodyChanges(UserConnection* source, kNet::packet_id_t packetId, const char* data, size_t numBytes)
{
    ScenePtr scene = scene_.Lock();
    if (!scene)
        return;

    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    if (!state)
        return;

    kNet::DataDeserializer dd(data, numBytes);
    while(dd.BitsLeft() >= 9)
    {
        u32 entityID = dd.ReadVLE<kNet::VLE8_16_32>();
        EntityPtr e = scene->EntityById(entityID);
        Placeable* placeable = e ? e->Component<Placeable>().Get() : nullptr;
//        shared_ptr<EC_RigidBody> rigidBody = e ? e->GetComponent<EC_RigidBody>() : shared_ptr<EC_RigidBody>();
        Transform t = e ? placeable->transform.Get() : Transform();
//
//        float3 newLinearVel = rigidBody ? rigidBody->linearVelocity.Get() : float3::zero;
        float3 newLinearVel(float3::zero);
//
//        // If the server omitted linear velocity, interpolate towards the last received linear velocity.
//        std::map<entity_id_t, RigidBodyInterpolationState>::iterator iter = e ? serverConnection_->syncState->entityInterpolations.find(entityID) : serverConnection_->syncState->entityInterpolations.end();
//        if (iter != serverConnection_->syncState->entityInterpolations.end())
//            newLinearVel = iter->second.interpEnd.vel;

        int posSendType;
        int rotSendType;
        int scaleSendType;
        int velSendType;
        int angVelSendType;
        dd.ReadArithmeticEncoded(8, posSendType, 3, rotSendType, 4, scaleSendType, 3, velSendType, 3, angVelSendType, 2);

        if (posSendType == 1)
        {
            t.pos.x = dd.ReadSignedFixedPoint(11, 8);
            t.pos.y = dd.ReadSignedFixedPoint(11, 8);
            t.pos.z = dd.ReadSignedFixedPoint(11, 8);
        }
        else if (posSendType == 2)
        {
            t.pos.x = dd.Read<float>();
            t.pos.y = dd.Read<float>();
            t.pos.z = dd.Read<float>();
        }

        if (rotSendType == 1) // 1 DOF
        {
            float3 forward;
            dd.ReadNormalizedVector2D(8, forward.x, forward.z);
            forward.y = 0.f;
            float3x3 orientation = float3x3::LookAt(float3::unitZ, forward, float3::unitY, float3::unitY);
            t.SetOrientation(orientation);
        }
        else if (rotSendType == 2)
        {
            float3 forward;
            dd.ReadNormalizedVector3D(9, 8, forward.x, forward.y, forward.z);

            float3x3 orientation = float3x3::LookAt(float3::unitZ, forward, float3::unitY, float3::unitY);
            t.SetOrientation(orientation);

        }
        else if (rotSendType == 3)
        {
            // Read the quantized float manually, without a call to ReadQuantizedFloat, to be able to compare the quantized bit pattern.
            u32 quantizedAngle = dd.ReadBits(10);
            if (quantizedAngle != 0)
            {
                float angle = quantizedAngle * 3.141592654f / (float)((1 << 10) - 1);
                float3 axis;
                dd.ReadNormalizedVector3D(11, 10, axis.x, axis.y, axis.z);
                t.SetOrientation(Quat(axis, angle));
            }
            else
                t.SetOrientation(Quat::identity);
        }

        if (scaleSendType == 1)
            t.scale = float3::FromScalar(dd.Read<float>());
        else if (scaleSendType == 2)
        {
            t.scale.x = dd.Read<float>();
            t.scale.y = dd.Read<float>();
            t.scale.z = dd.Read<float>();
        }

        if (velSendType == 1)
            dd.ReadVector3D(11, 10, 3, 8, newLinearVel.x, newLinearVel.y, newLinearVel.z);
        else if (velSendType == 2)
            dd.ReadVector3D(11, 10, 10, 8, newLinearVel.x, newLinearVel.y, newLinearVel.z);

//        float3 newAngVel = rigidBody ? rigidBody->angularVelocity.Get() : float3::zero;
        float3 newAngVel(float3::zero);

        if (angVelSendType == 1)
        {
            // Read the quantized float manually, without a call to ReadQuantizedFloat, to be able to compare the quantized bit pattern.
            u32 quantizedAngle = dd.ReadBits(10);
            if (quantizedAngle != 0)
            {
                float angle = quantizedAngle * 3.141592654f / (float)((1 << 10) - 1);
                float3 axis;
                dd.ReadNormalizedVector3D(11, 10, axis.x, axis.y, axis.z);
                Quat q(axis, angle);
                newAngVel = q.ToEulerZYX();
                Swap(newAngVel.z, newAngVel.x);
                newAngVel = RadToDeg(newAngVel);
            }
        }

        if (!e) // Discard this message - we don't have the entity in our scene to which the message applies to.
            continue;

        // Did anything change?
        if (posSendType != 0 || rotSendType != 0 || scaleSendType != 0 || velSendType != 0 || angVelSendType != 0)
        {
            if (placeable)
            {
                // Record the update time for calculating the update interval
                // Default update interval if state not found or interval not measured yet
                EntitySyncState &entityState = state->entities[e->Id()];
                float updateInterval = updatePeriod_;
                entityState.UpdateReceived();
                if (entityState.avgUpdateInterval > 0.0f)
                    updateInterval = entityState.avgUpdateInterval;

                // Add a fudge factor in case there is jitter in packet receipt or the server is too taxed
                updateInterval *= 1.25f;

                /// \todo This performs a heap allocation per each transform change received from server, which is not ideal
                IAttribute* endValue = placeable->transform.Clone();
                Attribute<Transform>* endValueTransform = static_cast<Attribute<Transform>*>(endValue);
                endValueTransform->Set(t, AttributeChange::Disconnected);

                scene->StartAttributeInterpolation(&placeable->transform, endValueTransform, updateInterval);
            }
        }
    }

//            // Create or update the interpolation state.
//            Transform orig = placeable->transform.Get();
//
//            std::map<entity_id_t, RigidBodyInterpolationState>::iterator iter = serverConnection_->syncState->entityInterpolations.find(entityID);
//            if (iter != serverConnection_->syncState->entityInterpolations.end())
//            {
//                RigidBodyInterpolationState &interp = iter->second;
//
//                KNetUserConnection* kNetSource = dynamic_cast<KNetUserConnection*>(source);
//                kNet::MessageConnection* conn = kNetSource ? kNetSource->connection.ptr() : (kNet::MessageConnection*)0;
//                if (conn && conn->GetSocket() && conn->GetSocket()->TransportLayer() == kNet::SocketOverUDP)
//                {
//                    if (kNet::PacketIDIsNewerThan(interp.lastReceivedPacketCounter, packetId))
//                        continue; // This is an out-of-order received packet. Ignore it. (latest-data-guarantee)
//                }
//                
//                interp.lastReceivedPacketCounter = packetId;
//
//                const float interpPeriod = updatePeriod_; // Time in seconds how long interpolating the Hermite spline from [0,1] should take.
//                float3 curVel;
//
//                if (interp.interpTime < 1.0f)
//                    curVel = HermiteDerivative(interp.interpStart.pos, interp.interpStart.vel*interpPeriod, interp.interpEnd.pos, interp.interpEnd.vel*interpPeriod, interp.interpTime);
//                else
//                    curVel = interp.interpEnd.vel;
//                float3 curAngVel = float3::zero; ///\todo
//                interp.interpStart.pos = orig.pos;
//                if (posSendType != 0)
//                    interp.interpEnd.pos = t.pos;
//                interp.interpStart.rot = orig.Orientation();
//                if (rotSendType != 0)
//                    interp.interpEnd.rot = t.Orientation();
//                interp.interpStart.scale = orig.scale;
//                if (scaleSendType != 0)
//                    interp.interpEnd.scale = t.scale;
//                interp.interpStart.vel = curVel;
//                if (velSendType != 0)
//                    interp.interpEnd.vel = newLinearVel;
//                interp.interpStart.angVel = curAngVel;
//                if (angVelSendType != 0)
//                    interp.interpEnd.angVel = newAngVel;
//                interp.interpTime = 0.f;
//                interp.interpolatorActive = true;
//
//                // Objects without a rigidbody, or with mass 0 never extrapolate (objects with mass 0 are stationary for Bullet).
//                const bool isNewtonian = rigidBody && rigidBody->mass.Get() > 0;
//                if (!isNewtonian)
//                    interp.interpStart.vel = interp.interpEnd.vel = float3::zero;
//            }
//            else
//            {
//                RigidBodyInterpolationState interp;
//                interp.interpStart.pos = orig.pos;
//                interp.interpEnd.pos = t.pos;
//                interp.interpStart.rot = orig.Orientation();
//                interp.interpEnd.rot = t.Orientation();
//                interp.interpStart.scale = orig.scale;
//                interp.interpEnd.scale = t.scale;
//                interp.interpStart.vel = rigidBody ? rigidBody->linearVelocity.Get() : float3::zero;
//                interp.interpEnd.vel = newLinearVel;
//                interp.interpStart.angVel = rigidBody ? rigidBody->angularVelocity.Get() : float3::zero;
//                interp.interpEnd.angVel = newAngVel;
//                interp.interpTime = 0.f;
//                interp.lastReceivedPacketCounter = packetId;
//                interp.interpolatorActive = true;
//                serverConnection_->syncState->entityInterpolations[entityID] = interp;
//            }
//        }
//    }
}

void SyncManager::HandleEditEntityProperties(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding EditEntityProperties message");
        return;
    }
    
    bool isServer = owner_->IsServer();
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    
    if (!ValidateAction(source, cEditEntityPropertiesMessage, entityID))
        return;

    EntitySyncState &entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (entity && !scene->AllowModifyEntity(source, entity.Get())) // check if allowed to modify this entity.
        return;
    if (!entity)
    {
        LogWarning("Entity " + String(entityID) + " not found for EditAttributes message");
        return;
    }
    
    // For now the edit properties consist only of the temporary flag
    bool newTemporary = ds.Read<u8>() ? true : false;
    entity->SetTemporary(newTemporary, change);
    
    // Remove the properties dirty bit from sender's syncstate so that we do not echo the change back
    entityState.hasPropertyChanges = false;
}

void SyncManager::HandleSetEntityParent(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding SetEntityParent message");
        return;
    }
    
    bool isServer = owner_->IsServer();
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.Read<u32>();
    entity_id_t parentEntityID = ds.Read<u32>();
    
    // If either entity ID is an unacked range ID, try to convert
    if (isServer)
    {
        if (entityID >= UniqueIdGenerator::FIRST_UNACKED_ID && entityID < UniqueIdGenerator::FIRST_LOCAL_ID)
        {
            if (source->unackedIdsToRealIds.find(entityID) != source->unackedIdsToRealIds.end())
                entityID = source->unackedIdsToRealIds[entityID];
            else
            {
                LogWarning("Client sent unknown unacked entity ID " + String(entityID) + " in SetEntityParent message");
                return;
            }
        }
        if (parentEntityID >= UniqueIdGenerator::FIRST_UNACKED_ID && parentEntityID < UniqueIdGenerator::FIRST_LOCAL_ID)
        {
            if (source->unackedIdsToRealIds.find(parentEntityID) != source->unackedIdsToRealIds.end())
                parentEntityID = source->unackedIdsToRealIds[parentEntityID];
            else
            {
                LogWarning("Client sent unknown unacked parent entity ID " + String(parentEntityID) + " in SetEntityParent message");
                return;
            }
        }
    }    
    
    if (!ValidateAction(source, cSetEntityParentMessage, entityID))
        return;
    
    EntitySyncState &entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (entity && !scene->AllowModifyEntity(source, entity.Get()))
        return;
    if (!entity)
    {
        LogWarning("Entity " + String(entityID) + " not found for SetEntityParent message");
        return;
    }

    EntityPtr parentEntity = (parentEntityID ? scene->EntityById(parentEntityID) : EntityPtr());
    if (parentEntityID && !parentEntity)
    {
        LogWarning("Parent entity " + String(parentEntityID) + " not found for SetEntityParent message");
        return;
    }
    
    entity->SetParent(parentEntity, change);
    
    // Remove the properties dirty bit from sender's syncstate so that we do not echo the change back
    entityState.hasParentChange = false;
}

void SyncManager::HandleRegisterComponentType(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    
    bool isServer = owner_->IsServer();
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    if (!ValidateAction(source, cRegisterComponentTypeMessage, 0))
        return;

    kNet::DataDeserializer ds(data, numBytes);
    ComponentDesc desc;
    desc.typeId = ds.ReadVLE<kNet::VLE8_16_32>();
    desc.typeName = String(ds.ReadString().c_str());

    // On client, remember the component types server has sent, so that we don't unnecessarily echo them back
    if (!isServer)
        componentTypesFromServer_.insert(desc.typeId);

    // If component type already exists as actual C++ component, no action necessary
    // However, allow to update an earlier custom component description
    SceneAPI* sceneAPI = framework_->Scene();
    if (sceneAPI->IsComponentFactoryRegistered(desc.typeName))
        return;

    size_t numAttrs = ds.ReadVLE<kNet::VLE8_16_32>();
    for (size_t i = 0; i < numAttrs; ++i)
    {
        AttributeDesc attrDesc;
        attrDesc.typeName = sceneAPI->AttributeTypeNameForTypeId(ds.Read<u8>());
        /// \todo Use UTF-8 encoding
        attrDesc.id = String(ds.ReadString().c_str());
        attrDesc.name = String(ds.ReadString().c_str());
        desc.attributes.Push(attrDesc);
    }

    // Do not send back to sender
    componentTypeSender_ = source;
    sceneAPI->RegisterPlaceholderComponentType(desc, change);
    componentTypeSender_ = 0;
}

void SyncManager::ProcessSyncState(UserConnection* user)
{
    PROFILE(SyncManager_ProcessSyncState);
    
    ScenePtr scene = scene_.Lock();
    bool isServer = owner_->IsServer();

    SceneSyncState* state = user->syncState.Get();
    
    // Send knowledge of registered placeholder components to the remote peer
    if (user->ProtocolVersion() >= ProtocolCustomComponents && state->NeedSendPlaceholderComponents())
    {
        SceneAPI* sceneAPI = framework_->Scene();
        const SceneAPI::PlaceholderComponentTypeMap& descs = sceneAPI->PlaceholderComponentTypes();
        for (auto i = descs.Begin(); i != descs.End(); ++i)
        {
            if (isServer || componentTypesFromServer_.find(i->first_) == componentTypesFromServer_.end())
                ReplicateComponentType(i->first_, user);
        }
        state->MarkPlaceholderComponentsSent();
    }

    // Process the state's dirty entity queue.
    /// \todo Limit and prioritize the data sent. For now the whole queue is processed, regardless of whether the connection is being saturated.
    if (state->dirtyQueue.Size() > 0)
    {
        for (auto iter = state->dirtyQueue.Begin() ; iter != state->dirtyQueue.End() ; ++iter)
            ProcessEntitySyncState(isServer, user, scene.Get(), state, iter->second_);

        
        state->dirtyQueue.Clear();
    }

    // Send queued entity actions after scene sync
    if (state->queuedActions.size())
    {
        for (size_t i = 0; i < state->queuedActions.size(); ++i)
            user->Send(state->queuedActions[i]);

        state->queuedActions.clear();
    }
}

void SyncManager::ProcessEntitySyncState(bool isServer, UserConnection* user, Scene *scene, SceneSyncState *sceneState, EntitySyncState *entityState)
{
    entityState->isInQueue = false;

    unsigned sceneId = 0;       /// @todo Replace with proper scene ID once multiscene support is in place.
    bool removeState = false;

    EntityPtr entity = entityState->weak.Lock();
    if (!entity)
    {
        if (!entityState->removed)
            LogWarning("Entity " + String(entityState->id) + " has gone missing from the scene without the remove properly signalled. Removing from replication state");
        entityState->isNew = false;
        removeState = true;
    }
    else
    {
        // Make sure we don't send data for local entities, or unacked entities after the create
        if (entity->IsLocal() || (!entityState->isNew && entity->IsUnacked()))
            return;
    }
    
    // Remove entity
    if (entityState->removed)
    {        
        /* This code is commented out as it seems this situation should not be possible.
           If you look at SceneSyncState::MarkEntityRemoved(entity_id_t id) where entityState->removed is set to true,
           if there is a ->isNew on the same state, it is just forgotten on the spot and we will never reach this code.
           @cadaver could validate that this seems correct, after which the commented code below should be removed!

        // If we have both new & removed flags on the entity, it will probably result in buggy behaviour
        if (entityState->isNew)
        {
            LogWarning("Entity " + String::number(entityState->id) + " queued for both deletion and creation. Buggy behaviour will possibly result!");
            // The delete has been processed. Do not remember it anymore, but requeue the state for creation
            entityState->removed = false;
            removeState = false;
            // fix for new QHash based iteration if enabled back! 
            // sceneState->dirtyQueue.push_back(&entityState);
            entityState->isInQueue = true;
        }
        else
            removeState = true;*/

        removeState = true;

        kNet::DataSerializer ds(removeEntityBuffer_, NUMELEMS(removeEntityBuffer_));
        ds.AddVLE<kNet::VLE8_16_32>(sceneId);
        ds.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
        user->Send(cRemoveEntityMessage, true, true, ds);
    }
    // New entity
    else if (entityState->isNew)
    {
        // Check if parent is dirty as a new state and send it first.
        // Must be done prior to below code using the createEntityBuffer_.
        if (user->ProtocolVersion() >= ProtocolHierarchicScene)
        {
            entity_id_t parentId = (entity->Parent() ? entity->Parent()->Id() : 0);

            // Check if parent is dirty as a new state and send it first.
            if (parentId > 0 && sceneState->dirtyQueue.Contains(parentId))
            {
                /* This will clear the .isNew etc. booleans in the queue,
                   once the main iteration reaches this parent it will do no
                   additional work. This also has a recursive nature in that if
                   the parent chain is deeper than one level, it will recurse
                   here untill a unparented Entity is found and sent them in the
                   correct order. */
                EntitySyncState *parentState = sceneState->dirtyQueue[parentId];
                if (parentState && parentState->isNew)
                    ProcessEntitySyncState(isServer, user, scene, sceneState, parentState);
            }
        }
        
        kNet::DataSerializer ds(createEntityBuffer_, NUMELEMS(createEntityBuffer_));
        
        // Entity identification and temporary flag
        ds.AddVLE<kNet::VLE8_16_32>(sceneId);
        ds.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
        // Do not write the temporary flag as a bit to not desync the byte alignment at this point, as a lot of data potentially follows
        ds.Add<u8>(entity->IsTemporary() ? 1 : 0);
        // If hierarchic scene is supported, send parent entity ID or 0 if unparented. Note that this is a full 32bit ID to handle the unacked range if necessary
        if (user->ProtocolVersion() >= ProtocolHierarchicScene)
        {
            if (entity->Parent() && entity->Parent()->IsLocal())
                LogWarning("Replicated entity " + String(entityState->id) + " is parented to a local entity, can not replicate parenting properly over the network");

            ds.Add<u32>(entity->Parent() ? entity->Parent()->Id() : 0);
        }
        
        const Entity::ComponentMap& components = entity->Components();
        // Count the amount of replicated components
        uint numReplicatedComponents = 0;
        for (auto i = components.Begin(); i != components.End(); ++i)
        {
            if (i->second_->IsReplicated())
                ++numReplicatedComponents;
        }
        ds.AddVLE<kNet::VLE8_16_32>(numReplicatedComponents);
        
        // Serialize each replicated component
        bool bufferValid = true;
        for (auto i = components.Begin(); i != components.End(); ++i)
        {
            ComponentPtr comp = i->second_;
            if (!comp->IsReplicated())
                continue;
            if (bufferValid && !WriteComponentFullUpdate(ds, comp))
            {
                bufferValid = false;
                ds.ResetFill();
            }
            // Mark the component undirty in the receiver's syncstate
            sceneState->MarkComponentProcessed(entity->Id(), comp->Id());
        }
        if (bufferValid)
            user->Send(cCreateEntityMessage, true, true, ds);

        // The create has been processed fully. Clear dirty flags.
        sceneState->MarkEntityProcessed(entity->Id());

        /** Client failed to serialize new entity. We need to forcefully
            destroy this entity or it will cause problems later. */
        if (!bufferValid && !isServer)
        {
            LogError("SyncManager: Failed to send new Entity to the server due to invalid buffer state. " + entity->ToString() + " will be forcefully destroyed from Scene.");
            sceneState->RemoveFromQueue(entity->Id());
            sceneState->entities.erase(entity->Id());
            scene->RemoveEntity(entity->Id(), AttributeChange::LocalOnly);
        }
    }
    else if (entity)
    {
        if (!entityState->dirtyQueue.Empty())
        {
            // Components or attributes have been added, changed, or removed. Prepare the dataserializers
            kNet::DataSerializer removeCompsDs(removeCompsBuffer_, NUMELEMS(removeCompsBuffer_));
            kNet::DataSerializer removeAttrsDs(removeAttrsBuffer_, NUMELEMS(removeAttrsBuffer_));
            kNet::DataSerializer createCompsDs(createCompsBuffer_, NUMELEMS(createCompsBuffer_));
            kNet::DataSerializer createAttrsDs(createAttrsBuffer_, NUMELEMS(createAttrsBuffer_));
            kNet::DataSerializer editAttrsDs(editAttrsBuffer_, NUMELEMS(editAttrsBuffer_));

            while (!entityState->dirtyQueue.Empty())
            {
                ComponentSyncState& compState = *entityState->dirtyQueue.Front();
                entityState->dirtyQueue.PopFront();
                compState.isInQueue = false;
                
                ComponentPtr comp = entity->ComponentById(compState.id);
                bool removeCompState = false;
                if (!comp)
                {
                    if (!compState.removed)
                        LogWarning("Component " + String(compState.id) + " of " + entity->ToString() + " has gone missing from the scene without the remove properly signalled. Removing from client replication state->");
                    compState.isNew = false;
                    removeCompState = true;
                }
                else
                {
                    // Make sure we don't send data for local components, or unacked components after the create
                    if (comp->IsLocal() || (!compState.isNew && comp->IsUnacked()))
                        continue;
                }
                
                // Remove component
                if (compState.removed)
                {
                    removeCompState = true;
                    
                    // If first component, write the entity ID first
                    if (!removeCompsDs.BytesFilled())
                    {
                        removeCompsDs.AddVLE<kNet::VLE8_16_32>(sceneId);
                        removeCompsDs.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
                    }
                    // Then add component ID
                    removeCompsDs.AddVLE<kNet::VLE8_16_32>(compState.id & UniqueIdGenerator::LAST_REPLICATED_ID);
                }
                // New component
                else if (compState.isNew)
                {
                    // If first component, write the entity ID first
                    if (!createCompsDs.BytesFilled())
                    {
                        createCompsDs.AddVLE<kNet::VLE8_16_32>(sceneId);
                        createCompsDs.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
                    }
                    // Then add the component data
                    if (!WriteComponentFullUpdate(createCompsDs, comp))
                        createCompsDs.ResetFill();
                    // Mark the component undirty in the receiver's syncstate
                    sceneState->MarkComponentProcessed(entity->Id(), comp->Id());
                }
                // Added/removed/edited attributes
                else if (comp)
                {
                    const AttributeVector& attrs = comp->Attributes();

                    bool attrBufferValid = true;
                    for (auto i = compState.newAndRemovedAttributes.Begin(); i != compState.newAndRemovedAttributes.End(); ++i)
                    {
                        u8 attrIndex = i->first_;
                        // Clear the corresponding dirty flags, so that we don't redundantly send attribute edited data.
                        compState.dirtyAttributes[attrIndex >> 3] &= ~(1 << (attrIndex & 7));
                        
                        if (i->second_)
                        {
                            // Create attribute. Make sure it exists and is dynamic.
                            if (attrIndex >= attrs.Size() || !attrs[attrIndex])
                                LogError("CreateAttribute for nonexisting attribute index " + String((int)attrIndex) + " was queued for component " + comp->TypeName() + " in " + entity->ToString() + ". Discarding.");
                            else if (!attrs[attrIndex]->IsDynamic())
                                LogError("CreateAttribute for a static attribute index " + String((int)attrIndex) + " was queued for component " + comp->TypeName() + " in " + entity->ToString() + ". Discarding.");
                            else
                            {
                                if (attrBufferValid)
                                {
                                    // If first attribute, write the entity ID first
                                    if (!createAttrsDs.BytesFilled())
                                    {
                                        createAttrsDs.AddVLE<kNet::VLE8_16_32>(sceneId);
                                        createAttrsDs.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
                                    }

                                    IAttribute* attr = attrs[attrIndex];
                                    createAttrsDs.AddVLE<kNet::VLE8_16_32>(compState.id & UniqueIdGenerator::LAST_REPLICATED_ID);
                                    createAttrsDs.Add<u8>(attrIndex); // Index
                                    createAttrsDs.Add<u8>((u8)attr->TypeId());
                                    createAttrsDs.AddString(attr->Name().CString());
                                    attr->ToBinary(createAttrsDs);

                                    attrBufferValid = ValidateAttributeBuffer(false, createAttrsDs, comp);
                                }
                            }
                        }
                        else
                        {
                            // Remove attribute
                            // If first attribute, write the entity ID first
                            if (!removeAttrsDs.BytesFilled())
                            {
                                removeAttrsDs.AddVLE<kNet::VLE8_16_32>(sceneId);
                                removeAttrsDs.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
                            }
                            removeAttrsDs.AddVLE<kNet::VLE8_16_32>(compState.id & UniqueIdGenerator::LAST_REPLICATED_ID);
                            removeAttrsDs.Add<u8>(attrIndex);
                        }
                    }
                    compState.newAndRemovedAttributes.Clear();

                    // Buffer in invalid state, reset data so it wont be sent to network.
                    if (!attrBufferValid)
                        createAttrsDs.ResetFill();

                    // Now, if remaining dirty bits exist, they must be sent in the edit attributes message. These are the majority of our network data.
                    changedAttributes_.clear();
                    unsigned numBytes = ((unsigned)attrs.Size() + 7) >> 3;
                    for (unsigned ib = 0; ib < numBytes; ++ib)
                    {
                        u8 byte = compState.dirtyAttributes[ib];
                        if (byte)
                        {
                            for (unsigned j = 0; j < 8; ++j)
                            {
                                if (byte & (1 << j))
                                {
                                    u8 attrIndex = (u8)((ib * 8) + j);
                                    if (attrIndex < attrs.Size() && attrs[attrIndex])
                                        changedAttributes_.push_back(attrIndex);
                                    else
                                        LogError("Attribute change for a nonexisting attribute index " + String((int)attrIndex) + " was queued for component " + comp->TypeName() + " in " + entity->ToString() + ". Discarding.");
                                }
                            }
                        }
                    }
                    if (changedAttributes_.size())
                    {
                        /// @todo HACK for web clients while ReplicateRigidBodyChanges() is not implemented! 
                        /// Don't send out minuscule pos/rot/scale changes as it spams the network.
                        bool sendChanges = true;
                        if (dynamic_cast<KNetUserConnection*>(user) == 0)
                        {
                            if (comp->TypeId() == Placeable::TypeIdStatic() && changedAttributes_.size() == 1 && changedAttributes_[0] == 0)
                            {
                                // Placeable::Transform is the only change!
                                Placeable *placeable = dynamic_cast<Placeable*>(comp.Get());
                                if (placeable)
                                {
                                    const Transform &t = placeable->transform.Get();
                                    bool posChanged = (t.pos.DistanceSq(entityState->transform.pos) > 1e-3f);
                                    bool rotChanged = (t.rot.DistanceSq(entityState->transform.rot) > 1e-1f);
                                    bool scaleChanged = (t.scale.DistanceSq(entityState->transform.scale) > 1e-3f);
                            
                                    if (!posChanged && !rotChanged && !scaleChanged) // Dont send anything!
                                    {
                                        //qDebug() << "EC_Placeable too small changes: " << t.pos.DistanceSq(entityState->transform.pos) << t.rot.DistanceSq(entityState->transform.rot) << t.scale.DistanceSq(entityState->transform.scale);
                                        sendChanges = false;
                                    }
                                    else
                                        entityState->transform = t; // Lets send the update. Update transform for the next above comparison.
                                }
                            }
                        }

                        if (sendChanges)
                        {
                            // If first component for which attribute changes are sent, write the entity ID first
                            if (!editAttrsDs.BytesFilled())
                            {
                                editAttrsDs.AddVLE<kNet::VLE8_16_32>(sceneId);
                                editAttrsDs.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
                            }
                            editAttrsDs.AddVLE<kNet::VLE8_16_32>(compState.id & UniqueIdGenerator::LAST_REPLICATED_ID);
                        
                            // Create a nested dataserializer for the actual attribute data, so we can skip components
                            kNet::DataSerializer attrDataDs(attrDataBuffer_, NUMELEMS(attrDataBuffer_));
                        
                            // There are changed attributes. Check if it is more optimal to send attribute indices, or the whole bitmask
                            unsigned bitsMethod1 = (unsigned)changedAttributes_.size() * 8 + 8;
                            unsigned bitsMethod2 = (unsigned)attrs.Size();
                            // Method 1: indices
                            if (bitsMethod1 <= bitsMethod2)
                            {
                                attrDataDs.Add<kNet::bit>(0);
                                attrDataDs.Add<u8>((u8)changedAttributes_.size());
                                for (unsigned i = 0; i < changedAttributes_.size(); ++i)
                                {
                                    attrDataDs.Add<u8>(changedAttributes_[i]);
                                    attrs[changedAttributes_[i]]->ToBinary(attrDataDs);
                                }
                            }
                            // Method 2: bitmask
                            else
                            {
                                attrDataDs.Add<kNet::bit>(1);
                                for (unsigned i = 0; i < attrs.Size(); ++i)
                                {
                                    if (compState.dirtyAttributes[i >> 3] & (1 << (i & 7)))
                                    {
                                        attrDataDs.Add<kNet::bit>(1);
                                        attrs[i]->ToBinary(attrDataDs);
                                    }
                                    else
                                        attrDataDs.Add<kNet::bit>(0);
                                }
                            }
                            // Add the attribute data array to the main serializer
                            if (ValidateAttributeBuffer(false, attrDataDs, comp))
                            {
                                editAttrsDs.AddVLE<kNet::VLE8_16_32>((u32)attrDataDs.BytesFilled());
                                editAttrsDs.AddArray<u8>((unsigned char*)attrDataBuffer_, (u32)attrDataDs.BytesFilled());

                                if (!ValidateAttributeBuffer(false, editAttrsDs, comp, NUMELEMS(editAttrsBuffer_)))
                                    editAttrsDs.ResetFill();
                            }
                            else
                            {
                                attrDataDs.ResetFill();
                                editAttrsDs.ResetFill();
                            }
                        }

                        // Now zero out all remaining dirty bits
                        for (unsigned i = 0; i < numBytes; ++i)
                            compState.dirtyAttributes[i] = 0;
                    }
                }
                
                if (removeCompState)
                    entityState->components.erase(compState.id);
            }
            
            // Send the messages which have data
            if (removeCompsDs.BytesFilled())
                user->Send(cRemoveComponentsMessage, true, true, removeCompsDs);

            if (removeAttrsDs.BytesFilled())
                user->Send(cRemoveAttributesMessage, true, true, removeAttrsDs);

            if (createCompsDs.BytesFilled())
                user->Send(cCreateComponentsMessage, true, true, createCompsDs);

            if (createAttrsDs.BytesFilled())
                user->Send(cCreateAttributesMessage, true, true, createAttrsDs);

            if (editAttrsDs.BytesFilled())
                user->Send(cEditAttributesMessage, true, true, editAttrsDs);
        }
        
        // Check if entity has other property changes (temporary flag)
        if (entityState->hasPropertyChanges)
        {
            kNet::DataSerializer editPropertiesDs(editAttrsBuffer_, NUMELEMS(editAttrsBuffer_));
            editPropertiesDs.AddVLE<kNet::VLE8_16_32>(sceneId);
            editPropertiesDs.AddVLE<kNet::VLE8_16_32>(entityState->id & UniqueIdGenerator::LAST_REPLICATED_ID);
            editPropertiesDs.Add<u8>(entity->IsTemporary() ? 1 : 0);
            user->Send(cEditEntityPropertiesMessage, true, true, editPropertiesDs);
        }
        if (entityState->hasParentChange && user->ProtocolVersion() >= ProtocolHierarchicScene)
        {
            EntityPtr parent = entity->Parent();
            kNet::DataSerializer editParentDs(editAttrsBuffer_, 1024);
            editParentDs.AddVLE<kNet::VLE8_16_32>(sceneId);
            editParentDs.Add<u32>(entityState->id);
            editParentDs.Add<u32>(parent ? parent->Id() : 0);
            user->Send(cSetEntityParentMessage, true, true, editParentDs);
        }
        
        // The entity has been processed fully. Clear dirty flags.
        sceneState->MarkEntityProcessed(entity->Id());
    }
    
    // Entity removal has been sent to the client, remove it from the SceneState.
    if (removeState)
        sceneState->entities.erase(entityState->id);
}

bool SyncManager::ValidateAction(UserConnection* source, unsigned /*messageID*/, entity_id_t /*entityID*/)
{
    assert(source);
    
    // For now, always trust scene actions from server
    if (!owner_->IsServer())
        return true;
    
    // And for now, always also trust scene actions from clients, if they are known and authenticated
    if (source->properties["authenticated"].GetBool() != true)
        return false;
    
    return true;
}

void SyncManager::HandleCameraOrientation(UserConnection* user, const char* data, size_t numBytes)
{
    assert(user);

    ScenePtr scene = scene_.Lock();

    if (!scene)
        return;

    kNet::DataDeserializer dd(data, numBytes);
    
    Quat orientation;
    float3 clientpos;

    //Read the position of the client from the message
    orientation.x = dd.ReadSignedFixedPoint(11, 8);
    orientation.y = dd.ReadSignedFixedPoint(11, 8);
    orientation.z = dd.ReadSignedFixedPoint(11, 8);
    orientation.w = dd.ReadSignedFixedPoint(11, 8);

    clientpos.x = dd.ReadSignedFixedPoint(11, 8);
    clientpos.y = dd.ReadSignedFixedPoint(11, 8);
    clientpos.z = dd.ReadSignedFixedPoint(11, 8);

    if(!user->syncState->locationInitialized) //If this is the first camera update, save it to the initialPosition variable
    {
        user->syncState->initialLocation = clientpos;
        user->syncState->initialOrientation = orientation;
        user->syncState->locationInitialized = true;
    }

    user->syncState->clientOrientation = orientation;
    user->syncState->clientLocation = clientpos;
}

void SyncManager::HandleCreateEntity(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding CreateEntity message");
        return;
    }

    if (!scene->AllowModifyEntity(source, 0)) //should be 'ModifyScene', but ModifyEntity is now the signal that covers all
        return;

    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    bool isServer = owner_->IsServer();
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    entity_id_t senderEntityID = entityID;
    
    if (!ValidateAction(source, cCreateEntityMessage, entityID))
        return;
    
    // If client gets a entity that already exists, destroy it forcibly
    if (!isServer && scene->EntityById(entityID))
    {
        LogWarning("Received entity creation from server for entity ID " + String(entityID) + " that already exists. Removing the old entity.");
        scene->RemoveEntity(entityID, AttributeChange::LocalOnly);
    }
    else if (isServer)
    {
        // Server never uses the client's entityID.
        entityID = scene->NextFreeId();
        // Store the unacked-to-real mapping in the UserConnection, in case it refers to the pending ID in later messages
        source->unackedIdsToRealIds[senderEntityID | UniqueIdGenerator::FIRST_UNACKED_ID] = entityID;
    }
    
    EntityPtr entity = scene->CreateEntity(entityID);
    if (!entity)
    {
        LogWarning("Could not create entity " + String(entityID) + ", disregarding CreateEntity message");
        return;
    }

    /** As the client created the entity and already has it in its local state,
        we must add it to the servers sync state for the client without emitting any StateChangeRequest signals.
        @note The below state->MarkComponentProcessed() already accomplishes part of this, but still do explicitly here!
        @note The below entity->CreateComponentWithId() will trigger the signaling logic but it will stop in 
        SceneSyncState::FillRequest() as the Entity is not yet in the scene! */
    if (isServer)
    {
        state->RemovePendingEntity(senderEntityID);
        state->RemovePendingEntity(entityID);
        state->MarkEntityProcessed(entityID);
    }
    
    std::vector<std::pair<component_id_t, component_id_t> > componentIdRewrites;

    entity_id_t parentEntityID = 0;

    try
    {    
        // Read the temporary flag
        bool temporary = ds.Read<u8>() != 0;
        entity->SetTemporary(temporary);

        // In hierarchic scene protocol, read the parent entity ID
        if (source->ProtocolVersion() >= ProtocolHierarchicScene)
        {
            parentEntityID = ds.Read<u32>();
            
            // Convert unacked ID if we can
            if (isServer && parentEntityID >= UniqueIdGenerator::FIRST_UNACKED_ID && parentEntityID < UniqueIdGenerator::FIRST_LOCAL_ID)
            {
                if (source->unackedIdsToRealIds.find(parentEntityID) != source->unackedIdsToRealIds.end())
                    parentEntityID = source->unackedIdsToRealIds[parentEntityID];
                else
                    LogError(String("[SyncManager]: HandleCreateEntityParent: Client sent unknown unacked parent Entity #" + String(parentEntityID) + " in CreateEntity message"));
            }
            if (parentEntityID)
            {
                EntityPtr parentEntity = scene->EntityById(parentEntityID);
                if (parentEntity)
                    entity->SetParent(parentEntity, change);
                else
                    LogError(String("[SyncManager]: HandleCreateEntityParent: Parent Entity #" + String(parentEntityID) + " not found from Scene when handling CreateEntity message"));
            }
        }

        // Read the components
        unsigned numComponents = ds.ReadVLE<kNet::VLE8_16_32>();
        for(uint i = 0; i < numComponents; ++i)
        {
            component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
            component_id_t senderCompID = compID;
            // If we are server, rewrite the ID
            if (isServer) compID = 0;
            
            u32 typeID = ds.ReadVLE<kNet::VLE8_16_32>();
            String name = String(ds.ReadString().c_str());
            unsigned attrDataSize = ds.ReadVLE<kNet::VLE8_16_32>();
            if (attrDataSize > NUMELEMS(attrDataBuffer_))
            {
                /// @todo Inspect if 'state' should be updated or a more fatal error would be appropriate here.
                LogError(String("SyncManager::HandleCreateEntity: Attribute data size " + String(attrDataSize) + " bytes is bigger than the destination buffer of " + 
                    String(NUMELEMS(attrDataBuffer_)) + " bytes. In " + framework_->Scene()->ComponentTypeNameForTypeId(typeID) + 
                    " in Entity " + String(entity->Id()) + ". Entity will be ignored!"));

                state->RemoveFromQueue(entity->Id());
                state->entities.erase(entity->Id());
                scene->RemoveEntity(entity->Id(), AttributeChange::LocalOnly);
                return;
            }
            ds.ReadArray<u8>((u8*)&attrDataBuffer_[0], attrDataSize);
            kNet::DataDeserializer attrDs(attrDataBuffer_, attrDataSize);
            
            // If client gets a component that already exists, destroy it forcibly
            if (!isServer && entity->ComponentById(compID))
            {
                LogWarning("Received component creation from server for component ID " + String(compID) + " that already exists in " + entity->ToString() + ". Removing the old component.");
                entity->RemoveComponentById(compID, AttributeChange::LocalOnly);
            }
            
            ComponentPtr comp = entity->CreateComponentWithId(compID, typeID, name, change);
            if (!comp)
            {
                LogWarning("Failed to create component type " + String(compID) + " to " + entity->ToString() + " while handling CreateEntity message, skipping component");
                continue;
            }
            // On server, get the assigned ID now
            if (isServer)
            {
                compID = comp->Id();
                componentIdRewrites.push_back(std::make_pair(senderCompID, compID));
            }
            // Create the component to the sender's syncstate, then mark it processed (undirty)
            state->MarkComponentProcessed(entityID, compID);
            
            // Fill static attributes
            unsigned numStaticAttrs = comp->NumStaticAttributes();
            const AttributeVector& attrs = comp->Attributes();
            for (uint i = 0; i < numStaticAttrs; ++i)
            {
                // Allow component version mismatches (adding more attributes to the end of static attributes list), break if no more data present.
                // All attributes (including bool) are at least 8 bits.
                if (attrDs.BitsLeft() >= 8)
                    attrs[i]->FromBinary(attrDs, AttributeChange::Disconnected);
                else
                {
                    if (mismatchingComponentTypes.find(comp->TypeId()) == mismatchingComponentTypes.end())
                    {
                        mismatchingComponentTypes.insert(comp->TypeId());
                        LogWarning("Not enough static attribute data in component " + comp->TypeName() + " (version mismatch).");
                    }
                    break;
                }
            }

            if (comp->SupportsDynamicAttributes())
            {
                // Create any dynamic attributes
                while (attrDs.BitsLeft() > 2 * 8)
                {
                    u8 index = attrDs.Read<u8>();
                    u8 typeId = attrDs.Read<u8>();
                    String name = String(attrDs.ReadString().c_str());
                    IAttribute* newAttr = comp->CreateAttribute(index, typeId, name, change);
                    if (!newAttr)
                    {
                        LogWarning("Failed to create dynamic attribute. Skipping rest of the attributes for this component.");
                        break;
                    }
                    newAttr->FromBinary(attrDs, AttributeChange::Disconnected);
                }
            }
            else if (attrDs.BitsLeft())
            {
                if (mismatchingComponentTypes.find(comp->TypeId()) == mismatchingComponentTypes.end())
                {
                    mismatchingComponentTypes.insert(comp->TypeId());
                    LogWarning("Extra static attribute data in component " + comp->TypeName() + " (version mismatch).");
                }
            }
        }
    }
    catch(kNet::NetException &/*e*/)
    {
        LogError("Failed to deserialize the creation of a new entity from the peer. Deleting the partially crafted entity!");
        scene->RemoveEntity(entity->Id(), AttributeChange::Disconnected);
        // Rethrowing knet exception up should be fine here, they get handled further up again
        throw; // Propagate the exception up, to handle a peer which is sending us bad protocol bits.
    }
    
    // Emit the component changes last, to signal only a coherent state of the whole entity
    scene->EmitEntityCreated(entity.Get(), change);
    const Entity::ComponentMap &components = entity->Components();
    for (auto i = components.Begin(); i != components.End(); ++i)
        i->second_->ComponentChanged(change);
    
    // Send CreateEntityReply (server only)
    if (isServer)
    {
        kNet::DataSerializer replyDs(createEntityBuffer_, NUMELEMS(createEntityBuffer_));
        replyDs.AddVLE<kNet::VLE8_16_32>(sceneID);
        replyDs.AddVLE<kNet::VLE8_16_32>(senderEntityID & UniqueIdGenerator::LAST_REPLICATED_ID);
        replyDs.AddVLE<kNet::VLE8_16_32>(entityID & UniqueIdGenerator::LAST_REPLICATED_ID);
        replyDs.AddVLE<kNet::VLE8_16_32>((u32)componentIdRewrites.size());
        for (unsigned i = 0; i < componentIdRewrites.size(); ++i)
        {
            replyDs.AddVLE<kNet::VLE8_16_32>(componentIdRewrites[i].first & UniqueIdGenerator::LAST_REPLICATED_ID);
            replyDs.AddVLE<kNet::VLE8_16_32>(componentIdRewrites[i].second & UniqueIdGenerator::LAST_REPLICATED_ID);
        }
        source->Send(cCreateEntityReplyMessage, true, true, replyDs);
    }

    // Mark the entity processed (undirty) in the sender's syncstate so that create is not echoed back
    state->MarkEntityProcessed(entityID);
}

void SyncManager::HandleCreateComponents(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding CreateComponents message");
        return;
    }
    
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    bool isServer = owner_->IsServer();
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    std::vector<std::pair<component_id_t, component_id_t> > componentIdRewrites;
    std::vector<ComponentPtr> addedComponents;

    EntityPtr entity;
    u32 sceneID;
    entity_id_t entityID;

    try
    {
        kNet::DataDeserializer ds(data, numBytes);
        sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
        entityID = ds.ReadVLE<kNet::VLE8_16_32>();
        
        if (!ValidateAction(source, cCreateComponentsMessage, entityID))
            return;

        EntitySyncState &entityState = state->entities[entityID];
        entity = entityState.weak.Lock();
        if (!entity)
        {
            LogWarning("Entity " + String(entityID) + " not found for CreateComponents message");
            return;
        }

        if (!scene->AllowModifyEntity(source, entity.Get()))
            return;
        
        // Read the components
        while (ds.BitsLeft() > 2 * 8)
        {
            component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
            component_id_t senderCompID = compID;
            // If we are server, rewrite the ID
            if (isServer) compID = 0;
            
            u32 typeID = ds.ReadVLE<kNet::VLE8_16_32>();
            String name = String(ds.ReadString().c_str());
            unsigned attrDataSize = ds.ReadVLE<kNet::VLE8_16_32>();
            if (attrDataSize > NUMELEMS(attrDataBuffer_))
            {
                /// @todo Inspect if 'state' should be updated or a more fatal error would be appropriate here.
                state->MarkEntityProcessed(entityID);
                LogError("SyncManager::HandleCreateComponents: Attribute data size " + String(attrDataSize) +
                    " bytes is bigger than the destination buffer of " + String(NUMELEMS(attrDataBuffer_)) +
                    " bytes. In " + framework_->Scene()->ComponentTypeNameForTypeId(typeID) + " in Entity " + String(entity->Id()) + ". Component(s) will be ignored!");
                return;
            }
            ds.ReadArray<u8>((u8*)&attrDataBuffer_[0], attrDataSize);
            kNet::DataDeserializer attrDs(attrDataBuffer_, attrDataSize);
            
            // If client gets a component that already exists, destroy it forcibly
            if (!isServer && entity->ComponentById(compID))
            {
                LogWarning("Received component creation from server for component ID " + String(compID) + " that already exists in " + entity->ToString() + ". Removing the old component.");
                entity->RemoveComponentById(compID, AttributeChange::LocalOnly);
            }
            
            ComponentPtr comp = entity->CreateComponentWithId(compID, typeID, name, change);
            if (!comp)
            {
                LogWarning("Failed to create component type " + String(compID) + " to " + entity->ToString() + " while handling CreateComponents message, skipping component");
                continue;
            }
            // On server, get the assigned ID now
            if (isServer)
            {
                compID = comp->Id();
                componentIdRewrites.push_back(std::make_pair(senderCompID, compID));
            }
            
            // Create the component to the sender's syncstate, then mark it processed (undirty)
            state->MarkComponentProcessed(entityID, compID);
            
            addedComponents.push_back(comp);
            
            // Fill static attributes
            unsigned numStaticAttrs = comp->NumStaticAttributes();
            const AttributeVector& attrs = comp->Attributes();
            for (uint i = 0; i < numStaticAttrs; ++i)
            {
                // Allow component version mismatches (adding more attributes to the end of static attributes list), break if no more data present.
                // All attributes (including bool) are at least 8 bits.
                if (attrDs.BitsLeft() >= 8)
                    attrs[i]->FromBinary(attrDs, AttributeChange::Disconnected);
                else
                {
                    if (mismatchingComponentTypes.find(comp->TypeId()) == mismatchingComponentTypes.end())
                    {
                        mismatchingComponentTypes.insert(comp->TypeId());
                        LogWarning("Not enough static attribute data in component " + comp->TypeName() + " (version mismatch).");
                    }
                    break;
                }
            }
            
            if (comp->SupportsDynamicAttributes())
            {
                // Create any dynamic attributes
                while (attrDs.BitsLeft() > 2 * 8)
                {
                    u8 index = attrDs.Read<u8>();
                    u8 typeId = attrDs.Read<u8>();
                    String name = String(attrDs.ReadString().c_str());
                    IAttribute* newAttr = comp->CreateAttribute(index, typeId, name, change);
                    if (!newAttr)
                    {
                        LogWarning("Failed to create dynamic attribute. Skipping rest of the attributes for this component.");
                        break;
                    }
                    newAttr->FromBinary(attrDs, AttributeChange::Disconnected);
                }
            }
            else if (attrDs.BitsLeft())
            {
                if (mismatchingComponentTypes.find(comp->TypeId()) == mismatchingComponentTypes.end())
                {
                    mismatchingComponentTypes.insert(comp->TypeId());
                    LogWarning("Extra static attribute data in component " + comp->TypeName() + " (version mismatch).");
                }
            }
        }
    } catch(kNet::NetException &/*e*/)
    {
        LogError("Failed to deserialize the creation of new component(s) from the peer. Deleting the partially crafted components!");
        for(size_t i = 0; i < addedComponents.size(); ++i)
            entity->RemoveComponent(addedComponents[i], AttributeChange::Disconnected);
        // Rethrowing knet exception up should be fine here, they get handled further up again
        throw; // Propagate the exception up, to handle a peer which is sending us bad protocol bits.
    }
    
    // Send CreateComponentsReply (server only)
    if (isServer)
    {
        kNet::DataSerializer replyDs(createEntityBuffer_, NUMELEMS(createEntityBuffer_));
        replyDs.AddVLE<kNet::VLE8_16_32>(sceneID);
        replyDs.AddVLE<kNet::VLE8_16_32>(entityID & UniqueIdGenerator::LAST_REPLICATED_ID);
        replyDs.AddVLE<kNet::VLE8_16_32>((u32)componentIdRewrites.size());
        for (unsigned i = 0; i < componentIdRewrites.size(); ++i)
        {
            replyDs.AddVLE<kNet::VLE8_16_32>(componentIdRewrites[i].first & UniqueIdGenerator::LAST_REPLICATED_ID);
            replyDs.AddVLE<kNet::VLE8_16_32>(componentIdRewrites[i].second & UniqueIdGenerator::LAST_REPLICATED_ID);
        }
        source->Send(cCreateComponentsReplyMessage, true, true, replyDs);
    }
    
    // Emit the component changes last, to signal only a coherent state of the whole entity
    for (unsigned i = 0; i < addedComponents.size(); ++i)
        addedComponents[i]->ComponentChanged(change);
}

void SyncManager::HandleRemoveEntity(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding RemoveEntity message");
        return;
    }
    
    bool isServer = owner_->IsServer();
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    
    if (!ValidateAction(source, cRemoveEntityMessage, entityID))
        return;

    EntityPtr entity = scene->EntityById(entityID);
    if (entity && !scene->AllowModifyEntity(source, entity.Get()))
        return;
    // @todo Is this second check ensuring AllowModifyEntity didn't remove the Entity from Scene?
    if (!scene->EntityById(entityID))
    {
        LogWarning("Missing entity " + String(entityID) + " for RemoveEntity message");
        return;
    }
    
    scene->RemoveEntity(entityID, change);

    // Delete from the sender's syncstate so that we don't echo the delete back needlessly
    state->RemoveFromQueue(entityID); // Be sure to erase from dirty queue so that we don't invoke UDB
    state->entities.erase(entityID);
}

void SyncManager::HandleRemoveComponents(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding RemoveComponents message");
        return;
    }
    
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    bool isServer = owner_->IsServer();
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    
    if (!ValidateAction(source, cRemoveComponentsMessage, entityID))
        return;

    EntitySyncState &entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (entity && !scene->AllowModifyEntity(source, entity.Get()))
        return;
    if (!entity)
    {
        LogWarning("Entity " + String(entityID) + " not found for RemoveComponents message");
        return;
    }
    
    while (ds.BitsLeft() >= 8)
    {
        component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
        ComponentPtr comp = entity->ComponentById(compID);
        if (!comp)
        {
            LogWarning("Component id " + String(compID) + " not found in " + entity->ToString() + " for RemoveComponents message, disregarding");
            continue;
        }
        entity->RemoveComponent(comp, change);

        entityState.RemoveFromQueue(compID); // Be sure to erase from dirty queue so that we don't invoke UDB
        entityState.components.erase(compID);
    }
}

void SyncManager::HandleCreateAttributes(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding CreateAttributes message");
        return;
    }
    
    bool isServer = owner_->IsServer();
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    
    if (!ValidateAction(source, cCreateAttributesMessage, entityID))
        return;
    
    EntitySyncState &entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (!entity)
    {
        LogWarning("Entity " + String(entityID) + " not found for CreateAttributes message");
        return;
    }

    if (!scene->AllowModifyEntity(source, 0)) //to check if creating entities is allowed (for this user)
        return;

    std::vector<IAttribute*> addedAttrs;
    while (ds.BitsLeft() >= 3 * 8)
    {
        component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
        ComponentPtr comp = entity->ComponentById(compID);
        if (!comp)
        {
            LogWarning("Component id " + String(compID) + " not found in " + entity->ToString() + " for CreateAttributes message, aborting message parsing");
            return;
        }
        
        u8 attrIndex = ds.Read<u8>();
        u8 typeId = ds.Read<u8>();
        String name = String(ds.ReadString().c_str());
        
        if (isServer)
        {
            // If we are server, do not allow to overwrite existing attributes by client requests
            const AttributeVector& existingAttrs = comp->Attributes();
            if (attrIndex < existingAttrs.Size() && existingAttrs[attrIndex])
            {
                LogWarning("Client attempted to overwrite an existing attribute index " + String((int)attrIndex) + " in component " + comp->TypeName() + " in " + entity->ToString() + ", aborting CreateAttributes message parsing");
                return;
            }
        }
        
        IAttribute* attr = comp->CreateAttribute(attrIndex, typeId, name, change);
        if (!attr)
        {
            LogWarning("Could not create attribute into component " + comp->TypeName() + " in " + entity->ToString() + ", aborting CreateAttributes message parsing");
            return;
        }
        
        addedAttrs.push_back(attr);
        try
        {
            attr->FromBinary(ds, AttributeChange::Disconnected);
        } catch (kNet::NetException &/*e*/)
        {
            LogError("Failed to deserialize the creation of a new attribute from the peer!");
            comp->RemoveAttribute(attrIndex, AttributeChange::Disconnected);
            throw;
        }
        
        // Remove the corresponding add command from the sender's syncstate, so that the attribute add is not echoed back
        entityState.components[compID].newAndRemovedAttributes.Erase(attrIndex);
    }
    
    // Signal attribute changes after creating and reading all
    for (unsigned i = 0; i < addedAttrs.size(); ++i)
    {
        IComponent* owner = addedAttrs[i]->Owner();
        u8 attrIndex = addedAttrs[i]->Index();
        owner->EmitAttributeChanged(addedAttrs[i], change);

        // Remove the dirty bit from sender's syncstate so that we do not echo the change back
        entityState.components[owner->Id()].dirtyAttributes[attrIndex >> 3] &= ~(1 << (attrIndex & 7));
    }
}

void SyncManager::HandleRemoveAttributes(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding RemoveAttributes message");
        return;
    }
    
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    bool isServer = owner_->IsServer();
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    
    if (!ValidateAction(source, cRemoveAttributesMessage, entityID))
        return;
    
    EntitySyncState &entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (entity && !scene->AllowModifyEntity(source, entity.Get()))
        return;
    if (!entity)
    {
        LogWarning("Entity " + String(entityID) + " not found for RemoveAttributes message");
        return;
    }
    
    while (ds.BitsLeft() >= 8)
    {
        component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
        u8 attrIndex = ds.Read<u8>();
        
        ComponentPtr comp = entity->ComponentById(compID);
        if (!comp)
        {
            LogWarning("Component id " + String(compID) + " not found in " + entity->ToString() + " for RemoveAttributes message");
            continue;
        }
        
        comp->RemoveAttribute(attrIndex, change);

        // Remove the corresponding remove command from the sender's syncstate, so that the attribute remove is not echoed back
        entityState.components[compID].newAndRemovedAttributes.Erase(attrIndex);
    }
}

void SyncManager::HandleEditAttributes(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    // Get matching syncstate for reflecting the changes
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding EditAttributes message");
        return;
    }
    
    // For clients, the change type is LocalOnly. For server, the change type is Replicate, so that it will get replicated to all clients in turn
    bool isServer = owner_->IsServer();
    AttributeChange::Type change = isServer ? AttributeChange::Replicate : AttributeChange::LocalOnly;
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    
    if (!ValidateAction(source, cRemoveAttributesMessage, entityID))
        return;

    EntitySyncState &entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (entity && !scene->AllowModifyEntity(source, entity.Get())) // check if allowed to modify this entity.
        return;
    if (!entity)
    {
        LogWarning("Entity " + String(entityID) + " not found for EditAttributes message");
        return;
    }
    
    // Record the update time for calculating the update interval
    // Default update interval if state not found or interval not measured yet
    float updateInterval = updatePeriod_;
    entityState.UpdateReceived();
    if (entityState.avgUpdateInterval > 0.0f)
        updateInterval = entityState.avgUpdateInterval;

    // Add a fudge factor in case there is jitter in packet receipt or the server is too taxed
    updateInterval *= 1.25f;

    std::vector<IAttribute*> changedAttrs;
    while (ds.BitsLeft() >= 8)
    {
        component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
        unsigned attrDataSize = ds.ReadVLE<kNet::VLE8_16_32>();
        if (attrDataSize > NUMELEMS(attrDataBuffer_))
        {
            /// @todo Inspect if 'state' should be updated or a more fatal error would be appropriate here.
            state->MarkEntityProcessed(entityID);
            
            LogError("SyncManager::HandleEditAttributes: Attribute data size " + String(attrDataSize) +
                " bytes is bigger than the destination buffer of " + String(NUMELEMS(attrDataBuffer_)) +
                " bytes. Component id " + String(compID) + " in Entity " + String(entity->Id()) + ". Attribute(s) will be ignored!");
            return;
        }
        ds.ReadArray<u8>((u8*)&attrDataBuffer_[0], attrDataSize);
        kNet::DataDeserializer attrDs(attrDataBuffer_, attrDataSize);

        ComponentPtr comp = entity->ComponentById(compID);
        if (!comp)
        {
            LogWarning("Component id " + String(compID) + " not found in " + entity->ToString() + " for EditAttributes message, skipping to next component");
            continue;
        }
        const AttributeVector& attributes = comp->Attributes();

        int indexingMethod = attrDs.Read<kNet::bit>();
        if (!indexingMethod)
        {
            // Method 1: indices
            u8 numChangedAttrs = attrDs.Read<u8>();
            for (unsigned i = 0; i < numChangedAttrs; ++i)
            {
                u8 attrIndex = attrDs.Read<u8>();
                if (attrIndex >= attributes.Size())
                {
                    LogWarning("Out of bounds attribute index in EditAttributes message, skipping to next component");
                    break;
                }
                IAttribute* attr = attributes[attrIndex];
                if (!attr)
                {
                    LogWarning("Nonexistent attribute in EditAttributes message, skipping to next component");
                    break;
                }
                
                bool interpolate = (!isServer && attr->Metadata() && attr->Metadata()->interpolation == AttributeMetadata::Interpolate);
                if (!interpolate)
                {
                    attr->FromBinary(attrDs, AttributeChange::Disconnected);
                    changedAttrs.push_back(attr);
                }
                else
                {
                    IAttribute* endValue = attr->Clone();
                    endValue->FromBinary(attrDs, AttributeChange::Disconnected);
                    scene->StartAttributeInterpolation(attr, endValue, updateInterval);
                }
            }
        }
        else
        {
            // Method 2: bitmask
            for (unsigned i = 0; i < attributes.Size(); ++i)
            {
                // Break if component version inconsistency and no more data
                if (attrDs.BitsLeft() == 0)
                    break;
                
                int changed = attrDs.Read<kNet::bit>();
                if (changed)
                {
                    IAttribute* attr = attributes[i];
                    if (!attr)
                    {
                        LogWarning("Nonexistent attribute in EditAttributes message, skipping to next component");
                        break;
                    }
                    bool interpolate = (!isServer && attr->Metadata() && attr->Metadata()->interpolation == AttributeMetadata::Interpolate);
                    if (!interpolate)
                    {
                        attr->FromBinary(attrDs, AttributeChange::Disconnected);
                        changedAttrs.push_back(attr);
                    }
                    else
                    {
                        IAttribute* endValue = attr->Clone();
                        endValue->FromBinary(attrDs, AttributeChange::Disconnected);
                        scene->StartAttributeInterpolation(attr, endValue, updateInterval);
                    }
                }
            }
        }
    }
    
    // Signal attribute changes after reading all
    for (unsigned i = 0; i < changedAttrs.size(); ++i)
    {
        IComponent* owner = changedAttrs[i]->Owner();
        u8 attrIndex = changedAttrs[i]->Index();
        owner->EmitAttributeChanged(changedAttrs[i], change);

        // Remove the dirty bit from sender's syncstate so that we do not echo the change back
        entityState.components[owner->Id()].dirtyAttributes[attrIndex >> 3] &= ~(1 << (attrIndex & 7));
    }
}

void SyncManager::HandleCreateEntityReply(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding CreateEntityReply message");
        return;
    }
    
    bool isServer = owner_->IsServer();
    if (isServer)
    {
        LogWarning("Discarding CreateEntityReply message on server");
        return;
    }
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)

    entity_id_t senderEntityID = ds.ReadVLE<kNet::VLE8_16_32>() | UniqueIdGenerator::FIRST_UNACKED_ID;
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    scene->ChangeEntityId(senderEntityID, entityID);

    state->RemoveFromQueue(senderEntityID);                         // Make sure we don't have stale pointers in the dirty queue
    state->entities[entityID] = state->entities[senderEntityID];    // Copy the sync state to the new ID
    state->entities[entityID].id = entityID;                        // Must remember to change ID manually
    state->entities[entityID].weak = scene->EntityById(entityID);   // Refresh the weak ptr
    state->entities.erase(senderEntityID);                          // Remove old id from the state
    
    //std::cout << "CreateEntityReply, entity " << senderEntityID << " -> " << entityID << std::endl;

    EntitySyncState& entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (!entity)
    {
        LogError("Failed to get entity after ID change");
        return;
    }
    
    unsigned numComps = ds.ReadVLE<kNet::VLE8_16_32>();
    for (unsigned i = 0; i < numComps; ++i)
    {
        component_id_t senderCompID = ds.ReadVLE<kNet::VLE8_16_32>() | UniqueIdGenerator::FIRST_UNACKED_ID;
        component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
        
        //std::cout << "CreateEntityReply, component " << senderCompID << " -> " << compID << std::endl;
        
        entity->ChangeComponentId(senderCompID, compID);
        entityState.components[compID] = entityState.components[senderCompID]; // Copy the sync state to the new ID
        entityState.components[compID].id = compID; // Must remember to change ID manually
        entityState.components.erase(senderCompID);
        
        // Send notification
        IComponent* comp = entity->ComponentById(compID).Get();
        scene->EmitComponentAcked(comp, senderCompID);
    }

    // Send notification
    scene->EmitEntityAcked(entity.Get(), senderEntityID);

    // Now mark every component dirty so they will be inspected for changes on the next update
    for (std::map<component_id_t, ComponentSyncState>::iterator i = entityState.components.begin(); i != entityState.components.end(); ++i)
        state->MarkComponentDirty(entityID, i->first);
}

void SyncManager::HandleCreateComponentsReply(UserConnection* source, const char* data, size_t numBytes)
{
    assert(source);
    SceneSyncState* state = source->syncState.Get();
    ScenePtr scene = GetRegisteredScene();
    if (!scene || !state)
    {
        LogWarning("Null scene or sync state, disregarding CreateComponentsReply message");
        return;
    }
    
    bool isServer = owner_->IsServer();
    if (isServer)
    {
        LogWarning("Discarding CreateComponentsReply message on server");
        return;
    }
    
    kNet::DataDeserializer ds(data, numBytes);
    unsigned sceneID = ds.ReadVLE<kNet::VLE8_16_32>(); ///\todo Dummy ID. Lookup scene once multiscene is properly supported
    UNREFERENCED_PARAM(sceneID)
    entity_id_t entityID = ds.ReadVLE<kNet::VLE8_16_32>();
    state->RemoveFromQueue(entityID); // Make sure we don't have stale pointers in the dirty queue
    
    EntitySyncState& entityState = state->entities[entityID];
    EntityPtr entity = entityState.weak.Lock();
    if (!entity)
    {
        LogError("Failed to get entity after ID change");
        return;
    }
    
    unsigned numComps = ds.ReadVLE<kNet::VLE8_16_32>();
    for (unsigned i = 0; i < numComps; ++i)
    {
        component_id_t senderCompID = ds.ReadVLE<kNet::VLE8_16_32>() | UniqueIdGenerator::FIRST_UNACKED_ID;
        component_id_t compID = ds.ReadVLE<kNet::VLE8_16_32>();
        
        //std::cout << "CreateComponentReply, component " << senderCompID << " -> " << compID << std::endl;
        
        entity->ChangeComponentId(senderCompID, compID);
        entityState.components[compID] = entityState.components[senderCompID]; // Copy the sync state to the new ID
        entityState.components[compID].id = compID; // Must remember to change ID manually
        entityState.components.erase(senderCompID);
        
        // Send notification
        IComponent* comp = entity->ComponentById(compID).Get();
        scene->EmitComponentAcked(comp, senderCompID);
    }
    
    for (auto i = entityState.components.begin(); i != entityState.components.end(); ++i)
    {
        // Now mark every component dirty so they will be inspected for changes on the next update
        state->MarkComponentDirty(entityID, i->first);
    }
}

void SyncManager::HandleEntityAction(UserConnection* source, MsgEntityAction& msg)
{
    bool isServer = owner_->IsServer();
    
    ScenePtr scene = GetRegisteredScene();
    if (!scene)
    {
        LogWarning("SyncManager: Ignoring received MsgEntityAction \"" + String(msg.name.size() == 0 ? "(null)" : std::string((const char *)&msg.name[0], msg.name.size()).c_str()) + "\" (" + String(msg.parameters.size()) + " parameters) for entity ID " + String(msg.entityId) + " as no scene exists!");
        return;
    }
    
    entity_id_t entityId = msg.entityId;
    EntityPtr entity = scene->EntityById(entityId);
    if (!entity)
    {
        LogWarning("Entity with ID " + String(entityId) + " not found for EntityAction message \"" + String(msg.name.size() == 0 ? "(null)" : std::string((const char *)&msg.name[0], msg.name.size()).c_str()) + "\" (" + String(msg.parameters.size()) + " parameters).");
        return;
    }

    // If we are server, get the user who sent the action, so it can be queried
    if (isServer)
    {
        Server* server = owner_->Server().Get();
        if (server)
        {
            server->SetActionSender(source);
        }
    }
    
    String action = String(BufferToString(msg.name));
    StringVector params;
    for(uint i = 0; i < msg.parameters.size(); ++i)
        params.Push(String(BufferToString(msg.parameters[i].parameter)));

    EntityAction::ExecTypeField type = (EntityAction::ExecTypeField)(msg.executionType);

    bool handled = false;

    if ((type & EntityAction::Local) != 0 || (isServer && (type & EntityAction::Server) != 0))
    {
        entity->Exec(EntityAction::Local, action, params); // Execute the action locally, so that it doesn't immediately propagate back to network for sending.
        handled = true;
    }

    // If execution type is Peers, replicate to all peers but the sender.
    if (isServer && (type & EntityAction::Peers) != 0)
    {
        msg.executionType = (u8)EntityAction::Local;
        foreach(UserConnectionPtr userConn, owner_->Server()->UserConnections())
            if (userConn.Get() != source) // The EC action will not be sent to the machine that originated the request to send an action to all peers.
                userConn->syncState->queuedActions.push_back(msg);
        handled = true;
    }
    
    if (!handled)
        LogWarning("SyncManager: Received MsgEntityAction message \"" + action + "\", but it went unhandled because of its type=" + String(type));

    // Clear the action sender after action handling
    Server *server = owner_->Server().Get();
    if (server)
        server->SetActionSender(UserConnectionPtr());
}

}
