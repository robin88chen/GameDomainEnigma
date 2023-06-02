#include "MaterialVariableMap.h"
#include "GraphicKernel/TargetViewPort.h"
#include "MathLib/Matrix3.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

constexpr const char* SEMANTIC_VIEW = "View";
constexpr const char* SEMANTIC_VIEW_INVERSE = "ViewInv";
constexpr const char* SEMANTIC_PROJECTION = "Projection";
constexpr const char* SEMANTIC_CAMERA_POSITION = "CameraPosition";
constexpr const char* SEMANTIC_WORLD = "World";
constexpr const char* SEMANTIC_WORLD_INVERSE = "WorldInv";
constexpr const char* SEMANTIC_FACE_TO_CAMERA_WORLD = "FaceToCameraWorld";
constexpr const char* SEMANTIC_VIEW_PROJECTION = "ViewProjection";
constexpr const char* SEMANTIC_WORLD_VIEW_PROJECTION = "WorldViewProjection";
constexpr const char* SEMANTIC_WORLD_VIEW_PROJECTION_INVERSE = "WorldViewProjectionInv";
constexpr const char* SEMANTIC_WORLD_VIEW_INVERSE_TRANSPOSE = "WorldViewInvTranspose";
constexpr const char* SEMANTIC_TANGENT_FOV = "TangentFov";
constexpr const char* SEMANTIC_AMBIENT_LIGHT = "AmbientLight";
constexpr const char* SEMANTIC_SUN_LIGHT_COLOR = "SunLightColor";
constexpr const char* SEMANTIC_SUN_LIGHT_DIRECTION = "SunLightDirection";
constexpr const char* SEMANTIC_VIEW_PORT_DIMENSION = "ViewPortDimension";
constexpr const char* SEMANTIC_POINT_LIGHT_POSITION = "PointLightPosition";
constexpr const char* SEMANTIC_POINT_LIGHT_COLOR = "PointLightColor";
constexpr const char* SEMANTIC_POINT_LIGHT_ATTENUATION = "PointLightAttenuation";
constexpr const char* SEMANTIC_POINT_LIGHT_COUNT = "PointLightCount";

MaterialVariableMap* MaterialVariableMap::m_instance = nullptr;

MaterialVariableMap::MaterialVariableMap() : m_countPointLightInfo(0)
{
    m_instance = this;
    InitializeFunctionMap();
}

MaterialVariableMap::~MaterialVariableMap()
{
    m_instance = nullptr;
}

MaterialVariableMap* MaterialVariableMap::Instance()
{
    assert(m_instance);
    return m_instance;
}

void MaterialVariableMap::InsertAutoVariableFunctionToMap(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn)
{
    assert(m_instance);
    m_instance->m_assignFuncMap.insert_or_assign(semantic, fn);
}

void MaterialVariableMap::SetAutoVariableCommitFunction(EffectVariable& var)
{
    assert(m_instance);
    auto iter = m_instance->m_assignFuncMap.find(var.GetSemantic());
    if (iter != m_instance->m_assignFuncMap.end())  // found
    {
        var.SetValueAssignFunction(iter->second);
    }
}

void MaterialVariableMap::AssignCameraViewTransform(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_mxViewTransform);
}

void MaterialVariableMap::AssignCameraViewInverseTransform(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_mxViewInvTransform);
}

void MaterialVariableMap::AssignCameraProjectionTransform(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_mxProjTransform);
}

void MaterialVariableMap::AssignCameraPosition(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_vecCameraPos);
}

void MaterialVariableMap::AssignWorldTransform(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_mxWorldTransform);
}

void MaterialVariableMap::AssignWorldInverseTransform(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_mxWorldInvTransform);
}

void MaterialVariableMap::AssignFaceToCameraWorldTransform(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_mxFaceToCameraWorldTransform);
}

void MaterialVariableMap::AssignCameraViewProjectionTransform(EffectVariable& var)
{
    assert(m_instance);
    Matrix4 vp = m_instance->m_mxProjTransform * m_instance->m_mxViewTransform;
    var.AssignValue(vp);
}

