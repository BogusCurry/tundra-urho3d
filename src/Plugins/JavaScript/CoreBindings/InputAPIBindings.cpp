// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "CoreTypes.h"
#include "JavaScriptInstance.h"
#include "LoggingFunctions.h"
#include "Input/InputAPI.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800)
#endif

#include "Framework/Framework.h"
#include "Input/KeyEvent.h"
#include "Input/MouseEvent.h"
#include "Input/InputContext.h"
#include "Math/Point.h"


using namespace Tundra;
using namespace std;

namespace JSBindings
{

static const char* Point_ID = "Point";

static duk_ret_t Point_Finalizer(duk_context* ctx)
{
    FinalizeValueObject<Point>(ctx, Point_ID);
    return 0;
}


static const char* InputAPI_ID = "InputAPI";

static duk_ret_t InputAPI_Reset(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    thisObj->Reset();
    return 0;
}

static duk_ret_t InputAPI_Update_float(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    float frametime = (float)duk_require_number(ctx, 0);
    thisObj->Update(frametime);
    return 0;
}

static duk_ret_t InputAPI_SetPriority_InputContextPtr_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    SharedPtr<InputContext> inputContext(GetWeakObject<InputContext>(ctx, 0));
    int newPriority = (int)duk_require_number(ctx, 1);
    thisObj->SetPriority(inputContext, newPriority);
    return 0;
}

static duk_ret_t InputAPI_SetReleaseInputWhenApplicationInactive_bool(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    bool releaseInput = duk_require_boolean(ctx, 0);
    thisObj->SetReleaseInputWhenApplicationInactive(releaseInput);
    return 0;
}

static duk_ret_t InputAPI_TriggerKeyEvent_KeyEvent(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    KeyEvent& key = *GetCheckedWeakObject<KeyEvent>(ctx, 0);
    thisObj->TriggerKeyEvent(key);
    return 0;
}

static duk_ret_t InputAPI_TriggerMouseEvent_MouseEvent(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    MouseEvent& mouse = *GetCheckedWeakObject<MouseEvent>(ctx, 0);
    thisObj->TriggerMouseEvent(mouse);
    return 0;
}

