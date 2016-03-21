// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "CoreTypes.h"
#include "JavaScriptInstance.h"
#include "LoggingFunctions.h"
#include "EnvironmentLight.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800)
#endif

#include "Entity.h"


using namespace Tundra;
using namespace std;

namespace JSBindings
{



static const char* EnvironmentLight_ID = "EnvironmentLight";

const char* SignalWrapper_EnvironmentLight_ComponentNameChanged_ID = "SignalWrapper_EnvironmentLight_ComponentNameChanged";

class SignalWrapper_EnvironmentLight_ComponentNameChanged
{
public:
    SignalWrapper_EnvironmentLight_ComponentNameChanged(Object* owner, Signal2< const String &, const String & >* signal) :
        owner_(owner),
        signal_(signal)
    {
    }

    WeakPtr<Object> owner_;
    Signal2< const String &, const String & >* signal_;
};

class SignalReceiver_EnvironmentLight_ComponentNameChanged : public SignalReceiver
{
public:
    void OnSignal(const String & param0, const String & param1)
    {
        duk_context* ctx = ctx_;
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, "_OnSignal");
        duk_remove(ctx, -2);
        duk_push_number(ctx, (size_t)key_);
        duk_push_array(ctx);
        duk_push_string(ctx, param0.CString());
        duk_put_prop_index(ctx, -2, 0);
        duk_push_string(ctx, param1.CString());
        duk_put_prop_index(ctx, -2, 1);
        bool success = duk_pcall(ctx, 2) == 0;
        if (!success) LogError("[JavaScript] OnSignal: " + String(duk_safe_to_string(ctx, -1)));
        duk_pop(ctx);
    }
};

