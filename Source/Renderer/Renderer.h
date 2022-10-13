/*********************************************************************
 * \file   Renderer.h
 * \brief  Renderer class, entity, shared_ptr, maintained by manager
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDERER_H
#define RENDERER_H

#include "RenderTargetClearingProperties.h"
#include "MathLib/Matrix4.h"
#include "GameEngine/RenderLightingState.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/VisibleSet.h"
#include "SceneGraph/Spatial.h"
#include "RenderPackList.h"
#include "GameEngine/IRenderer.h"
#include <memory>
#include <string>
#include <system_error>
#include <array>

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderTarget;
    class RenderElement;

    class Renderer : public Engine::IRenderer
    {
    public:
        /** enum Render Group ID */
        enum class RenderListID
        {
            Sky = 0,
            Scene,
            Shadow,
            TerrainWater,
            DeferredLighting,
            OffSurface,  // particle, text primitive
            Overlay, // ui
            Count
        };
    public:
        Renderer(const std::string& name);
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        virtual ~Renderer();
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        /** set render target */
        error SetRenderTarget(const std::shared_ptr<RenderTarget>& target);
        /** get render target */
        std::shared_ptr<RenderTarget> GetRenderTarget() const;

        virtual error ChangeClearingProperty(const RenderTargetClearChangingProperty& prop);
        /** clear render target */
        virtual error ClearRenderTarget();
        /** flip */
        error Flip();

        /** insert render element, element 會被加上 active frame bit,
                然後檢查是否已存在序列中, 不在序列中才會加入.
                所以不會有重複加入的問題 */
        virtual error InsertRenderElement(const std::shared_ptr<RenderElement>& element, const MathLib::Matrix4& mxWorld,
            const Engine::RenderLightingState& lighting, RenderListID list_id);
        /** remove render element */
        virtual error RemoveRenderElement(const std::shared_ptr<RenderElement>& element, RenderListID list_id);

        /** begin scene */
        virtual error BeginScene();
        /** begin scene (with camera, frustum parameter)
        @remark 在render thread的狀況下，update frame 與 render frame 可能會有不同的參數，所以要用保留給render frame的參數 */
        virtual error BeginScene(const MathLib::Vector3& camera_loc, const MathLib::Matrix4& mxView, const MathLib::Matrix4& mxProj);
        /** prepare scene
        @param accept_filter insert to render if spatial flag bits is set
        @param reject_filter insert to render if spatial flag bits is not set
        @remark 為什麼要兩個flag? 舉例來說，我們可以接受bit 0,2 但是不能接受bit 1，當一個物件有bit 0,1 時，
        accept filter會接受，而reject filter 會把它排除, 所以最終是排除的
        */
        virtual error PrepareScene(const SceneGraph::VisibleSet& visible_set,
                                   SceneGraph::Spatial::SpatialFlags accept_filter = SceneGraph::Spatial::Spatial_FullOpen,
                                   SceneGraph::Spatial::SpatialFlags reject_filter = SceneGraph::Spatial::Spatial_None);
        /** draw scene */
        virtual error DrawScene();
        /** end scene */
        virtual error EndScene();

        /** flush all group */
        void FlushAll();

        /** associated camera */
        void SetAssociatedCamera(const std::shared_ptr<SceneGraph::Camera>& camera);

        /** set stamp bit mask */
        void SetStampBitMask(unsigned int mask) { m_stampBitMask = mask; };
        /** get stamp bit mask */
        unsigned int GetStampBitMask() const { return m_stampBitMask; };

        /** we need change the sorting setting sometime */
        void EnableSortBeforeDraw(RenderListID list_id, bool flag);

        /** select renderer technique */
        void SelectRendererTechnique(const std::string& techniqueName);

    protected:
        std::weak_ptr<RenderTarget> m_target;

        std::array<RenderPackList, (size_t)RenderListID::Count> m_renderPacksArray;

        std::weak_ptr<SceneGraph::Camera> m_associatedCamera;

        /** renderer bit mask, 每個renderer各用不同的bit, render element用以記錄放在哪個renderer中 */
        unsigned int m_stampBitMask;

        std::string m_rendererTechniqueName;
    };
}

#endif // RENDERER_H
