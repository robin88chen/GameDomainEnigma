/*********************************************************************
 * \file   MaterialVariableMap.h
 * \brief  material variable map, static object & functions
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef MATERIAL_VARIABLE_MAP_H
#define MATERIAL_VARIABLE_MAP_H

#include <functional>
#include <unordered_map>
#include "MathLib/Matrix4.h"
#include "MathLib/ColorRGBA.h"
#include "EffectVariable.h"

namespace Enigma::Graphics
{
    class TargetViewPort;
};

namespace Enigma::Engine
{
    size_t constexpr MAX_POINT_LIGHT = 8;

    /** Material Variable Map */
    class MaterialVariableMap
    {
    public:
        MaterialVariableMap();
        MaterialVariableMap(const MaterialVariableMap&) = delete;
        MaterialVariableMap(MaterialVariableMap&&) = delete;
        ~MaterialVariableMap();
        MaterialVariableMap& operator=(const MaterialVariableMap&) = delete;
        MaterialVariableMap& operator=(MaterialVariableMap&&) = delete;

        static MaterialVariableMap* Instance();

        /** insert variable function to map */
        static void InsertAutoVariableFunctionToMap(const std::string& semantic, EffectVariable::VariableValueAssignFunc fn);

        /** set variable assign function */
        static void SetAutoVariableCommitFunction(EffectVariable& var);

        /** @name variable assign functions */
        //@{
        static void AssignCameraViewTransform(EffectVariable& var);
        static void AssignCameraViewInverseTransform(EffectVariable& var);
        static void AssignCameraProjectionTransform(EffectVariable& var);
        static void AssignCameraPosition(EffectVariable& var);
        static void AssignWorldTransform(EffectVariable& var);
        static void AssignWorldInverseTransform(EffectVariable& var);
        static void AssignFaceToCameraWorldTransform(EffectVariable& var);
        static void AssignCameraViewProjectionTransform(EffectVariable& var);
        static void AssignWorldViewProjectionTransform(EffectVariable& var);
        static void AssignWorldViewProjectionInverseTransform(EffectVariable& var);
        static void AssignFrustumTangentFov(EffectVariable& var);
        /*static void AssignEffectTexture(EffectVariable& var);
        static void AssignBoneAnimationMatrix(EffectVariable& var);*/
        static void AssignAmbientLightColor(EffectVariable& var);
        static void AssignSunLightColor(EffectVariable& var);
        static void AssignSunLightDirection(EffectVariable& var);
        /*static void AssignDepthWriteFlag(EffectVariable& var);
        static void AssignTextureAnimOffsetU(EffectVariable& var);
        static void AssignTextureAnimOffsetV(EffectVariable& var);
        static void AssignClipPlane(EffectVariable& var);
        static void AssignDepthBias(EffectVariable& var);
        static void AssignUseNormalMap(EffectVariable& var);
        static void AssignSpecularPower(EffectVariable& var);
        static void AssignSpecularLighting(EffectVariable& var);
        static void AssignUseSpecularMap(EffectVariable& var);*/
        static void AssignPointLightPosition(EffectVariable& var);
        static void AssignPointLightColor(EffectVariable& var);
        static void AssignPointLightAttenuation(EffectVariable& var);
        static void AssignPointLightCount(EffectVariable& var);
        //static void AssignRasterizerState(EffectVariable& var);*/
        static void AssignViewPortDimension(EffectVariable& var);
        //@}

        /** @name set parameter source object */
        //@{
        static void UseCameraParameter(const MathLib::Vector3& pos, const MathLib::Matrix4& view, const MathLib::Matrix4& proj);
        static void UseWorldTransform(const MathLib::Matrix4& mxWorld);
        static void UseAmbientLightColor(const MathLib::ColorRGBA& ambient);
        static void UseSunLight(const MathLib::ColorRGBA& color, const MathLib::Vector3& dir);
        /*static void UseClipPlane(const Plane3& kPlane);
        static void UseDepthBias(float fDepthBias);*/
        /// pos_range : pos (x,y,z), w: light range
        static void UsePointLight(MathLib::Vector4* pos_range, MathLib::ColorRGBA* color, MathLib::Vector4* attenuation, unsigned int count);
        static void UseViewPortDimension(const Graphics::TargetViewPort& vp);
        static void UseViewPortDimension(unsigned int width, unsigned int height, float min_z, float max_z);
        //@}

    private:
        void InitializeFunctionMap();
        void CalculateFaceToCameraWorldTransform();

    private:
        static MaterialVariableMap* m_instance;

        MathLib::Vector4 m_vecCameraPos;
        MathLib::Matrix4 m_mxViewTransform;
        MathLib::Matrix4 m_mxViewInvTransform;
        MathLib::Matrix4 m_mxProjTransform;
        MathLib::Matrix4 m_mxProjInvTransform;
        MathLib::Matrix4 m_mxWorldTransform;
        MathLib::Matrix4 m_mxWorldInvTransform;
        MathLib::Vector4 m_vecTangentFov;  //< tan(fovx) & tan(fovy), projection 與 view space 轉換計算要用
        MathLib::Matrix4 m_mxFaceToCameraWorldTransform;  //< Face to Camera world transform

        //float m_fDepthBias;

        MathLib::ColorRGBA m_colorAmbientLight;

        MathLib::ColorRGBA m_colorSunLight;
        MathLib::Vector4    m_vecSunLightDir;

        MathLib::Vector4 m_vecPointLightPos[MAX_POINT_LIGHT]; ///< pos:(x,y,z), light range : w
        MathLib::Vector4 m_vecPointLightAttenuation[MAX_POINT_LIGHT];
        MathLib::Vector4 m_vecPointLightColorInVector4[MAX_POINT_LIGHT];
        unsigned int m_countPointLightInfo;

        //Plane3 m_kClipPlane;

        MathLib::Vector4 m_vecViewPortDimension;  ///< (width, height, min_z, max_z)

        typedef std::unordered_map<std::string, EffectVariable::VariableValueAssignFunc> VariableAssignFuncMap;
        VariableAssignFuncMap m_assignFuncMap;
    };
}


#endif // MATERIAL_VARIABLE_MAP_H