static duk_ret_t SignalWrapper_EnvironmentLight_ComponentNameChanged_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<SignalWrapper_EnvironmentLight_ComponentNameChanged>(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_ID);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ComponentNameChanged_Connect(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ComponentNameChanged* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ComponentNameChanged>(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_ID);
    if (!wrapper->owner_) return 0;
    HashMap<void*, SharedPtr<SignalReceiver> >& signalReceivers = JavaScriptInstance::InstanceFromContext(ctx)->SignalReceivers();
    if (signalReceivers.Find(wrapper->signal_) == signalReceivers.End())
    {
        SignalReceiver_EnvironmentLight_ComponentNameChanged* receiver = new SignalReceiver_EnvironmentLight_ComponentNameChanged();
        receiver->ctx_ = ctx;
        receiver->key_ = wrapper->signal_;
        wrapper->signal_->Connect(receiver, &SignalReceiver_EnvironmentLight_ComponentNameChanged::OnSignal);
        signalReceivers[wrapper->signal_] = receiver;
    }
    CallConnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ComponentNameChanged_Disconnect(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ComponentNameChanged* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ComponentNameChanged>(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_ID);
    if (!wrapper->owner_) return 0;
    CallDisconnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ComponentNameChanged_Emit(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ComponentNameChanged* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ComponentNameChanged>(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_ID);
    if (!wrapper->owner_) return 0;
    String param0 = duk_require_string(ctx, 0);
    String param1 = duk_require_string(ctx, 1);
    wrapper->signal_->Emit(param0, param1);
    return 0;
}

static duk_ret_t EnvironmentLight_Get_ComponentNameChanged(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    SignalWrapper_EnvironmentLight_ComponentNameChanged* wrapper = new SignalWrapper_EnvironmentLight_ComponentNameChanged(thisObj, &thisObj->ComponentNameChanged);
    PushValueObject(ctx, wrapper, SignalWrapper_EnvironmentLight_ComponentNameChanged_ID, SignalWrapper_EnvironmentLight_ComponentNameChanged_Finalizer, false);
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Connect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "connect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Disconnect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "disconnect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ComponentNameChanged_Emit, 2);
    duk_put_prop_string(ctx, -2, "Emit");
    return 1;
}

const char* SignalWrapper_EnvironmentLight_ParentEntitySet_ID = "SignalWrapper_EnvironmentLight_ParentEntitySet";

class SignalWrapper_EnvironmentLight_ParentEntitySet
{
public:
    SignalWrapper_EnvironmentLight_ParentEntitySet(Object* owner, Signal0< void >* signal) :
        owner_(owner),
        signal_(signal)
    {
    }

    WeakPtr<Object> owner_;
    Signal0< void >* signal_;
};

class SignalReceiver_EnvironmentLight_ParentEntitySet : public SignalReceiver
{
public:
    void OnSignal()
    {
        duk_context* ctx = ctx_;
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, "_OnSignal");
        duk_remove(ctx, -2);
        duk_push_number(ctx, (size_t)key_);
        duk_push_array(ctx);
        bool success = duk_pcall(ctx, 2) == 0;
        if (!success) LogError("[JavaScript] OnSignal: " + String(duk_safe_to_string(ctx, -1)));
        duk_pop(ctx);
    }
};

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntitySet_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<SignalWrapper_EnvironmentLight_ParentEntitySet>(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_ID);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntitySet_Connect(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ParentEntitySet* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ParentEntitySet>(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_ID);
    if (!wrapper->owner_) return 0;
    HashMap<void*, SharedPtr<SignalReceiver> >& signalReceivers = JavaScriptInstance::InstanceFromContext(ctx)->SignalReceivers();
    if (signalReceivers.Find(wrapper->signal_) == signalReceivers.End())
    {
        SignalReceiver_EnvironmentLight_ParentEntitySet* receiver = new SignalReceiver_EnvironmentLight_ParentEntitySet();
        receiver->ctx_ = ctx;
        receiver->key_ = wrapper->signal_;
        wrapper->signal_->Connect(receiver, &SignalReceiver_EnvironmentLight_ParentEntitySet::OnSignal);
        signalReceivers[wrapper->signal_] = receiver;
    }
    CallConnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntitySet_Disconnect(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ParentEntitySet* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ParentEntitySet>(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_ID);
    if (!wrapper->owner_) return 0;
    CallDisconnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntitySet_Emit(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ParentEntitySet* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ParentEntitySet>(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_ID);
    if (!wrapper->owner_) return 0;
    wrapper->signal_->Emit();
    return 0;
}

static duk_ret_t EnvironmentLight_Get_ParentEntitySet(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    SignalWrapper_EnvironmentLight_ParentEntitySet* wrapper = new SignalWrapper_EnvironmentLight_ParentEntitySet(thisObj, &thisObj->ParentEntitySet);
    PushValueObject(ctx, wrapper, SignalWrapper_EnvironmentLight_ParentEntitySet_ID, SignalWrapper_EnvironmentLight_ParentEntitySet_Finalizer, false);
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Connect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "connect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Disconnect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "disconnect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntitySet_Emit, 0);
    duk_put_prop_string(ctx, -2, "Emit");
    return 1;
}

const char* SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_ID = "SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached";

class SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached
{
public:
    SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached(Object* owner, Signal0< void >* signal) :
        owner_(owner),
        signal_(signal)
    {
    }

    WeakPtr<Object> owner_;
    Signal0< void >* signal_;
};

class SignalReceiver_EnvironmentLight_ParentEntityAboutToBeDetached : public SignalReceiver
{
public:
    void OnSignal()
    {
        duk_context* ctx = ctx_;
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, "_OnSignal");
        duk_remove(ctx, -2);
        duk_push_number(ctx, (size_t)key_);
        duk_push_array(ctx);
        bool success = duk_pcall(ctx, 2) == 0;
        if (!success) LogError("[JavaScript] OnSignal: " + String(duk_safe_to_string(ctx, -1)));
        duk_pop(ctx);
    }
};

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached>(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_ID);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Connect(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached>(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_ID);
    if (!wrapper->owner_) return 0;
    HashMap<void*, SharedPtr<SignalReceiver> >& signalReceivers = JavaScriptInstance::InstanceFromContext(ctx)->SignalReceivers();
    if (signalReceivers.Find(wrapper->signal_) == signalReceivers.End())
    {
        SignalReceiver_EnvironmentLight_ParentEntityAboutToBeDetached* receiver = new SignalReceiver_EnvironmentLight_ParentEntityAboutToBeDetached();
        receiver->ctx_ = ctx;
        receiver->key_ = wrapper->signal_;
        wrapper->signal_->Connect(receiver, &SignalReceiver_EnvironmentLight_ParentEntityAboutToBeDetached::OnSignal);
        signalReceivers[wrapper->signal_] = receiver;
    }
    CallConnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Disconnect(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached>(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_ID);
    if (!wrapper->owner_) return 0;
    CallDisconnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Emit(duk_context* ctx)
{
    SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached* wrapper = GetThisValueObject<SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached>(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_ID);
    if (!wrapper->owner_) return 0;
    wrapper->signal_->Emit();
    return 0;
}

static duk_ret_t EnvironmentLight_Get_ParentEntityAboutToBeDetached(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached* wrapper = new SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached(thisObj, &thisObj->ParentEntityAboutToBeDetached);
    PushValueObject(ctx, wrapper, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_ID, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Finalizer, false);
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Connect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "connect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Disconnect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "disconnect");
    duk_push_c_function(ctx, SignalWrapper_EnvironmentLight_ParentEntityAboutToBeDetached_Emit, 0);
    duk_put_prop_string(ctx, -2, "Emit");
    return 1;
}

static duk_ret_t EnvironmentLight_TypeName(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    const String & ret = thisObj->TypeName();
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t EnvironmentLight_TypeId(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    u32 ret = thisObj->TypeId();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_Name(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    const String & ret = thisObj->Name();
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t EnvironmentLight_SetName_String(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    String name = duk_require_string(ctx, 0);
    thisObj->SetName(name);
    return 0;
}

static duk_ret_t EnvironmentLight_SetParentEntity_Entity(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    Entity* entity = GetWeakObject<Entity>(ctx, 0);
    thisObj->SetParentEntity(entity);
    return 0;
}

static duk_ret_t EnvironmentLight_SetReplicated_bool(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool enable = duk_require_boolean(ctx, 0);
    thisObj->SetReplicated(enable);
    return 0;
}

static duk_ret_t EnvironmentLight_IsReplicated(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool ret = thisObj->IsReplicated();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_IsLocal(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool ret = thisObj->IsLocal();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_IsUnacked(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool ret = thisObj->IsUnacked();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_SetUpdateMode_AttributeChange__Type(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    AttributeChange::Type defaultmode = (AttributeChange::Type)(int)duk_require_number(ctx, 0);
    thisObj->SetUpdateMode(defaultmode);
    return 0;
}

static duk_ret_t EnvironmentLight_UpdateMode(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    AttributeChange::Type ret = thisObj->UpdateMode();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_Id(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    component_id_t ret = thisObj->Id();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_SupportsDynamicAttributes(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool ret = thisObj->SupportsDynamicAttributes();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_NumAttributes(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    int ret = thisObj->NumAttributes();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_NumStaticAttributes(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    int ret = thisObj->NumStaticAttributes();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_EmitAttributeChanged_String_AttributeChange__Type(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    String attributeName = duk_require_string(ctx, 0);
    AttributeChange::Type change = (AttributeChange::Type)(int)duk_require_number(ctx, 1);
    thisObj->EmitAttributeChanged(attributeName, change);
    return 0;
}

static duk_ret_t EnvironmentLight_ComponentChanged_AttributeChange__Type(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    AttributeChange::Type change = (AttributeChange::Type)(int)duk_require_number(ctx, 0);
    thisObj->ComponentChanged(change);
    return 0;
}

static duk_ret_t EnvironmentLight_ParentEntity(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    Entity * ret = thisObj->ParentEntity();
    PushWeakObject(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_SetTemporary_bool(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool enable = duk_require_boolean(ctx, 0);
    thisObj->SetTemporary(enable);
    return 0;
}

static duk_ret_t EnvironmentLight_IsTemporary(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool ret = thisObj->IsTemporary();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_ViewEnabled(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool ret = thisObj->ViewEnabled();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_AttributeNames(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    StringVector ret = thisObj->AttributeNames();
    PushStringVector(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_AttributeIds(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    StringVector ret = thisObj->AttributeIds();
    PushStringVector(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_ShouldBeSerialized_bool_bool(duk_context* ctx)
{
    EnvironmentLight* thisObj = GetThisWeakObject<EnvironmentLight>(ctx);
    bool serializeTemporary = duk_require_boolean(ctx, 0);
    bool serializeLocal = duk_require_boolean(ctx, 1);
    bool ret = thisObj->ShouldBeSerialized(serializeTemporary, serializeLocal);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t EnvironmentLight_EnsureTypeNameWithoutPrefix_Static_String(duk_context* ctx)
{
    String tn = duk_require_string(ctx, 0);
    String ret = EnvironmentLight::EnsureTypeNameWithoutPrefix(tn);
    duk_push_string(ctx, ret.CString());
    return 1;
}

static const duk_function_list_entry EnvironmentLight_Functions[] = {
    {"TypeName", EnvironmentLight_TypeName, 0}
    ,{"TypeId", EnvironmentLight_TypeId, 0}
    ,{"Name", EnvironmentLight_Name, 0}
    ,{"SetName", EnvironmentLight_SetName_String, 1}
    ,{"SetParentEntity", EnvironmentLight_SetParentEntity_Entity, 1}
    ,{"SetReplicated", EnvironmentLight_SetReplicated_bool, 1}
    ,{"IsReplicated", EnvironmentLight_IsReplicated, 0}
    ,{"IsLocal", EnvironmentLight_IsLocal, 0}
    ,{"IsUnacked", EnvironmentLight_IsUnacked, 0}
    ,{"SetUpdateMode", EnvironmentLight_SetUpdateMode_AttributeChange__Type, 1}
    ,{"UpdateMode", EnvironmentLight_UpdateMode, 0}
    ,{"Id", EnvironmentLight_Id, 0}
    ,{"SupportsDynamicAttributes", EnvironmentLight_SupportsDynamicAttributes, 0}
    ,{"NumAttributes", EnvironmentLight_NumAttributes, 0}
    ,{"NumStaticAttributes", EnvironmentLight_NumStaticAttributes, 0}
    ,{"EmitAttributeChanged", EnvironmentLight_EmitAttributeChanged_String_AttributeChange__Type, 2}
    ,{"ComponentChanged", EnvironmentLight_ComponentChanged_AttributeChange__Type, 1}
    ,{"ParentEntity", EnvironmentLight_ParentEntity, 0}
    ,{"SetTemporary", EnvironmentLight_SetTemporary_bool, 1}
    ,{"IsTemporary", EnvironmentLight_IsTemporary, 0}
    ,{"ViewEnabled", EnvironmentLight_ViewEnabled, 0}
    ,{"AttributeNames", EnvironmentLight_AttributeNames, 0}
    ,{"AttributeIds", EnvironmentLight_AttributeIds, 0}
    ,{"ShouldBeSerialized", EnvironmentLight_ShouldBeSerialized_bool_bool, 2}
    ,{nullptr, nullptr, 0}
};

static const duk_function_list_entry EnvironmentLight_StaticFunctions[] = {
    {"EnsureTypeNameWithoutPrefix", EnvironmentLight_EnsureTypeNameWithoutPrefix_Static_String, 1}
    ,{nullptr, nullptr, 0}
};

void Expose_EnvironmentLight(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, EnvironmentLight_StaticFunctions);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, EnvironmentLight_Functions);
    DefineProperty(ctx, "ComponentNameChanged", EnvironmentLight_Get_ComponentNameChanged, nullptr);
    DefineProperty(ctx, "ParentEntitySet", EnvironmentLight_Get_ParentEntitySet, nullptr);
    DefineProperty(ctx, "ParentEntityAboutToBeDetached", EnvironmentLight_Get_ParentEntityAboutToBeDetached, nullptr);
    DefineProperty(ctx, "typeName", EnvironmentLight_TypeName, nullptr);
    DefineProperty(ctx, "typeId", EnvironmentLight_TypeId, nullptr);
    DefineProperty(ctx, "name", EnvironmentLight_Name, EnvironmentLight_SetName_String);
    DefineProperty(ctx, "replicated", EnvironmentLight_IsReplicated, EnvironmentLight_SetReplicated_bool);
    DefineProperty(ctx, "local", EnvironmentLight_IsLocal, nullptr);
    DefineProperty(ctx, "unacked", EnvironmentLight_IsUnacked, nullptr);
    DefineProperty(ctx, "updateMode", EnvironmentLight_UpdateMode, EnvironmentLight_SetUpdateMode_AttributeChange__Type);
    DefineProperty(ctx, "id", EnvironmentLight_Id, nullptr);
    DefineProperty(ctx, "temporary", EnvironmentLight_IsTemporary, EnvironmentLight_SetTemporary_bool);
    DefineProperty(ctx, "viewEnabled", EnvironmentLight_ViewEnabled, nullptr);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, EnvironmentLight_ID);
}

}
