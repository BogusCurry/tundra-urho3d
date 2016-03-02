// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "CoreTypes.h"
#include "JavaScriptInstance.h"
#include "LoggingFunctions.h"
#include "Framework/ConfigAPI.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800)
#endif

#include "Framework/Framework.h"


using namespace Tundra;
using namespace std;

namespace JSBindings
{



const char* ConfigAPI_ID = "ConfigAPI";

static duk_ret_t ConfigAPI_HasKey_String_String_String(duk_context* ctx)
{
    ConfigAPI* thisObj = GetThisWeakObject<ConfigAPI>(ctx);
    String file(duk_require_string(ctx, 0));
    String section(duk_require_string(ctx, 1));
    String key(duk_require_string(ctx, 2));
    bool ret = thisObj->HasKey(file, section, key);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t ConfigAPI_Read_String_String_String_Variant(duk_context* ctx)
{
    ConfigAPI* thisObj = GetThisWeakObject<ConfigAPI>(ctx);
    String file(duk_require_string(ctx, 0));
    String section(duk_require_string(ctx, 1));
    String key(duk_require_string(ctx, 2));
    Variant defaultValue = GetVariant(ctx, 3);
    Variant ret = thisObj->Read(file, section, key, defaultValue);
    PushVariant(ctx, ret);
    return 1;
}

static duk_ret_t ConfigAPI_Write_String_String_String_Variant(duk_context* ctx)
{
    ConfigAPI* thisObj = GetThisWeakObject<ConfigAPI>(ctx);
    String file(duk_require_string(ctx, 0));
    String section(duk_require_string(ctx, 1));
    String key(duk_require_string(ctx, 2));
    Variant value = GetVariant(ctx, 3);
    bool ret = thisObj->Write(file, section, key, value);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t ConfigAPI_ConfigFolder(duk_context* ctx)
{
    ConfigAPI* thisObj = GetThisWeakObject<ConfigAPI>(ctx);
    String ret = thisObj->ConfigFolder();
    duk_push_string(ctx, ret.CString());
    return 1;
}

static duk_ret_t ConfigAPI_DeclareSetting_String_String_String_Variant(duk_context* ctx)
{
    ConfigAPI* thisObj = GetThisWeakObject<ConfigAPI>(ctx);
    String file(duk_require_string(ctx, 0));
    String section(duk_require_string(ctx, 1));
    String key(duk_require_string(ctx, 2));
    Variant defaultValue = GetVariant(ctx, 3);
    Variant ret = thisObj->DeclareSetting(file, section, key, defaultValue);
    PushVariant(ctx, ret);
    return 1;
}

static duk_ret_t ConfigAPI_WriteFile_String(duk_context* ctx)
{
    ConfigAPI* thisObj = GetThisWeakObject<ConfigAPI>(ctx);
    String file(duk_require_string(ctx, 0));
    thisObj->WriteFile(file);
    return 0;
}

static const duk_function_list_entry ConfigAPI_Functions[] = {
    {"HasKey", ConfigAPI_HasKey_String_String_String, 3}
    ,{"Read", ConfigAPI_Read_String_String_String_Variant, 4}
    ,{"Write", ConfigAPI_Write_String_String_String_Variant, 4}
    ,{"ConfigFolder", ConfigAPI_ConfigFolder, 0}
    ,{"DeclareSetting", ConfigAPI_DeclareSetting_String_String_String_Variant, 4}
    ,{"WriteFile", ConfigAPI_WriteFile_String, 1}
    ,{nullptr, nullptr, 0}
};

void Expose_ConfigAPI(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, ConfigAPI_Functions);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, ConfigAPI_ID);
}

}
