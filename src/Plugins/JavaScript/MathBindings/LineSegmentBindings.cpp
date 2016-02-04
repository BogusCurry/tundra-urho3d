// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "BindingsHelpers.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Geometry/Line.h"
#include "Math/float3x3.h"
#include "Math/float3x4.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "Geometry/Plane.h"
#include "Geometry/Sphere.h"
#include "Geometry/Capsule.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "Geometry/Frustum.h"
#include "Geometry/Circle.h"

namespace JSBindings
{

extern const char* Ray_Id;
extern const char* Line_Id;
extern const char* float3x3_Id;
extern const char* float3x4_Id;
extern const char* float4x4_Id;
extern const char* Quat_Id;
extern const char* Plane_Id;
extern const char* Sphere_Id;
extern const char* Capsule_Id;
extern const char* AABB_Id;
extern const char* OBB_Id;
extern const char* Frustum_Id;
extern const char* Circle_Id;

duk_ret_t Ray_Dtor(duk_context* ctx);
duk_ret_t Line_Dtor(duk_context* ctx);
duk_ret_t float3x3_Dtor(duk_context* ctx);
duk_ret_t float3x4_Dtor(duk_context* ctx);
duk_ret_t float4x4_Dtor(duk_context* ctx);
duk_ret_t Quat_Dtor(duk_context* ctx);
duk_ret_t Plane_Dtor(duk_context* ctx);
duk_ret_t Sphere_Dtor(duk_context* ctx);
duk_ret_t Capsule_Dtor(duk_context* ctx);
duk_ret_t AABB_Dtor(duk_context* ctx);
duk_ret_t OBB_Dtor(duk_context* ctx);
duk_ret_t Frustum_Dtor(duk_context* ctx);
duk_ret_t Circle_Dtor(duk_context* ctx);

const char* LineSegment_Id = "LineSegment";

duk_ret_t LineSegment_Dtor(duk_context* ctx)
{
    LineSegment* obj = GetObject<LineSegment>(ctx, 0, LineSegment_Id);
    if (obj)
    {
        delete obj;
        SetObject(ctx, 0, 0, LineSegment_Id);
    }
    return 0;
}

static duk_ret_t LineSegment_Ctor(duk_context* ctx)
{
    LineSegment* newObj = new LineSegment();
    duk_push_this(ctx); SetObject(ctx, -1, newObj, LineSegment_Id); duk_push_c_function(ctx, LineSegment_Dtor, 1); duk_set_finalizer(ctx, -2);
    return 0;
}

static duk_ret_t LineSegment_Ctor_Ray_float(duk_context* ctx)
{
    Ray* ray = GetObject<Ray>(ctx, 0, Ray_Id); if (!ray) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    LineSegment* newObj = new LineSegment(*ray, d);
    duk_push_this(ctx); SetObject(ctx, -1, newObj, LineSegment_Id); duk_push_c_function(ctx, LineSegment_Dtor, 1); duk_set_finalizer(ctx, -2);
    return 0;
}

static duk_ret_t LineSegment_Ctor_Line_float(duk_context* ctx)
{
    Line* line = GetObject<Line>(ctx, 0, Line_Id); if (!line) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    LineSegment* newObj = new LineSegment(*line, d);
    duk_push_this(ctx); SetObject(ctx, -1, newObj, LineSegment_Id); duk_push_c_function(ctx, LineSegment_Dtor, 1); duk_set_finalizer(ctx, -2);
    return 0;
}

static duk_ret_t LineSegment_Reverse(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    thisObj->Reverse();
    return 0;
}

static duk_ret_t LineSegment_Transform_float3x3(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float3x3* transform = GetObject<float3x3>(ctx, 0, float3x3_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t LineSegment_Transform_float3x4(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float3x4* transform = GetObject<float3x4>(ctx, 0, float3x4_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t LineSegment_Transform_float4x4(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float4x4* transform = GetObject<float4x4>(ctx, 0, float4x4_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t LineSegment_Transform_Quat(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Quat* transform = GetObject<Quat>(ctx, 0, Quat_Id); if (!transform) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t LineSegment_Length(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float ret = thisObj->Length();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_LengthSq(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    float ret = thisObj->LengthSq();
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_IsFinite(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    bool ret = thisObj->IsFinite();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Equals_LineSegment_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* rhs = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!rhs) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float distanceThreshold = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Equals(*rhs, distanceThreshold);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_BitEquals_LineSegment(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* other = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->BitEquals(*other);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Contains_LineSegment_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* lineSegment = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!lineSegment) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float distanceThreshold = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Contains(*lineSegment, distanceThreshold);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Ray(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Ray* other = GetObject<Ray>(ctx, 0, Ray_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Ray_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Ray* other = GetObject<Ray>(ctx, 0, Ray_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    float ret = thisObj->Distance(*other, d);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Ray_float_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Ray* other = GetObject<Ray>(ctx, 0, Ray_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    float d2 = (float)duk_require_number(ctx, 2);
    float ret = thisObj->Distance(*other, d, d2);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Line(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Line* other = GetObject<Line>(ctx, 0, Line_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Line_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Line* other = GetObject<Line>(ctx, 0, Line_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    float ret = thisObj->Distance(*other, d);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Line_float_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Line* other = GetObject<Line>(ctx, 0, Line_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    float d2 = (float)duk_require_number(ctx, 2);
    float ret = thisObj->Distance(*other, d, d2);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_LineSegment(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* other = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_LineSegment_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* other = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    float ret = thisObj->Distance(*other, d);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_LineSegment_float_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* other = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float d = (float)duk_require_number(ctx, 1);
    float d2 = (float)duk_require_number(ctx, 2);
    float ret = thisObj->Distance(*other, d, d2);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Plane(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Plane* other = GetObject<Plane>(ctx, 0, Plane_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Sphere(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Sphere* other = GetObject<Sphere>(ctx, 0, Sphere_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Distance_Capsule(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Capsule* other = GetObject<Capsule>(ctx, 0, Capsule_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->Distance(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_DistanceSq_LineSegment(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* other = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!other) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float ret = thisObj->DistanceSq(*other);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_Plane(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Plane* plane = GetObject<Plane>(ctx, 0, Plane_Id); if (!plane) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*plane);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_AABB_float_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float dNear = (float)duk_require_number(ctx, 1);
    float dFar = (float)duk_require_number(ctx, 2);
    bool ret = thisObj->Intersects(*aabb, dNear, dFar);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_AABB(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    AABB* aabb = GetObject<AABB>(ctx, 0, AABB_Id); if (!aabb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*aabb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_OBB_float_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    OBB* obb = GetObject<OBB>(ctx, 0, OBB_Id); if (!obb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float dNear = (float)duk_require_number(ctx, 1);
    float dFar = (float)duk_require_number(ctx, 2);
    bool ret = thisObj->Intersects(*obb, dNear, dFar);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_OBB(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    OBB* obb = GetObject<OBB>(ctx, 0, OBB_Id); if (!obb) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*obb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_Capsule(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Capsule* capsule = GetObject<Capsule>(ctx, 0, Capsule_Id); if (!capsule) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*capsule);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_Frustum(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Frustum* frustum = GetObject<Frustum>(ctx, 0, Frustum_Id); if (!frustum) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->Intersects(*frustum);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_Intersects_LineSegment_float(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    LineSegment* lineSegment = GetObject<LineSegment>(ctx, 0, LineSegment_Id); if (!lineSegment) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Intersects(*lineSegment, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_IntersectsDisc_Circle(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Circle* disc = GetObject<Circle>(ctx, 0, Circle_Id); if (!disc) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null or invalid object argument");
    bool ret = thisObj->IntersectsDisc(*disc);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t LineSegment_ToRay(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Ray ret = thisObj->ToRay();
    duk_push_object(ctx); SetObject(ctx, -1, new Ray(ret), Ray_Id); duk_push_c_function(ctx, Ray_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, Ray_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t LineSegment_ToLine(duk_context* ctx)
{
    LineSegment* thisObj = GetThisObject<LineSegment>(ctx, LineSegment_Id); if (!thisObj) duk_error(ctx, DUK_ERR_REFERENCE_ERROR, "Null this pointer");
    Line ret = thisObj->ToLine();
    duk_push_object(ctx); SetObject(ctx, -1, new Line(ret), Line_Id); duk_push_c_function(ctx, Line_Dtor, 1); duk_set_finalizer(ctx, -2); duk_get_global_string(ctx, Line_Id); duk_get_prop_string(ctx, -1, "prototype"); duk_set_prototype(ctx, -3); duk_pop(ctx);
    return 1;
}

static duk_ret_t LineSegment_Ctor_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 0)
        return LineSegment_Ctor(ctx);
    if (numArgs == 2 && GetObject<Ray>(ctx, 0, Ray_Id) && duk_is_number(ctx, 1))
        return LineSegment_Ctor_Ray_float(ctx);
    if (numArgs == 2 && GetObject<Line>(ctx, 0, Line_Id) && duk_is_number(ctx, 1))
        return LineSegment_Ctor_Line_float(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t LineSegment_Transform_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetObject<float3x3>(ctx, 0, float3x3_Id))
        return LineSegment_Transform_float3x3(ctx);
    if (numArgs == 1 && GetObject<float3x4>(ctx, 0, float3x4_Id))
        return LineSegment_Transform_float3x4(ctx);
    if (numArgs == 1 && GetObject<float4x4>(ctx, 0, float4x4_Id))
        return LineSegment_Transform_float4x4(ctx);
    if (numArgs == 1 && GetObject<Quat>(ctx, 0, Quat_Id))
        return LineSegment_Transform_Quat(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t LineSegment_Distance_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetObject<Ray>(ctx, 0, Ray_Id))
        return LineSegment_Distance_Ray(ctx);
    if (numArgs == 2 && GetObject<Ray>(ctx, 0, Ray_Id) && duk_is_number(ctx, 1))
        return LineSegment_Distance_Ray_float(ctx);
    if (numArgs == 3 && GetObject<Ray>(ctx, 0, Ray_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return LineSegment_Distance_Ray_float_float(ctx);
    if (numArgs == 1 && GetObject<Line>(ctx, 0, Line_Id))
        return LineSegment_Distance_Line(ctx);
    if (numArgs == 2 && GetObject<Line>(ctx, 0, Line_Id) && duk_is_number(ctx, 1))
        return LineSegment_Distance_Line_float(ctx);
    if (numArgs == 3 && GetObject<Line>(ctx, 0, Line_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return LineSegment_Distance_Line_float_float(ctx);
    if (numArgs == 1 && GetObject<LineSegment>(ctx, 0, LineSegment_Id))
        return LineSegment_Distance_LineSegment(ctx);
    if (numArgs == 2 && GetObject<LineSegment>(ctx, 0, LineSegment_Id) && duk_is_number(ctx, 1))
        return LineSegment_Distance_LineSegment_float(ctx);
    if (numArgs == 3 && GetObject<LineSegment>(ctx, 0, LineSegment_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return LineSegment_Distance_LineSegment_float_float(ctx);
    if (numArgs == 1 && GetObject<Plane>(ctx, 0, Plane_Id))
        return LineSegment_Distance_Plane(ctx);
    if (numArgs == 1 && GetObject<Sphere>(ctx, 0, Sphere_Id))
        return LineSegment_Distance_Sphere(ctx);
    if (numArgs == 1 && GetObject<Capsule>(ctx, 0, Capsule_Id))
        return LineSegment_Distance_Capsule(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t LineSegment_Intersects_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetObject<Plane>(ctx, 0, Plane_Id))
        return LineSegment_Intersects_Plane(ctx);
    if (numArgs == 3 && GetObject<AABB>(ctx, 0, AABB_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return LineSegment_Intersects_AABB_float_float(ctx);
    if (numArgs == 1 && GetObject<AABB>(ctx, 0, AABB_Id))
        return LineSegment_Intersects_AABB(ctx);
    if (numArgs == 3 && GetObject<OBB>(ctx, 0, OBB_Id) && duk_is_number(ctx, 1) && duk_is_number(ctx, 2))
        return LineSegment_Intersects_OBB_float_float(ctx);
    if (numArgs == 1 && GetObject<OBB>(ctx, 0, OBB_Id))
        return LineSegment_Intersects_OBB(ctx);
    if (numArgs == 1 && GetObject<Capsule>(ctx, 0, Capsule_Id))
        return LineSegment_Intersects_Capsule(ctx);
    if (numArgs == 1 && GetObject<Frustum>(ctx, 0, Frustum_Id))
        return LineSegment_Intersects_Frustum(ctx);
    if (numArgs == 2 && GetObject<LineSegment>(ctx, 0, LineSegment_Id) && duk_is_number(ctx, 1))
        return LineSegment_Intersects_LineSegment_float(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static const duk_function_list_entry LineSegment_Functions[] = {
    {"Reverse", LineSegment_Reverse, 0}
    ,{"Transform", LineSegment_Transform_Selector, DUK_VARARGS}
    ,{"Length", LineSegment_Length, 0}
    ,{"LengthSq", LineSegment_LengthSq, 0}
    ,{"IsFinite", LineSegment_IsFinite, 0}
    ,{"Equals", LineSegment_Equals_LineSegment_float, 2}
    ,{"BitEquals", LineSegment_BitEquals_LineSegment, 1}
    ,{"Contains", LineSegment_Contains_LineSegment_float, 2}
    ,{"Distance", LineSegment_Distance_Selector, DUK_VARARGS}
    ,{"DistanceSq", LineSegment_DistanceSq_LineSegment, 1}
    ,{"Intersects", LineSegment_Intersects_Selector, DUK_VARARGS}
    ,{"IntersectsDisc", LineSegment_IntersectsDisc_Circle, 1}
    ,{"ToRay", LineSegment_ToRay, 0}
    ,{"ToLine", LineSegment_ToLine, 0}
    ,{nullptr, nullptr, 0}
};

void Expose_LineSegment(duk_context* ctx)
{
    duk_push_c_function(ctx, LineSegment_Ctor_Selector, DUK_VARARGS);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, LineSegment_Functions);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, LineSegment_Id);
}

}