void MaterialVariableMap::AssignWorldViewProjectionTransform(EffectVariable& var)
{
    assert(m_instance);
    Matrix4 wvp = m_instance->m_mxProjTransform * m_instance->m_mxViewTransform * m_instance->m_mxWorldTransform;
    var.AssignValue(wvp);
}

void MaterialVariableMap::AssignWorldViewProjectionInverseTransform(EffectVariable& var)
{
    assert(m_instance);
    Matrix4 wvpInv = m_instance->m_mxWorldInvTransform * m_instance->m_mxViewInvTransform * m_instance->
        m_mxProjInvTransform;
    var.AssignValue(wvpInv);
}

void MaterialVariableMap::AssignWorldViewInverseTransposeMatrix(EffectVariable& var)
{
    assert(m_instance);
    Matrix4 wvInvTrans = m_instance->m_mxWorldInvTransform * m_instance->m_mxViewInvTransform;
    wvInvTrans = wvInvTrans.Transpose();
    var.AssignValue(wvInvTrans);
}

void MaterialVariableMap::AssignFrustumTangentFov(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_vecTangentFov);
}

void MaterialVariableMap::AssignAmbientLightColor(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(Vector4((float*)m_instance->m_colorAmbientLight));
}

void MaterialVariableMap::AssignSunLightColor(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(Vector4((float*)m_instance->m_colorSunLight));
}

void MaterialVariableMap::AssignSunLightDirection(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_vecSunLightDir);
}

void MaterialVariableMap::AssignPointLightPosition(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValues(m_instance->m_vecPointLightPos, m_instance->m_countPointLightInfo);
}

void MaterialVariableMap::AssignPointLightColor(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValues(m_instance->m_vecPointLightColorInVector4, m_instance->m_countPointLightInfo);
}

void MaterialVariableMap::AssignPointLightAttenuation(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValues(m_instance->m_vecPointLightAttenuation, m_instance->m_countPointLightInfo);
}

void MaterialVariableMap::AssignPointLightCount(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(static_cast<int>(m_instance->m_countPointLightInfo));
}

void MaterialVariableMap::AssignViewPortDimension(EffectVariable& var)
{
    assert(m_instance);
    var.AssignValue(m_instance->m_vecViewPortDimension);
}
void MaterialVariableMap::UseCameraParameter(const Vector3& pos, const Matrix4& view, const Matrix4& proj)
{
    assert(m_instance);
    m_instance->m_vecCameraPos = Vector4(pos.X(), pos.Y(), pos.Z(), 1.0f);
    m_instance->m_mxViewTransform = view;
    m_instance->m_mxProjTransform = proj;
    m_instance->m_vecTangentFov.X() = 1.0f / proj[0][0];
    m_instance->m_vecTangentFov.Y() = 1.0f / proj[1][1];
    m_instance->m_mxViewInvTransform = view.Inverse();
    m_instance->m_mxProjInvTransform = proj.Inverse();
    m_instance->CalculateFaceToCameraWorldTransform();
}

void MaterialVariableMap::UseWorldTransform(const Matrix4& mxWorld)
{
    assert(m_instance);
    m_instance->m_mxWorldTransform = mxWorld;
    m_instance->CalculateFaceToCameraWorldTransform();
    m_instance->m_mxWorldInvTransform = mxWorld.Inverse();
}
void MaterialVariableMap::UseAmbientLightColor(const ColorRGBA& ambient)
{
    assert(m_instance);
    m_instance->m_colorAmbientLight = ambient;
}

void MaterialVariableMap::UseSunLight(const ColorRGBA& color, const Vector3& dir)
{
    assert(m_instance);
    m_instance->m_colorSunLight = color;
    m_instance->m_vecSunLightDir = Vector4(-dir.X(), -dir.Y(), -dir.Z(), 1.0f);  // shader裡面使用反方向
}

