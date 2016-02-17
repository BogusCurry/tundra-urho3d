// For conditions of distribution and use, see copyright notice in LICENSE
// This file has been autogenerated with BindingsGenerator

#include "StableHeaders.h"
#include "CoreTypes.h"
#include "BindingsHelpers.h"
#include "Geometry/Plane.h"
#include "Geometry/Ray.h"
#include "Geometry/Line.h"
#include "Geometry/LineSegment.h"
#include "Math/float3x3.h"
#include "Math/float3x4.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "Geometry/Triangle.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include "Geometry/Sphere.h"
#include "Geometry/Capsule.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "Geometry/Frustum.h"
#include "Geometry/Circle.h"

using namespace std;

namespace JSBindings
{

extern const char* Ray_Id;
extern const char* Line_Id;
extern const char* LineSegment_Id;
extern const char* float3x3_Id;
extern const char* float3x4_Id;
extern const char* float4x4_Id;
extern const char* Quat_Id;
extern const char* Triangle_Id;
extern const char* float3_Id;
extern const char* float4_Id;
extern const char* Sphere_Id;
extern const char* Capsule_Id;
extern const char* AABB_Id;
extern const char* OBB_Id;
extern const char* Frustum_Id;
extern const char* Circle_Id;

duk_ret_t Ray_Finalizer(duk_context* ctx);
duk_ret_t Line_Finalizer(duk_context* ctx);
duk_ret_t LineSegment_Finalizer(duk_context* ctx);
duk_ret_t float3x3_Finalizer(duk_context* ctx);
duk_ret_t float3x4_Finalizer(duk_context* ctx);
duk_ret_t float4x4_Finalizer(duk_context* ctx);
duk_ret_t Quat_Finalizer(duk_context* ctx);
duk_ret_t Triangle_Finalizer(duk_context* ctx);
duk_ret_t float3_Finalizer(duk_context* ctx);
duk_ret_t float4_Finalizer(duk_context* ctx);
duk_ret_t Sphere_Finalizer(duk_context* ctx);
duk_ret_t Capsule_Finalizer(duk_context* ctx);
duk_ret_t AABB_Finalizer(duk_context* ctx);
duk_ret_t OBB_Finalizer(duk_context* ctx);
duk_ret_t Frustum_Finalizer(duk_context* ctx);
duk_ret_t Circle_Finalizer(duk_context* ctx);

const char* Plane_Id = "Plane";

duk_ret_t Plane_Finalizer(duk_context* ctx)
{
    Plane* obj = GetValueObject<Plane>(ctx, 0, Plane_Id);
    if (obj)
    {
        delete obj;
        SetValueObject(ctx, 0, 0, Plane_Id);
    }
    return 0;
}

static duk_ret_t Plane_Set_d(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float d = (float)duk_require_number(ctx, 0);
    thisObj->d = d;
    return 0;
}

static duk_ret_t Plane_Get_d(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    duk_push_number(ctx, thisObj->d);
    return 1;
}

static duk_ret_t Plane_Ctor(duk_context* ctx)
{
    Plane* newObj = new Plane();
    PushConstructorResult<Plane>(ctx, newObj, Plane_Id, Plane_Finalizer);
    return 0;
}

static duk_ret_t Plane_IsDegenerate(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    bool ret = thisObj->IsDegenerate();
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_ReverseNormal(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    thisObj->ReverseNormal();
    return 0;
}

static duk_ret_t Plane_Transform_float3x3(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float3x3* transform = GetCheckedValueObject<float3x3>(ctx, 0, float3x3_Id);
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t Plane_Transform_float3x4(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float3x4* transform = GetCheckedValueObject<float3x4>(ctx, 0, float3x4_Id);
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t Plane_Transform_float4x4(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float4x4* transform = GetCheckedValueObject<float4x4>(ctx, 0, float4x4_Id);
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t Plane_Transform_Quat(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Quat* transform = GetCheckedValueObject<Quat>(ctx, 0, Quat_Id);
    thisObj->Transform(*transform);
    return 0;
}

static duk_ret_t Plane_ExamineSide_Triangle(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Triangle* triangle = GetCheckedValueObject<Triangle>(ctx, 0, Triangle_Id);
    int ret = thisObj->ExamineSide(*triangle);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Distance_float3(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float3* point = GetCheckedValueObject<float3>(ctx, 0, float3_Id);
    float ret = thisObj->Distance(*point);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Distance_LineSegment(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    LineSegment* lineSegment = GetCheckedValueObject<LineSegment>(ctx, 0, LineSegment_Id);
    float ret = thisObj->Distance(*lineSegment);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Distance_Sphere(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Sphere* sphere = GetCheckedValueObject<Sphere>(ctx, 0, Sphere_Id);
    float ret = thisObj->Distance(*sphere);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Distance_Capsule(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Capsule* capsule = GetCheckedValueObject<Capsule>(ctx, 0, Capsule_Id);
    float ret = thisObj->Distance(*capsule);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_AABB(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    AABB* aabb = GetCheckedValueObject<AABB>(ctx, 0, AABB_Id);
    float ret = thisObj->SignedDistance(*aabb);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_OBB(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    OBB* obb = GetCheckedValueObject<OBB>(ctx, 0, OBB_Id);
    float ret = thisObj->SignedDistance(*obb);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_Capsule(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Capsule* capsule = GetCheckedValueObject<Capsule>(ctx, 0, Capsule_Id);
    float ret = thisObj->SignedDistance(*capsule);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_Frustum(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Frustum* frustum = GetCheckedValueObject<Frustum>(ctx, 0, Frustum_Id);
    float ret = thisObj->SignedDistance(*frustum);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_Line(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Line* line = GetCheckedValueObject<Line>(ctx, 0, Line_Id);
    float ret = thisObj->SignedDistance(*line);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_LineSegment(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    LineSegment* lineSegment = GetCheckedValueObject<LineSegment>(ctx, 0, LineSegment_Id);
    float ret = thisObj->SignedDistance(*lineSegment);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_Ray(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Ray* ray = GetCheckedValueObject<Ray>(ctx, 0, Ray_Id);
    float ret = thisObj->SignedDistance(*ray);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_Sphere(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Sphere* sphere = GetCheckedValueObject<Sphere>(ctx, 0, Sphere_Id);
    float ret = thisObj->SignedDistance(*sphere);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SignedDistance_Triangle(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Triangle* triangle = GetCheckedValueObject<Triangle>(ctx, 0, Triangle_Id);
    float ret = thisObj->SignedDistance(*triangle);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_OrthoProjection(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float3x4 ret = thisObj->OrthoProjection();
    PushValueObjectCopy<float3x4>(ctx, ret, float3x4_Id, float3x4_Finalizer);
    return 1;
}

static duk_ret_t Plane_Project_LineSegment(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    LineSegment* lineSegment = GetCheckedValueObject<LineSegment>(ctx, 0, LineSegment_Id);
    LineSegment ret = thisObj->Project(*lineSegment);
    PushValueObjectCopy<LineSegment>(ctx, ret, LineSegment_Id, LineSegment_Finalizer);
    return 1;
}

static duk_ret_t Plane_Project_Triangle(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Triangle* triangle = GetCheckedValueObject<Triangle>(ctx, 0, Triangle_Id);
    Triangle ret = thisObj->Project(*triangle);
    PushValueObjectCopy<Triangle>(ctx, ret, Triangle_Id, Triangle_Finalizer);
    return 1;
}

static duk_ret_t Plane_MirrorMatrix(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float3x4 ret = thisObj->MirrorMatrix();
    PushValueObjectCopy<float3x4>(ctx, ret, float3x4_Id, float3x4_Finalizer);
    return 1;
}

static duk_ret_t Plane_Contains_Line_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Line* line = GetCheckedValueObject<Line>(ctx, 0, Line_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Contains(*line, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Contains_Ray_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Ray* ray = GetCheckedValueObject<Ray>(ctx, 0, Ray_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Contains(*ray, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Contains_LineSegment_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    LineSegment* lineSegment = GetCheckedValueObject<LineSegment>(ctx, 0, LineSegment_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Contains(*lineSegment, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Contains_Triangle_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Triangle* triangle = GetCheckedValueObject<Triangle>(ctx, 0, Triangle_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Contains(*triangle, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Contains_Circle_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Circle* circle = GetCheckedValueObject<Circle>(ctx, 0, Circle_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Contains(*circle, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_SetEquals_Plane_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Plane* plane = GetCheckedValueObject<Plane>(ctx, 0, Plane_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->SetEquals(*plane, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Equals_Plane_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Plane* other = GetCheckedValueObject<Plane>(ctx, 0, Plane_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->Equals(*other, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_BitEquals_Plane(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Plane* other = GetCheckedValueObject<Plane>(ctx, 0, Plane_Id);
    bool ret = thisObj->BitEquals(*other);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_IsParallel_Plane_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Plane* plane = GetCheckedValueObject<Plane>(ctx, 0, Plane_Id);
    float epsilon = (float)duk_require_number(ctx, 1);
    bool ret = thisObj->IsParallel(*plane, epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_DihedralAngle_Plane(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Plane* plane = GetCheckedValueObject<Plane>(ctx, 0, Plane_Id);
    float ret = thisObj->DihedralAngle(*plane);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_Sphere(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Sphere* sphere = GetCheckedValueObject<Sphere>(ctx, 0, Sphere_Id);
    bool ret = thisObj->Intersects(*sphere);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_AABB(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    AABB* aabb = GetCheckedValueObject<AABB>(ctx, 0, AABB_Id);
    bool ret = thisObj->Intersects(*aabb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_OBB(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    OBB* obb = GetCheckedValueObject<OBB>(ctx, 0, OBB_Id);
    bool ret = thisObj->Intersects(*obb);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_Triangle(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Triangle* triangle = GetCheckedValueObject<Triangle>(ctx, 0, Triangle_Id);
    bool ret = thisObj->Intersects(*triangle);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_Frustum(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Frustum* frustum = GetCheckedValueObject<Frustum>(ctx, 0, Frustum_Id);
    bool ret = thisObj->Intersects(*frustum);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_Capsule(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Capsule* capsule = GetCheckedValueObject<Capsule>(ctx, 0, Capsule_Id);
    bool ret = thisObj->Intersects(*capsule);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Intersects_Circle(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Circle* circle = GetCheckedValueObject<Circle>(ctx, 0, Circle_Id);
    int ret = thisObj->Intersects(*circle);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Clip_LineSegment(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    LineSegment* line = GetCheckedValueObject<LineSegment>(ctx, 0, LineSegment_Id);
    bool ret = thisObj->Clip(*line);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Clip_Line_Ray(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Line* line = GetCheckedValueObject<Line>(ctx, 0, Line_Id);
    Ray* outRay = GetCheckedValueObject<Ray>(ctx, 1, Ray_Id);
    int ret = thisObj->Clip(*line, *outRay);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_Clip_Triangle_Triangle_Triangle(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    Triangle* triangle = GetCheckedValueObject<Triangle>(ctx, 0, Triangle_Id);
    Triangle* t1 = GetCheckedValueObject<Triangle>(ctx, 1, Triangle_Id);
    Triangle* t2 = GetCheckedValueObject<Triangle>(ctx, 2, Triangle_Id);
    int ret = thisObj->Clip(*triangle, *t1, *t2);
    duk_push_number(ctx, ret);
    return 1;
}

static duk_ret_t Plane_PassesThroughOrigin_float(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    float epsilon = (float)duk_require_number(ctx, 0);
    bool ret = thisObj->PassesThroughOrigin(epsilon);
    duk_push_boolean(ctx, ret);
    return 1;
}

static duk_ret_t Plane_ToString(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    std::string ret = thisObj->ToString();
    duk_push_string(ctx, ret.c_str());
    return 1;
}

static duk_ret_t Plane_SerializeToString(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    std::string ret = thisObj->SerializeToString();
    duk_push_string(ctx, ret.c_str());
    return 1;
}

static duk_ret_t Plane_SerializeToCodeString(duk_context* ctx)
{
    Plane* thisObj = GetThisValueObject<Plane>(ctx, Plane_Id);
    std::string ret = thisObj->SerializeToCodeString();
    duk_push_string(ctx, ret.c_str());
    return 1;
}

static duk_ret_t Plane_Transform_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetValueObject<float3x3>(ctx, 0, float3x3_Id))
        return Plane_Transform_float3x3(ctx);
    if (numArgs == 1 && GetValueObject<float3x4>(ctx, 0, float3x4_Id))
        return Plane_Transform_float3x4(ctx);
    if (numArgs == 1 && GetValueObject<float4x4>(ctx, 0, float4x4_Id))
        return Plane_Transform_float4x4(ctx);
    if (numArgs == 1 && GetValueObject<Quat>(ctx, 0, Quat_Id))
        return Plane_Transform_Quat(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_Distance_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetValueObject<float3>(ctx, 0, float3_Id))
        return Plane_Distance_float3(ctx);
    if (numArgs == 1 && GetValueObject<LineSegment>(ctx, 0, LineSegment_Id))
        return Plane_Distance_LineSegment(ctx);
    if (numArgs == 1 && GetValueObject<Sphere>(ctx, 0, Sphere_Id))
        return Plane_Distance_Sphere(ctx);
    if (numArgs == 1 && GetValueObject<Capsule>(ctx, 0, Capsule_Id))
        return Plane_Distance_Capsule(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_SignedDistance_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetValueObject<AABB>(ctx, 0, AABB_Id))
        return Plane_SignedDistance_AABB(ctx);
    if (numArgs == 1 && GetValueObject<OBB>(ctx, 0, OBB_Id))
        return Plane_SignedDistance_OBB(ctx);
    if (numArgs == 1 && GetValueObject<Capsule>(ctx, 0, Capsule_Id))
        return Plane_SignedDistance_Capsule(ctx);
    if (numArgs == 1 && GetValueObject<Frustum>(ctx, 0, Frustum_Id))
        return Plane_SignedDistance_Frustum(ctx);
    if (numArgs == 1 && GetValueObject<Line>(ctx, 0, Line_Id))
        return Plane_SignedDistance_Line(ctx);
    if (numArgs == 1 && GetValueObject<LineSegment>(ctx, 0, LineSegment_Id))
        return Plane_SignedDistance_LineSegment(ctx);
    if (numArgs == 1 && GetValueObject<Ray>(ctx, 0, Ray_Id))
        return Plane_SignedDistance_Ray(ctx);
    if (numArgs == 1 && GetValueObject<Sphere>(ctx, 0, Sphere_Id))
        return Plane_SignedDistance_Sphere(ctx);
    if (numArgs == 1 && GetValueObject<Triangle>(ctx, 0, Triangle_Id))
        return Plane_SignedDistance_Triangle(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_Project_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetValueObject<LineSegment>(ctx, 0, LineSegment_Id))
        return Plane_Project_LineSegment(ctx);
    if (numArgs == 1 && GetValueObject<Triangle>(ctx, 0, Triangle_Id))
        return Plane_Project_Triangle(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_Contains_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 2 && GetValueObject<Line>(ctx, 0, Line_Id) && duk_is_number(ctx, 1))
        return Plane_Contains_Line_float(ctx);
    if (numArgs == 2 && GetValueObject<Ray>(ctx, 0, Ray_Id) && duk_is_number(ctx, 1))
        return Plane_Contains_Ray_float(ctx);
    if (numArgs == 2 && GetValueObject<LineSegment>(ctx, 0, LineSegment_Id) && duk_is_number(ctx, 1))
        return Plane_Contains_LineSegment_float(ctx);
    if (numArgs == 2 && GetValueObject<Triangle>(ctx, 0, Triangle_Id) && duk_is_number(ctx, 1))
        return Plane_Contains_Triangle_float(ctx);
    if (numArgs == 2 && GetValueObject<Circle>(ctx, 0, Circle_Id) && duk_is_number(ctx, 1))
        return Plane_Contains_Circle_float(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_Intersects_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetValueObject<Sphere>(ctx, 0, Sphere_Id))
        return Plane_Intersects_Sphere(ctx);
    if (numArgs == 1 && GetValueObject<AABB>(ctx, 0, AABB_Id))
        return Plane_Intersects_AABB(ctx);
    if (numArgs == 1 && GetValueObject<OBB>(ctx, 0, OBB_Id))
        return Plane_Intersects_OBB(ctx);
    if (numArgs == 1 && GetValueObject<Triangle>(ctx, 0, Triangle_Id))
        return Plane_Intersects_Triangle(ctx);
    if (numArgs == 1 && GetValueObject<Frustum>(ctx, 0, Frustum_Id))
        return Plane_Intersects_Frustum(ctx);
    if (numArgs == 1 && GetValueObject<Capsule>(ctx, 0, Capsule_Id))
        return Plane_Intersects_Capsule(ctx);
    if (numArgs == 1 && GetValueObject<Circle>(ctx, 0, Circle_Id))
        return Plane_Intersects_Circle(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_Clip_Selector(duk_context* ctx)
{
    int numArgs = duk_get_top(ctx);
    if (numArgs == 1 && GetValueObject<LineSegment>(ctx, 0, LineSegment_Id))
        return Plane_Clip_LineSegment(ctx);
    if (numArgs == 2 && GetValueObject<Line>(ctx, 0, Line_Id) && GetValueObject<Ray>(ctx, 1, Ray_Id))
        return Plane_Clip_Line_Ray(ctx);
    if (numArgs == 3 && GetValueObject<Triangle>(ctx, 0, Triangle_Id) && GetValueObject<Triangle>(ctx, 1, Triangle_Id) && GetValueObject<Triangle>(ctx, 2, Triangle_Id))
        return Plane_Clip_Triangle_Triangle_Triangle(ctx);
    duk_error(ctx, DUK_ERR_ERROR, "Could not select function overload");
}

static duk_ret_t Plane_FromString_Static_string(duk_context* ctx)
{
    string str(duk_require_string(ctx, 0));
    Plane ret = Plane::FromString(str);
    PushValueObjectCopy<Plane>(ctx, ret, Plane_Id, Plane_Finalizer);
    return 1;
}

static const duk_function_list_entry Plane_Functions[] = {
    {"IsDegenerate", Plane_IsDegenerate, 0}
    ,{"ReverseNormal", Plane_ReverseNormal, 0}
    ,{"Transform", Plane_Transform_Selector, DUK_VARARGS}
    ,{"ExamineSide", Plane_ExamineSide_Triangle, 1}
    ,{"Distance", Plane_Distance_Selector, DUK_VARARGS}
    ,{"SignedDistance", Plane_SignedDistance_Selector, DUK_VARARGS}
    ,{"OrthoProjection", Plane_OrthoProjection, 0}
    ,{"Project", Plane_Project_Selector, DUK_VARARGS}
    ,{"MirrorMatrix", Plane_MirrorMatrix, 0}
    ,{"Contains", Plane_Contains_Selector, DUK_VARARGS}
    ,{"SetEquals", Plane_SetEquals_Plane_float, 2}
    ,{"Equals", Plane_Equals_Plane_float, 2}
    ,{"BitEquals", Plane_BitEquals_Plane, 1}
    ,{"IsParallel", Plane_IsParallel_Plane_float, 2}
    ,{"DihedralAngle", Plane_DihedralAngle_Plane, 1}
    ,{"Intersects", Plane_Intersects_Selector, DUK_VARARGS}
    ,{"Clip", Plane_Clip_Selector, DUK_VARARGS}
    ,{"PassesThroughOrigin", Plane_PassesThroughOrigin_float, 1}
    ,{"ToString", Plane_ToString, 0}
    ,{"SerializeToString", Plane_SerializeToString, 0}
    ,{"SerializeToCodeString", Plane_SerializeToCodeString, 0}
    ,{nullptr, nullptr, 0}
};

static const duk_function_list_entry Plane_StaticFunctions[] = {
    {"FromString", Plane_FromString_Static_string, 1}
    ,{nullptr, nullptr, 0}
};

void Expose_Plane(duk_context* ctx)
{
    duk_push_c_function(ctx, Plane_Ctor, DUK_VARARGS);
    duk_put_function_list(ctx, -1, Plane_StaticFunctions);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, Plane_Functions);
    DefineProperty(ctx, "d", Plane_Get_d, Plane_Set_d);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, Plane_Id);
}

}
