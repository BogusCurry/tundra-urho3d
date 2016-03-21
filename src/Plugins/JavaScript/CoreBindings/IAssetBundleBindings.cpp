// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "CoreTypes.h"
#include "JavaScriptInstance.h"
#include "LoggingFunctions.h"
#include "Asset/IAssetBundle.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800)
#endif

#include "Asset/AssetAPI.h"


using namespace Tundra;
using namespace std;

namespace JSBindings
{



static const char* IAssetBundle_ID = "IAssetBundle";

const char* SignalWrapper_IAssetBundle_Loaded_ID = "SignalWrapper_IAssetBundle_Loaded";

class SignalWrapper_IAssetBundle_Loaded
{
public:
    SignalWrapper_IAssetBundle_Loaded(Object* owner, Signal1< IAssetBundle * >* signal) :
        owner_(owner),
        signal_(signal)
    {
    }

    WeakPtr<Object> owner_;
    Signal1< IAssetBundle * >* signal_;
};

class SignalReceiver_IAssetBundle_Loaded : public SignalReceiver
{
public:
    void OnSignal(IAssetBundle * param0)
    {
        duk_context* ctx = ctx_;
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, "_OnSignal");
        duk_remove(ctx, -2);
        duk_push_number(ctx, (size_t)key_);
        duk_push_array(ctx);
        PushWeakObject(ctx, param0);
        duk_put_prop_index(ctx, -2, 0);
        bool success = duk_pcall(ctx, 2) == 0;
        if (!success) LogError("[JavaScript] OnSignal: " + String(duk_safe_to_string(ctx, -1)));
        duk_pop(ctx);
    }
};

static duk_ret_t SignalWrapper_IAssetBundle_Loaded_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<SignalWrapper_IAssetBundle_Loaded>(ctx, SignalWrapper_IAssetBundle_Loaded_ID);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Loaded_Connect(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Loaded* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Loaded>(ctx, SignalWrapper_IAssetBundle_Loaded_ID);
    if (!wrapper->owner_) return 0;
    HashMap<void*, SharedPtr<SignalReceiver> >& signalReceivers = JavaScriptInstance::InstanceFromContext(ctx)->SignalReceivers();
    if (signalReceivers.Find(wrapper->signal_) == signalReceivers.End())
    {
        SignalReceiver_IAssetBundle_Loaded* receiver = new SignalReceiver_IAssetBundle_Loaded();
        receiver->ctx_ = ctx;
        receiver->key_ = wrapper->signal_;
        wrapper->signal_->Connect(receiver, &SignalReceiver_IAssetBundle_Loaded::OnSignal);
        signalReceivers[wrapper->signal_] = receiver;
    }
    CallConnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Loaded_Disconnect(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Loaded* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Loaded>(ctx, SignalWrapper_IAssetBundle_Loaded_ID);
    if (!wrapper->owner_) return 0;
    CallDisconnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Loaded_Emit(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Loaded* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Loaded>(ctx, SignalWrapper_IAssetBundle_Loaded_ID);
    if (!wrapper->owner_) return 0;
    IAssetBundle* param0 = GetWeakObject<IAssetBundle>(ctx, 0);
    wrapper->signal_->Emit(param0);
    return 0;
}

static duk_ret_t IAssetBundle_Get_Loaded(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    SignalWrapper_IAssetBundle_Loaded* wrapper = new SignalWrapper_IAssetBundle_Loaded(thisObj, &thisObj->Loaded);
    PushValueObject(ctx, wrapper, SignalWrapper_IAssetBundle_Loaded_ID, SignalWrapper_IAssetBundle_Loaded_Finalizer, false);
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Loaded_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Connect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Loaded_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "connect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Loaded_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Disconnect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Loaded_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "disconnect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Loaded_Emit, 1);
    duk_put_prop_string(ctx, -2, "Emit");
    return 1;
}

const char* SignalWrapper_IAssetBundle_Unloaded_ID = "SignalWrapper_IAssetBundle_Unloaded";

class SignalWrapper_IAssetBundle_Unloaded
{
public:
    SignalWrapper_IAssetBundle_Unloaded(Object* owner, Signal1< IAssetBundle * >* signal) :
        owner_(owner),
        signal_(signal)
    {
    }

    WeakPtr<Object> owner_;
    Signal1< IAssetBundle * >* signal_;
};

class SignalReceiver_IAssetBundle_Unloaded : public SignalReceiver
{
public:
    void OnSignal(IAssetBundle * param0)
    {
        duk_context* ctx = ctx_;
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, "_OnSignal");
        duk_remove(ctx, -2);
        duk_push_number(ctx, (size_t)key_);
        duk_push_array(ctx);
        PushWeakObject(ctx, param0);
        duk_put_prop_index(ctx, -2, 0);
        bool success = duk_pcall(ctx, 2) == 0;
        if (!success) LogError("[JavaScript] OnSignal: " + String(duk_safe_to_string(ctx, -1)));
        duk_pop(ctx);
    }
};