void MaterialVariableMap::UsePointLight(Vector4* pos_range, ColorRGBA* color, Vector4* attenuation, unsigned int count)
{
    assert(m_instance);

    unsigned int info_count = count;
    if (info_count > MAX_POINT_LIGHT) info_count = MAX_POINT_LIGHT;
    if (pos_range)
    {
        memcpy(m_instance->m_vecPointLightPos, pos_range, info_count * sizeof(Vector4));
    }
    if (color)
    {
        for (unsigned int i = 0; i < info_count; i++)
        {
            memcpy(&(m_instance->m_vecPointLightColorInVector4[i]), (float*)color[i], sizeof(Vector4));
        }
    }
    if (attenuation)
    {
        memcpy(m_instance->m_vecPointLightAttenuation, attenuation, info_count * sizeof(Vector4));
    }
    m_instance->m_countPointLightInfo = info_count;
}
void MaterialVariableMap::UseViewPortDimension(const Enigma::Graphics::TargetViewPort& vp)
{
    UseViewPortDimension(vp.Width(), vp.Height(), vp.MinZ(), vp.MaxZ());
}

void MaterialVariableMap::UseViewPortDimension(unsigned int width, unsigned int height, float min_z, float max_z)
{
    assert(m_instance);
    m_instance->m_vecViewPortDimension = Vector4(static_cast<float>(width), static_cast<float>(height), min_z, max_z);
}

void MaterialVariableMap::InitializeFunctionMap()
{
    m_assignFuncMap.emplace(SEMANTIC_VIEW, AssignCameraViewTransform);
    m_assignFuncMap.emplace(SEMANTIC_VIEW_INVERSE, AssignCameraViewInverseTransform);
    m_assignFuncMap.emplace(SEMANTIC_PROJECTION, AssignCameraProjectionTransform);
    m_assignFuncMap.emplace(SEMANTIC_CAMERA_POSITION, AssignCameraPosition);
    m_assignFuncMap.emplace(SEMANTIC_WORLD, AssignWorldTransform);
    m_assignFuncMap.emplace(SEMANTIC_WORLD_INVERSE, AssignWorldInverseTransform);
    m_assignFuncMap.emplace(SEMANTIC_FACE_TO_CAMERA_WORLD, AssignFaceToCameraWorldTransform);
    m_assignFuncMap.emplace(SEMANTIC_VIEW_PROJECTION, AssignCameraViewProjectionTransform);
    m_assignFuncMap.emplace(SEMANTIC_WORLD_VIEW_PROJECTION, AssignWorldViewProjectionTransform);
    m_assignFuncMap.emplace(SEMANTIC_WORLD_VIEW_PROJECTION_INVERSE, AssignWorldViewProjectionInverseTransform);
    m_assignFuncMap.emplace(SEMANTIC_WORLD_VIEW_INVERSE_TRANSPOSE, AssignWorldViewInverseTransposeMatrix);
    m_assignFuncMap.emplace(SEMANTIC_TANGENT_FOV, AssignFrustumTangentFov);
    m_assignFuncMap.emplace(SEMANTIC_VIEW_PORT_DIMENSION, AssignViewPortDimension);
    m_assignFuncMap.emplace(SEMANTIC_AMBIENT_LIGHT, AssignAmbientLightColor);
    m_assignFuncMap.emplace(SEMANTIC_SUN_LIGHT_COLOR, AssignSunLightColor);
    m_assignFuncMap.emplace(SEMANTIC_SUN_LIGHT_DIRECTION, AssignSunLightDirection);
    m_assignFuncMap.emplace(SEMANTIC_POINT_LIGHT_POSITION, AssignPointLightPosition);
    m_assignFuncMap.emplace(SEMANTIC_POINT_LIGHT_COLOR, AssignPointLightColor);
    m_assignFuncMap.emplace(SEMANTIC_POINT_LIGHT_ATTENUATION, AssignPointLightAttenuation);
    m_assignFuncMap.emplace(SEMANTIC_POINT_LIGHT_COUNT, AssignPointLightCount);
}

void MaterialVariableMap::CalculateFaceToCameraWorldTransform()
{
    Vector3 scale;
    Vector3 trans;
    std::tie(scale, std::ignore, trans) = m_instance->m_mxWorldTransform.UnMatrixSRT_WithRotateMatrix();
    Matrix3 mxCamera(m_instance->m_mxViewTransform);
    Matrix3 mxFaceToCamera(mxCamera.GetRow(0), mxCamera.GetRow(1), -(mxCamera.GetRow(2)), true);
    m_instance->m_mxFaceToCameraWorldTransform = Matrix4::FromSRT(scale, mxFaceToCamera, trans);
}
