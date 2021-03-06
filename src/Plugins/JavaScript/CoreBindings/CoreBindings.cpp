// For conditions of distribution and use, see copyright notice in LICENSE

#include "StableHeaders.h"
#include "CoreBindings.h"

namespace JSBindings
{

void Expose_Entity(duk_context* ctx);
void Expose_EntityAction(duk_context* ctx);
void Expose_IComponent(duk_context* ctx);
void Expose_Name(duk_context* ctx);
void Expose_DynamicComponent(duk_context* ctx);
void Expose_Script(duk_context* ctx);
void Expose_AttributeChange(duk_context* ctx);
void Expose_Scene(duk_context* ctx);
void Expose_Color(duk_context* ctx);
void Expose_Point(duk_context* ctx);
void Expose_Transform(duk_context* ctx);
void Expose_AssetReference(duk_context* ctx);
void Expose_AssetReferenceList(duk_context* ctx);
void Expose_EntityReference(duk_context* ctx);
void Expose_RayQueryResult(duk_context* ctx);
void Expose_Framework(duk_context* ctx);
void Expose_FrameAPI(duk_context* ctx);
void Expose_SceneAPI(duk_context* ctx);
void Expose_ConfigAPI(duk_context* ctx);
void Expose_ConsoleAPI(duk_context* ctx);
void Expose_AssetAPI(duk_context* ctx);
void Expose_IAsset(duk_context* ctx);
void Expose_IAssetTransfer(duk_context* ctx);
void Expose_IAssetStorage(duk_context* ctx);
void Expose_IAssetBundle(duk_context* ctx);
void Expose_InputAPI(duk_context* ctx);
void Expose_InputContext(duk_context* ctx);
void Expose_KeyEvent(duk_context* ctx);
void Expose_MouseEvent(duk_context* ctx);

void ExposeCoreClasses(duk_context* ctx)
{
    Expose_Entity(ctx);
    Expose_EntityAction(ctx);
    Expose_IComponent(ctx);
    Expose_Name(ctx);
    Expose_DynamicComponent(ctx);
    Expose_Script(ctx);
    Expose_AttributeChange(ctx);
    Expose_Scene(ctx);
    Expose_Color(ctx);
    Expose_Point(ctx);
    Expose_Transform(ctx);
    Expose_AssetReference(ctx);
    Expose_AssetReferenceList(ctx);
    Expose_EntityReference(ctx);
    Expose_RayQueryResult(ctx);
    Expose_Framework(ctx);
    Expose_FrameAPI(ctx);
    Expose_SceneAPI(ctx);
    Expose_ConfigAPI(ctx);
    Expose_ConsoleAPI(ctx);
    Expose_AssetAPI(ctx);
    Expose_IAsset(ctx);
    Expose_IAssetTransfer(ctx);
    Expose_IAssetStorage(ctx);
    Expose_IAssetBundle(ctx);
    Expose_InputAPI(ctx);
    Expose_InputContext(ctx);
    Expose_KeyEvent(ctx);
    Expose_MouseEvent(ctx);
}

}