static duk_ret_t SignalWrapper_IAssetBundle_Unloaded_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<SignalWrapper_IAssetBundle_Unloaded>(ctx, SignalWrapper_IAssetBundle_Unloaded_ID);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Unloaded_Connect(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Unloaded* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Unloaded>(ctx, SignalWrapper_IAssetBundle_Unloaded_ID);
    if (!wrapper->owner_) return 0;
    HashMap<void*, SharedPtr<SignalReceiver> >& signalReceivers = JavaScriptInstance::InstanceFromContext(ctx)->SignalReceivers();
    if (signalReceivers.Find(wrapper->signal_) == signalReceivers.End())
    {
        SignalReceiver_IAssetBundle_Unloaded* receiver = new SignalReceiver_IAssetBundle_Unloaded();
        receiver->ctx_ = ctx;
        receiver->key_ = wrapper->signal_;
        wrapper->signal_->Connect(receiver, &SignalReceiver_IAssetBundle_Unloaded::OnSignal);
        signalReceivers[wrapper->signal_] = receiver;
    }
    CallConnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Unloaded_Disconnect(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Unloaded* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Unloaded>(ctx, SignalWrapper_IAssetBundle_Unloaded_ID);
    if (!wrapper->owner_) return 0;
    CallDisconnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Unloaded_Emit(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Unloaded* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Unloaded>(ctx, SignalWrapper_IAssetBundle_Unloaded_ID);
    if (!wrapper->owner_) return 0;
    IAssetBundle* param0 = GetWeakObject<IAssetBundle>(ctx, 0);
    wrapper->signal_->Emit(param0);
    return 0;
}

static duk_ret_t IAssetBundle_Get_Unloaded(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    SignalWrapper_IAssetBundle_Unloaded* wrapper = new SignalWrapper_IAssetBundle_Unloaded(thisObj, &thisObj->Unloaded);
    PushValueObject(ctx, wrapper, SignalWrapper_IAssetBundle_Unloaded_ID, SignalWrapper_IAssetBundle_Unloaded_Finalizer, false);
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Unloaded_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Connect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Unloaded_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "connect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Unloaded_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Disconnect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Unloaded_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "disconnect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Unloaded_Emit, 1);
    duk_put_prop_string(ctx, -2, "Emit");
    return 1;
}

const char* SignalWrapper_IAssetBundle_Failed_ID = "SignalWrapper_IAssetBundle_Failed";

class SignalWrapper_IAssetBundle_Failed
{
public:
    SignalWrapper_IAssetBundle_Failed(Object* owner, Signal1< IAssetBundle * >* signal) :
        owner_(owner),
        signal_(signal)
    {
    }

    WeakPtr<Object> owner_;
    Signal1< IAssetBundle * >* signal_;
};

class SignalReceiver_IAssetBundle_Failed : public SignalReceiver
{
public:
    void OnSignal(IAssetBundle * param0)
    {
        duk_context* ctx = ctx_;
        duk_push_global_object(ctx);
        duk_get_prop_string(ctx, -1, "_OnSignal");
        duk_remove(ctx, -2);
        duk_push_number(ctx, (size_t)key_);
        duk_push_array(ctx);
        PushWeakObject(ctx, param0);
        duk_put_prop_index(ctx, -2, 0);
        bool success = duk_pcall(ctx, 2) == 0;
        if (!success) LogError("[JavaScript] OnSignal: " + String(duk_safe_to_string(ctx, -1)));
        duk_pop(ctx);
    }
};

static duk_ret_t SignalWrapper_IAssetBundle_Failed_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<SignalWrapper_IAssetBundle_Failed>(ctx, SignalWrapper_IAssetBundle_Failed_ID);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Failed_Connect(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Failed* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Failed>(ctx, SignalWrapper_IAssetBundle_Failed_ID);
    if (!wrapper->owner_) return 0;
    HashMap<void*, SharedPtr<SignalReceiver> >& signalReceivers = JavaScriptInstance::InstanceFromContext(ctx)->SignalReceivers();
    if (signalReceivers.Find(wrapper->signal_) == signalReceivers.End())
    {
        SignalReceiver_IAssetBundle_Failed* receiver = new SignalReceiver_IAssetBundle_Failed();
        receiver->ctx_ = ctx;
        receiver->key_ = wrapper->signal_;
        wrapper->signal_->Connect(receiver, &SignalReceiver_IAssetBundle_Failed::OnSignal);
        signalReceivers[wrapper->signal_] = receiver;
    }
    CallConnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Failed_Disconnect(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Failed* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Failed>(ctx, SignalWrapper_IAssetBundle_Failed_ID);
    if (!wrapper->owner_) return 0;
    CallDisconnectSignal(ctx, wrapper->signal_);
    return 0;
}

static duk_ret_t SignalWrapper_IAssetBundle_Failed_Emit(duk_context* ctx)
{
    SignalWrapper_IAssetBundle_Failed* wrapper = GetThisValueObject<SignalWrapper_IAssetBundle_Failed>(ctx, SignalWrapper_IAssetBundle_Failed_ID);
    if (!wrapper->owner_) return 0;
    IAssetBundle* param0 = GetWeakObject<IAssetBundle>(ctx, 0);
    wrapper->signal_->Emit(param0);
    return 0;
}

static duk_ret_t IAssetBundle_Get_Failed(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    SignalWrapper_IAssetBundle_Failed* wrapper = new SignalWrapper_IAssetBundle_Failed(thisObj, &thisObj->Failed);
    PushValueObject(ctx, wrapper, SignalWrapper_IAssetBundle_Failed_ID, SignalWrapper_IAssetBundle_Failed_Finalizer, false);
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Failed_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Connect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Failed_Connect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "connect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Failed_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "Disconnect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Failed_Disconnect, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "disconnect");
    duk_push_c_function(ctx, SignalWrapper_IAssetBundle_Failed_Emit, 1);
    duk_put_prop_string(ctx, -2, "Emit");
    return 1;
}

static duk_ret_t IAssetBundle_IsLoaded(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    bool ret = thisObj->IsLoaded();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t IAssetBundle_RequiresDiskSource(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    bool ret = thisObj->RequiresDiskSource();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t IAssetBundle_DeserializeFromDiskSource(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    bool ret = thisObj->DeserializeFromDiskSource();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t IAssetBundle_GetSubAssetDiskSource_String(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    String subAssetName = duk_require_string(ctx, 0);
    String ret = thisObj->GetSubAssetDiskSource(subAssetName);
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t IAssetBundle_SubAssetCount(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    int ret = thisObj->SubAssetCount();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t IAssetBundle_Type(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    String ret = thisObj->Type();
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t IAssetBundle_Name(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    String ret = thisObj->Name();
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t IAssetBundle_AssetStorage(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    AssetStoragePtr ret = thisObj->AssetStorage();
    PushWeakObject(ctx, ret);
    return 1;
}

static duk_ret_t IAssetBundle_DiskSource(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    String ret = thisObj->DiskSource();
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t IAssetBundle_SetAssetStorage_AssetStoragePtr(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    SharedPtr<IAssetStorage> storage(GetWeakObject<IAssetStorage>(ctx, 0));
    thisObj->SetAssetStorage(storage);
    return 0;
}

static duk_ret_t IAssetBundle_SetDiskSource_String(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    String diskSource = duk_require_string(ctx, 0);
    thisObj->SetDiskSource(diskSource);
    return 0;
}

static duk_ret_t IAssetBundle_Unload(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    thisObj->Unload();
    return 0;
}

static duk_ret_t IAssetBundle_IsEmpty(duk_context* ctx)
{
    IAssetBundle* thisObj = GetThisWeakObject<IAssetBundle>(ctx);
    bool ret = thisObj->IsEmpty();
    duk_push_boolean(ctx, ret);
    return 1;
}

static const duk_function_list_entry IAssetBundle_Functions[] = {
    {"IsLoaded", IAssetBundle_IsLoaded, 0}
    ,{"RequiresDiskSource", IAssetBundle_RequiresDiskSource, 0}
    ,{"DeserializeFromDiskSource", IAssetBundle_DeserializeFromDiskSource, 0}
    ,{"GetSubAssetDiskSource", IAssetBundle_GetSubAssetDiskSource_String, 1}
    ,{"SubAssetCount", IAssetBundle_SubAssetCount, 0}
    ,{"Type", IAssetBundle_Type, 0}
    ,{"Name", IAssetBundle_Name, 0}
    ,{"AssetStorage", IAssetBundle_AssetStorage, 0}
    ,{"DiskSource", IAssetBundle_DiskSource, 0}
    ,{"SetAssetStorage", IAssetBundle_SetAssetStorage_AssetStoragePtr, 1}
    ,{"SetDiskSource", IAssetBundle_SetDiskSource_String, 1}
    ,{"Unload", IAssetBundle_Unload, 0}
    ,{"IsEmpty", IAssetBundle_IsEmpty, 0}
    ,{nullptr, nullptr, 0}
};

void Expose_IAssetBundle(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, IAssetBundle_Functions);
    DefineProperty(ctx, "Loaded", IAssetBundle_Get_Loaded, nullptr);
    DefineProperty(ctx, "Unloaded", IAssetBundle_Get_Unloaded, nullptr);
    DefineProperty(ctx, "Failed", IAssetBundle_Get_Failed, nullptr);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, IAssetBundle_ID);
}

}
