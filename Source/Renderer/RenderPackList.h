/*********************************************************************
 * \file   RenderPackList.h
 * \brief  Render Pack List, value object list
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 * Draw 程序的說明 :
*       一般的 Draw 程序，用在一般的渲染流程，(清除已經無效的，渲染過後改為無效)
*       另外三個 Draw 程序，在 Cascade Shadow Map 的使用中，
*       同樣的場景要渲染至好幾個 Shadow Map 上，這些element 要重複使用
*       所以在第一個 shadow map 時，使用 DrawWithRemoveDated (移除過期但不加記號)
*       最後一個使用 DrawWithMarkDated (不清除但加過期記號)
*       中間的單純只用 DrawOnlyNative
*********************************************************************/
#ifndef RENDER_PACK_LIST_H
#define RENDER_PACK_LIST_H

#include "MathLib/Matrix4.h"
#include "RenderPack.h"
#include <memory>
#include <system_error>
#include <list>
#include <functional>

namespace Enigma::Engine
{
    class RenderLightingState;
}
namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderElement;

    class RenderPackList
    {
    public:
        RenderPackList();
        RenderPackList(const RenderPackList&) = default;
        RenderPackList(RenderPackList&&) = default;
        ~RenderPackList();
        RenderPackList& operator=(const RenderPackList&) = default;
        RenderPackList& operator=(RenderPackList&&) = default;

        error insertRenderElement(const std::shared_ptr<RenderElement>& element, const MathLib::Matrix4& mxWorld,
            const Engine::RenderLightingState& lighting_state, unsigned int renderer_bit);
        error insertRenderElement(const std::shared_ptr<RenderElement>& element, const MathLib::Matrix4& mxWorld,
            const MathLib::Vector3& camera_loc, const Engine::RenderLightingState& lighting_state, unsigned int renderer_bit);
        error removeRenderElement(const std::shared_ptr<RenderElement>& element, unsigned int renderer_bit);

        /** @name drawing procedure */
        //@{
        /** 一般正常的程序, 包含 清除舊的element,draw,並做過期記號 */
        error draw(unsigned int stamp_mask, const std::string& rendererTechnique);
        /** 有清除舊的element, 但不做過期記號的draw程序 */
        error drawWithRemoveDated(unsigned int stamp_mask, const std::string& rendererTechnique);
        /** 不清除也不做記號的draw程序 */
        error drawOnlyNative(unsigned int stamp_mask, const std::string& rendererTechnique);
        /** 不清除舊 element, 並同時做過期記號的draw程序 */
        error drawWithMarkDated(unsigned int stamp_mask, const std::string& rendererTechnique);
        //@}

        inline bool hasElements() const { return (!m_packs.empty()); };

        void flushAll(unsigned int stamp_mask);

        void enableSortBeforeDraw(bool flag) { m_isSortBeforeDraw = flag; };
        void sortByDistance();
    private:
        void clearList();

    private:
        typedef std::list<RenderPack> PackList;

        typedef std::function<bool(const RenderPack&, const RenderPack&)> PackCompareFunc;
        PackCompareFunc m_compareFunc;

        bool m_isListModified;
        PackList m_packs;

        bool m_isSortBeforeDraw;  ///< default is true
    };
}

#endif // RENDER_PACK_LIST_H