static duk_ret_t InputAPI_Fw(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    Framework * ret = thisObj->Fw();
    PushWeakObject(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_RegisterInputContextRaw_String_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    String name = duk_require_string(ctx, 0);
    int priority = (int)duk_require_number(ctx, 1);
    InputContext * ret = thisObj->RegisterInputContextRaw(name, priority);
    PushWeakObject(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_UnregisterInputContextRaw_String(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    String name = duk_require_string(ctx, 0);
    thisObj->UnregisterInputContextRaw(name);
    return 0;
}

static duk_ret_t InputAPI_SetMouseCursorVisible_bool(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    bool visible = duk_require_boolean(ctx, 0);
    thisObj->SetMouseCursorVisible(visible);
    return 0;
}

static duk_ret_t InputAPI_IsMouseCursorVisible(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    bool ret = thisObj->IsMouseCursorVisible();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsKeyDown_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int keyCode = (int)duk_require_number(ctx, 0);
    bool ret = thisObj->IsKeyDown(keyCode);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsKeyPressed_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int keyCode = (int)duk_require_number(ctx, 0);
    bool ret = thisObj->IsKeyPressed(keyCode);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsKeyReleased_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int keyCode = (int)duk_require_number(ctx, 0);
    bool ret = thisObj->IsKeyReleased(keyCode);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsMouseButtonDown_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int mouseButton = (int)duk_require_number(ctx, 0);
    bool ret = thisObj->IsMouseButtonDown(mouseButton);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsMouseButtonPressed_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int mouseButton = (int)duk_require_number(ctx, 0);
    bool ret = thisObj->IsMouseButtonPressed(mouseButton);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsMouseButtonReleased_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int mouseButton = (int)duk_require_number(ctx, 0);
    bool ret = thisObj->IsMouseButtonReleased(mouseButton);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_GetMouseMoveX(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int ret = thisObj->GetMouseMoveX();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_GetMouseMoveY(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int ret = thisObj->GetMouseMoveY();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_GetNumTouches(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    unsigned ret = thisObj->GetNumTouches();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_IsGesturesEnabled(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    bool ret = thisObj->IsGesturesEnabled();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_MousePressedPos_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int mouseButton = (int)duk_require_number(ctx, 0);
    Point ret = thisObj->MousePressedPos(mouseButton);
    PushValueObjectCopy<Point>(ctx, ret, Point_ID, Point_Finalizer);
    return 1;
}

static duk_ret_t InputAPI_MousePos(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    Point ret = thisObj->MousePos();
    PushValueObjectCopy<Point>(ctx, ret, Point_ID, Point_Finalizer);
    return 1;
}

static duk_ret_t InputAPI_TopLevelInputContext(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    InputContext * ret = thisObj->TopLevelInputContext();
    PushWeakObject(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_SetKeyBinding_String_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    String actionName = duk_require_string(ctx, 0);
    int key = (int)duk_require_number(ctx, 1);
    thisObj->SetKeyBinding(actionName, key);
    return 0;
}

static duk_ret_t InputAPI_KeyBinding_String(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    String actionName = duk_require_string(ctx, 0);
    KeySequence ret = thisObj->KeyBinding(actionName);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_KeyBinding_String_int(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    String actionName = duk_require_string(ctx, 0);
    int defaultKey = (int)duk_require_number(ctx, 1);
    KeySequence ret = thisObj->KeyBinding(actionName, defaultKey);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_RemoveKeyBinding_String(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    String actionName = duk_require_string(ctx, 0);
    thisObj->RemoveKeyBinding(actionName);
    return 0;
}

static duk_ret_t InputAPI_SceneReleaseAllKeys(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    thisObj->SceneReleaseAllKeys();
    return 0;
}

static duk_ret_t InputAPI_SceneReleaseMouseButtons(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    thisObj->SceneReleaseMouseButtons();
    return 0;
}

static duk_ret_t InputAPI_LoadKeyBindingsFromFile(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    thisObj->LoadKeyBindingsFromFile();
    return 0;
}

static duk_ret_t InputAPI_SaveKeyBindingsToFile(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    thisObj->SaveKeyBindingsToFile();
    return 0;
}

static duk_ret_t InputAPI_DumpInputContexts(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    thisObj->DumpInputContexts();
    return 0;
}

static duk_ret_t InputAPI_NumTouchPoints(duk_context* ctx)
{
    InputAPI* thisObj = GetThisWeakObject<InputAPI>(ctx);
    int ret = thisObj->NumTouchPoints();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t InputAPI_KeyBinding_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 2 && duk_is_string(ctx, 0) && duk_is_number(ctx, 1))
        return InputAPI_KeyBinding_String_int(ctx);
    if (numArgs == 1 && duk_is_string(ctx, 0))
        return InputAPI_KeyBinding_String(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static const duk_function_list_entry InputAPI_Functions[] = {
    {"Reset", InputAPI_Reset, 0}
    ,{"Update", InputAPI_Update_float, 1}
    ,{"SetPriority", InputAPI_SetPriority_InputContextPtr_int, 2}
    ,{"SetReleaseInputWhenApplicationInactive", InputAPI_SetReleaseInputWhenApplicationInactive_bool, 1}
    ,{"TriggerKeyEvent", InputAPI_TriggerKeyEvent_KeyEvent, 1}
    ,{"TriggerMouseEvent", InputAPI_TriggerMouseEvent_MouseEvent, 1}
    ,{"Fw", InputAPI_Fw, 0}
    ,{"RegisterInputContextRaw", InputAPI_RegisterInputContextRaw_String_int, 2}
    ,{"UnregisterInputContextRaw", InputAPI_UnregisterInputContextRaw_String, 1}
    ,{"SetMouseCursorVisible", InputAPI_SetMouseCursorVisible_bool, 1}
    ,{"IsMouseCursorVisible", InputAPI_IsMouseCursorVisible, 0}
    ,{"IsKeyDown", InputAPI_IsKeyDown_int, 1}
    ,{"IsKeyPressed", InputAPI_IsKeyPressed_int, 1}
    ,{"IsKeyReleased", InputAPI_IsKeyReleased_int, 1}
    ,{"IsMouseButtonDown", InputAPI_IsMouseButtonDown_int, 1}
    ,{"IsMouseButtonPressed", InputAPI_IsMouseButtonPressed_int, 1}
    ,{"IsMouseButtonReleased", InputAPI_IsMouseButtonReleased_int, 1}
    ,{"GetMouseMoveX", InputAPI_GetMouseMoveX, 0}
    ,{"GetMouseMoveY", InputAPI_GetMouseMoveY, 0}
    ,{"GetNumTouches", InputAPI_GetNumTouches, 0}
    ,{"IsGesturesEnabled", InputAPI_IsGesturesEnabled, 0}
    ,{"MousePressedPos", InputAPI_MousePressedPos_int, 1}
    ,{"MousePos", InputAPI_MousePos, 0}
    ,{"TopLevelInputContext", InputAPI_TopLevelInputContext, 0}
    ,{"SetKeyBinding", InputAPI_SetKeyBinding_String_int, 2}
    ,{"KeyBinding", InputAPI_KeyBinding_Selector, DUK_VARARGS}
    ,{"RemoveKeyBinding", InputAPI_RemoveKeyBinding_String, 1}
    ,{"SceneReleaseAllKeys", InputAPI_SceneReleaseAllKeys, 0}
    ,{"SceneReleaseMouseButtons", InputAPI_SceneReleaseMouseButtons, 0}
    ,{"LoadKeyBindingsFromFile", InputAPI_LoadKeyBindingsFromFile, 0}
    ,{"SaveKeyBindingsToFile", InputAPI_SaveKeyBindingsToFile, 0}
    ,{"DumpInputContexts", InputAPI_DumpInputContexts, 0}
    ,{"NumTouchPoints", InputAPI_NumTouchPoints, 0}
    ,{nullptr, nullptr, 0}
};

void Expose_InputAPI(duk_context* ctx)
{
    duk_push_object(ctx);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, InputAPI_Functions);
    DefineProperty(ctx, "mouseMoveX", InputAPI_GetMouseMoveX, nullptr);
    DefineProperty(ctx, "mouseMoveY", InputAPI_GetMouseMoveY, nullptr);
    DefineProperty(ctx, "numTouches", InputAPI_GetNumTouches, nullptr);
    DefineProperty(ctx, "gesturesEnabled", InputAPI_IsGesturesEnabled, nullptr);
    DefineProperty(ctx, "mousePos", InputAPI_MousePos, nullptr);
    DefineProperty(ctx, "numTouchPoints", InputAPI_NumTouchPoints, nullptr);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, InputAPI_ID);
}

}