// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "BindingsHelpers.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "Math/float3x3.h"
#include "Math/float3x4.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Plane.h"
#include "Algorithm/Random/LCG.h"
#include "Geometry/Triangle.h"
#include "Geometry/Frustum.h"
#include "Geometry/Ray.h"
#include "Geometry/Line.h"
#include "Geometry/Capsule.h"
#include "Math/float2.h"

namespace JSBindings
{

extern const char* AABB_Id;
extern const char* float3x3_Id;
extern const char* float3x4_Id;
extern const char* float4x4_Id;
extern const char* Quat_Id;
extern const char* Sphere_Id;
extern const char* LineSegment_Id;
extern const char* Plane_Id;
extern const char* LCG_Id;
extern const char* Triangle_Id;
extern const char* Frustum_Id;
extern const char* Ray_Id;
extern const char* Line_Id;
extern const char* Capsule_Id;
extern const char* float2_Id;

duk_ret_t AABB_Dtor(duk_context* ctx);
duk_ret_t float3x3_Dtor(duk_context* ctx);
duk_ret_t float3x4_Dtor(duk_context* ctx);
duk_ret_t float4x4_Dtor(duk_context* ctx);
duk_ret_t Quat_Dtor(duk_context* ctx);
duk_ret_t Sphere_Dtor(duk_context* ctx);
duk_ret_t LineSegment_Dtor(duk_context* ctx);
duk_ret_t Plane_Dtor(duk_context* ctx);
duk_ret_t LCG_Dtor(duk_context* ctx);
duk_ret_t Triangle_Dtor(duk_context* ctx);
duk_ret_t Frustum_Dtor(duk_context* ctx);
duk_ret_t Ray_Dtor(duk_context* ctx);
duk_ret_t Line_Dtor(duk_context* ctx);
duk_ret_t Capsule_Dtor(duk_context* ctx);
duk_ret_t float2_Dtor(duk_context* ctx);

const char* OBB_Id = "OBB";

duk_ret_t OBB_Dtor(duk_context* ctx)
{
    OBB* obj = GetObject<OBB>(ctx, 0, OBB_Id);
    if (obj)
    {
        delete obj;
        SetObject(ctx, 0, 0, OBB_Id);
    }
    return 0;
}

static duk_ret_t OBB_Ctor(duk_context* ctx)
{
    OBB* newObj = new OBB();
    duk_push_this(ctx); SetObject(ctx, -1, newObj, OBB_Id); duk_push_c_function(ctx, OBB_Dtor, 1); duk_set_finalizer(ctx, -2);
    return 0;
}

static duk_ret_t OBB_Ctor_AABB(duk_context* ctx)
{
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    OBB* newObj = new OBB(*aabb);
    duk_push_this(ctx); SetObject(ctx, -1, newObj, OBB_Id); duk_push_c_function(ctx, OBB_Dtor, 1); duk_set_finalizer(ctx, -2);
    return 0;
}

static duk_ret_t OBB_SetNegativeInfinity(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    thisObj->SetNegativeInfinity();
    return 0;
}

static duk_ret_t OBB_SetFrom_AABB(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->SetFrom(*aabb);
    return 0;
}

static duk_ret_t OBB_SetFrom_AABB_float3x3(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float3x3* transform = GetObject<float3x3>(ctx, 1, float3x3_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->SetFrom(*aabb, *transform);
    return 0;
}

static duk_ret_t OBB_SetFrom_AABB_float3x4(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float3x4* transform = GetObject<float3x4>(ctx, 1, float3x4_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->SetFrom(*aabb, *transform);
    return 0;
}

static duk_ret_t OBB_SetFrom_AABB_float4x4(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float4x4* transform = GetObject<float4x4>(ctx, 1, float4x4_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->SetFrom(*aabb, *transform);
    return 0;
}

static duk_ret_t OBB_SetFrom_AABB_Quat(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    Quat* transform = GetObject<Quat>(ctx, 1, Quat_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->SetFrom(*aabb, *transform);
    return 0;
}

static duk_ret_t OBB_SetFrom_Sphere(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Sphere* sphere = GetObject<Sphere>(ctx, 0, Sphere_Id); if (!sphere) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->SetFrom(*sphere);
    return 0;
}

static duk_ret_t OBB_MinimalEnclosingAABB(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB ret = thisObj->MinimalEnclosingAABB();
    duk_push_object(ctx); SetObject(ctx, -1, new AABB(ret), AABB_Id); duk_push_c_function(ctx, AABB_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, AABB_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_MinimalEnclosingSphere(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Sphere ret = thisObj->MinimalEnclosingSphere();
    duk_push_object(ctx); SetObject(ctx, -1, new Sphere(ret), Sphere_Id); duk_push_c_function(ctx, Sphere_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, Sphere_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_MaximalContainedSphere(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Sphere ret = thisObj->MaximalContainedSphere();
    duk_push_object(ctx); SetObject(ctx, -1, new Sphere(ret), Sphere_Id); duk_push_c_function(ctx, Sphere_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, Sphere_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_WorldToLocal(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float3x4 ret = thisObj->WorldToLocal();
    duk_push_object(ctx); SetObject(ctx, -1, new float3x4(ret), float3x4_Id); duk_push_c_function(ctx, float3x4_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, float3x4_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_LocalToWorld(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float3x4 ret = thisObj->LocalToWorld();
    duk_push_object(ctx); SetObject(ctx, -1, new float3x4(ret), float3x4_Id); duk_push_c_function(ctx, float3x4_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, float3x4_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_IsFinite(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    bool ret = thisObj->IsFinite();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_IsDegenerate(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    bool ret = thisObj->IsDegenerate();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Volume(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float ret = thisObj->Volume();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t OBB_SurfaceArea(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float ret = thisObj->SurfaceArea();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Edge_int(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    int edgeIndex = (int)duk_require_number(ctx, 0);
    LineSegment ret = thisObj->Edge(edgeIndex);
    duk_push_object(ctx); SetObject(ctx, -1, new LineSegment(ret), LineSegment_Id); duk_push_c_function(ctx, LineSegment_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, LineSegment_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_FacePlane_int(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    int faceIndex = (int)duk_require_number(ctx, 0);
    Plane ret = thisObj->FacePlane(faceIndex);
    duk_push_object(ctx); SetObject(ctx, -1, new Plane(ret), Plane_Id); duk_push_c_function(ctx, Plane_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, Plane_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t OBB_Transform_float3x3(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float3x3* transform = GetObject<float3x3>(ctx, 0, float3x3_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t OBB_Transform_float3x4(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float3x4* transform = GetObject<float3x4>(ctx, 0, float3x4_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t OBB_Transform_float4x4(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float4x4* transform = GetObject<float4x4>(ctx, 0, float4x4_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t OBB_Transform_Quat(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Quat* transform = GetObject<Quat>(ctx, 0, Quat_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t OBB_Distance_Sphere(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Sphere* sphere = GetObject<Sphere>(ctx, 0, Sphere_Id); if (!sphere) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*sphere);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Contains_LineSegment(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* lineSegment = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!lineSegment) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Contains(*lineSegment);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Contains_AABB(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Contains(*aabb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Contains_OBB(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    OBB* obb = GetObject<OBB>(ctx, 0, OBB_Id); if (!obb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Contains(*obb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Contains_Triangle(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Triangle* triangle = GetObject<Triangle>(ctx, 0, Triangle_Id); if (!triangle) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Contains(*triangle);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Contains_Frustum(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Frustum* frustum = GetObject<Frustum>(ctx, 0, Frustum_Id); if (!frustum) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Contains(*frustum);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_OBB_float(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    OBB* obb = GetObject<OBB>(ctx, 0, OBB_Id); if (!obb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Intersects(*obb, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_AABB(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*aabb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Plane(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Plane* plane = GetObject<Plane>(ctx, 0, Plane_Id); if (!plane) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*plane);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Ray_float_float(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Ray* ray = GetObject<Ray>(ctx, 0, Ray_Id); if (!ray) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float dNear = (float)duk_require_number(ctx, 1);
    float dFar = (float)duk_require_number(ctx, 2);
    bool ret = thisObj->Intersects(*ray, dNear, dFar);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Ray(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Ray* ray = GetObject<Ray>(ctx, 0, Ray_Id); if (!ray) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*ray);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Line_float_float(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Line* line = GetObject<Line>(ctx, 0, Line_Id); if (!line) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float dNear = (float)duk_require_number(ctx, 1);
    float dFar = (float)duk_require_number(ctx, 2);
    bool ret = thisObj->Intersects(*line, dNear, dFar);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Line(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Line* line = GetObject<Line>(ctx, 0, Line_Id); if (!line) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*line);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_LineSegment_float_float(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* lineSegment = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!lineSegment) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float dNear = (float)duk_require_number(ctx, 1);
    float dFar = (float)duk_require_number(ctx, 2);
    bool ret = thisObj->Intersects(*lineSegment, dNear, dFar);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_LineSegment(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* lineSegment = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!lineSegment) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*lineSegment);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Capsule(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Capsule* capsule = GetObject<Capsule>(ctx, 0, Capsule_Id); if (!capsule) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*capsule);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Triangle(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Triangle* triangle = GetObject<Triangle>(ctx, 0, Triangle_Id); if (!triangle) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*triangle);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Intersects_Frustum(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Frustum* frustum = GetObject<Frustum>(ctx, 0, Frustum_Id); if (!frustum) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*frustum);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Equals_OBB_float(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    OBB* rhs = GetObject<OBB>(ctx, 0, OBB_Id); if (!rhs) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Equals(*rhs, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_BitEquals_OBB(duk_context* ctx)
{
    OBB* thisObj = GetThisObject<OBB>(ctx, OBB_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    OBB* other = GetObject<OBB>(ctx, 0, OBB_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->BitEquals(*other);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t OBB_Ctor_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 0)
        return OBB_Ctor(ctx);
    if (numArgs == 1 && GetObject<AABB>(ctx, 0, AABB_Id))
        return OBB_Ctor_AABB(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t OBB_SetFrom_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetObject<AABB>(ctx, 0, AABB_Id))
        return OBB_SetFrom_AABB(ctx);
    if (numArgs == 2 && GetObject<AABB>(ctx, 0, AABB_Id) && GetObject<float3x3>(ctx, 1, float3x3_Id))
        return OBB_SetFrom_AABB_float3x3(ctx);
    if (numArgs == 2 && GetObject<AABB>(ctx, 0, AABB_Id) && GetObject<float3x4>(ctx, 1, float3x4_Id))
        return OBB_SetFrom_AABB_float3x4(ctx);
    if (numArgs == 2 && GetObject<AABB>(ctx, 0, AABB_Id) && GetObject<float4x4>(ctx, 1, float4x4_Id))
        return OBB_SetFrom_AABB_float4x4(ctx);
    if (numArgs == 2 && GetObject<AABB>(ctx, 0, AABB_Id) && GetObject<Quat>(ctx, 1, Quat_Id))
        return OBB_SetFrom_AABB_Quat(ctx);
    if (numArgs == 1 && GetObject<Sphere>(ctx, 0, Sphere_Id))
        return OBB_SetFrom_Sphere(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t OBB_Transform_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetObject<float3x3>(ctx, 0, float3x3_Id))
        return OBB_Transform_float3x3(ctx);
    if (numArgs == 1 && GetObject<float3x4>(ctx, 0, float3x4_Id))
        return OBB_Transform_float3x4(ctx);
    if (numArgs == 1 && GetObject<float4x4>(ctx, 0, float4x4_Id))
        return OBB_Transform_float4x4(ctx);
    if (numArgs == 1 && GetObject<Quat>(ctx, 0, Quat_Id))
        return OBB_Transform_Quat(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t OBB_Contains_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetObject<LineSegment>(ctx, 0, LineSegment_Id))
        return OBB_Contains_LineSegment(ctx);
    if (numArgs == 1 && GetObject<AABB>(ctx, 0, AABB_Id))
        return OBB_Contains_AABB(ctx);
    if (numArgs == 1 && GetObject<OBB>(ctx, 0, OBB_Id))
        return OBB_Contains_OBB(ctx);
    if (numArgs == 1 && GetObject<Triangle>(ctx, 0, Triangle_Id))
        return OBB_Contains_Triangle(ctx);
    if (numArgs == 1 && GetObject<Frustum>(ctx, 0, Frustum_Id))
        return OBB_Contains_Frustum(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t OBB_Intersects_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 2 && GetObject<OBB>(ctx, 0, OBB_Id) && duk_is_number(ctx, 1))
        return OBB_Intersects_OBB_float(ctx);
    if (numArgs == 1 && GetObject<AABB>(ctx, 0, AABB_Id))
        return OBB_Intersects_AABB(ctx);
    if (numArgs == 1 && GetObject<Plane>(ctx, 0, Plane_Id))
        return OBB_Intersects_Plane(ctx);
    if (numArgs == 3 && GetObject<Ray>(ctx, 0, Ray_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return OBB_Intersects_Ray_float_float(ctx);
    if (numArgs == 1 && GetObject<Ray>(ctx, 0, Ray_Id))
        return OBB_Intersects_Ray(ctx);
    if (numArgs == 3 && GetObject<Line>(ctx, 0, Line_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return OBB_Intersects_Line_float_float(ctx);
    if (numArgs == 1 && GetObject<Line>(ctx, 0, Line_Id))
        return OBB_Intersects_Line(ctx);
    if (numArgs == 3 && GetObject<LineSegment>(ctx, 0, LineSegment_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return OBB_Intersects_LineSegment_float_float(ctx);
    if (numArgs == 1 && GetObject<LineSegment>(ctx, 0, LineSegment_Id))
        return OBB_Intersects_LineSegment(ctx);
    if (numArgs == 1 && GetObject<Capsule>(ctx, 0, Capsule_Id))
        return OBB_Intersects_Capsule(ctx);
    if (numArgs == 1 && GetObject<Triangle>(ctx, 0, Triangle_Id))
        return OBB_Intersects_Triangle(ctx);
    if (numArgs == 1 && GetObject<Frustum>(ctx, 0, Frustum_Id))
        return OBB_Intersects_Frustum(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t OBB_NumVerticesInTriangulation_Static_int_int_int(duk_context* ctx)
{
    int numFacesX = (int)duk_require_number(ctx, 0);
    int numFacesY = (int)duk_require_number(ctx, 1);
    int numFacesZ = (int)duk_require_number(ctx, 2);
    int ret = OBB::NumVerticesInTriangulation(numFacesX, numFacesY, numFacesZ);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t OBB_NumVerticesInEdgeList_Static(duk_context* ctx)
{
    int ret = OBB::NumVerticesInEdgeList();
    duk_push_number(ctx, ret);
    return 1;
}

static const duk_function_list_entry OBB_Functions[] = {
    {"SetNegativeInfinity", OBB_SetNegativeInfinity, 0}
    ,{"SetFrom", OBB_SetFrom_Selector, DUK_VARARGS}
    ,{"MinimalEnclosingAABB", OBB_MinimalEnclosingAABB, 0}
    ,{"MinimalEnclosingSphere", OBB_MinimalEnclosingSphere, 0}
    ,{"MaximalContainedSphere", OBB_MaximalContainedSphere, 0}
    ,{"WorldToLocal", OBB_WorldToLocal, 0}
    ,{"LocalToWorld", OBB_LocalToWorld, 0}
    ,{"IsFinite", OBB_IsFinite, 0}
    ,{"IsDegenerate", OBB_IsDegenerate, 0}
    ,{"Volume", OBB_Volume, 0}
    ,{"SurfaceArea", OBB_SurfaceArea, 0}
    ,{"Edge", OBB_Edge_int, 1}
    ,{"FacePlane", OBB_FacePlane_int, 1}
    ,{"Transform", OBB_Transform_Selector, DUK_VARARGS}
    ,{"Distance", OBB_Distance_Sphere, 1}
    ,{"Contains", OBB_Contains_Selector, DUK_VARARGS}
    ,{"Intersects", OBB_Intersects_Selector, DUK_VARARGS}
    ,{"Equals", OBB_Equals_OBB_float, 2}
    ,{"BitEquals", OBB_BitEquals_OBB, 1}
    ,{nullptr, nullptr, 0}
};

static const duk_function_list_entry OBB_StaticFunctions[] = {
    {"NumVerticesInTriangulation", OBB_NumVerticesInTriangulation_Static_int_int_int, 3}
    ,{"NumVerticesInEdgeList", OBB_NumVerticesInEdgeList_Static, 0}
    ,{nullptr, nullptr, 0}
};

void Expose_OBB(duk_context* ctx)
{
    duk_push_c_function(ctx, OBB_Ctor_Selector, DUK_VARARGS);
    duk_put_function_list(ctx, -1, OBB_StaticFunctions);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, OBB_Functions);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, OBB_Id);
}

}