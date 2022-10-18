#include "RenderPackList.h"
#include "RenderElement.h"
#include "RendererErrors.h"
#include "GameEngine/EffectMaterial.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Renderer;

bool compare_render_pack(const RenderPack& first, const RenderPack& second)
{
    // 排序的概念是，同樣的buffer通常有同樣的effect，但一個effect常常用在不同的buffer上，
    // 所以，先比對shader再比對buffer
    // 比對shader
    if (static_cast<void*>(first.GetRenderElement()->GetEffectMaterial()->GetEffectMaterialSource().get()) <
        static_cast<void*>(second.GetRenderElement()->GetEffectMaterial()->GetEffectMaterialSource().get())) return true;
    if (static_cast<void*>(first.GetRenderElement()->GetEffectMaterial()->GetEffectMaterialSource().get()) >
        static_cast<void*>(second.GetRenderElement()->GetEffectMaterial()->GetEffectMaterialSource().get())) return false;
    // 比對 render buffer
    if (static_cast<void*>(first.GetRenderElement()->GetRenderBuffer().get()) <
        static_cast<void*>(second.GetRenderElement()->GetRenderBuffer().get())) return true;
    if (static_cast<void*>(first.GetRenderElement()->GetRenderBuffer().get()) >
        static_cast<void*>(second.GetRenderElement()->GetRenderBuffer().get())) return false;
    //
    //if ((void*)(first->GetRenderBuffer()->GetVertexDeclaration())<(void*)(second->GetRenderBuffer()->GetVertexDeclaration())) return true;
    return false;
}

bool compare_render_pack_by_distance(const RenderPack& first, const RenderPack& second)
{
    // 這個是alpha物件需要的排序 (遠的排前面)
    if (first.GetSquareCameraDistance() > second.GetSquareCameraDistance()) return true;
    return false;
}

RenderPackList::RenderPackList()
{
    m_isListModified = false;
    m_isSortBeforeDraw = false;
    m_compareFunc = compare_render_pack;
}

RenderPackList::~RenderPackList()
{
    ClearList();
}

error RenderPackList::InsertRenderElement(const std::shared_ptr<RenderElement>& element, const MathLib::Matrix4& mxWorld,
    const Engine::RenderLightingState& lighting_state, unsigned renderer_bit)
{
    if (!element) return ErrorCode::nullRenderElement;
    element->AddActiveFrameFlag(renderer_bit);
    if (element->GetRendererStamp() & renderer_bit)  // this element already in renderer
    {
        auto iter = std::find_if(m_packs.begin(), m_packs.end(),
            [=](const RenderPack& p) -> bool { return p.GetRenderElement() == element; });
        if (iter != m_packs.end())
        {
            (*iter).SetWorldTransform(mxWorld);
            (*iter).SetRenderLightingState(lighting_state);
        }
    }
    else
    {
        element->AddRendererStamp(renderer_bit);
        m_packs.emplace_back(RenderPack{ element, mxWorld, lighting_state });
    }
    return ErrorCode::ok;
}

error RenderPackList::InsertRenderElement(const std::shared_ptr<RenderElement>& element, const MathLib::Matrix4& mxWorld,
    const MathLib::Vector3& camera_loc, const Engine::RenderLightingState& lighting_state, unsigned renderer_bit)
{
    if (!element) return ErrorCode::nullRenderElement;
    element->AddActiveFrameFlag(renderer_bit);
    if (element->GetRendererStamp() & renderer_bit)  // this element already in renderer
    {
        auto iter = std::find_if(m_packs.begin(), m_packs.end(),
            [=](const RenderPack& p) -> bool { return p.GetRenderElement() == element; });
        if (iter != m_packs.end())
        {
            (*iter).SetWorldTransform(mxWorld);
            (*iter).SetRenderLightingState(lighting_state);
            (*iter).CalcSquareCameraDistance(camera_loc);
        }
    }
    else
    {
        element->AddRendererStamp(renderer_bit);
        auto iter = m_packs.emplace(m_packs.end(), RenderPack{ element, mxWorld, lighting_state });
        if (iter != m_packs.end())
        {
            (*iter).CalcSquareCameraDistance(camera_loc);
        }
    }
    return ErrorCode::ok;
}

error RenderPackList::RemoveRenderElement(const std::shared_ptr<RenderElement>& element, unsigned renderer_bit)
{
    if (!element) return ErrorCode::nullRenderElement;
    element->RemoveRenderStamp(renderer_bit);
    m_packs.remove_if([=](const RenderPack& p) -> bool { return p.GetRenderElement() == element; });
    return ErrorCode::ok;
}

/***************************
* Draw
* 這裡有四個Draw, 主要程序相同,
* 差異在out of date element的處理
* 考慮到在loop內一直檢查條件的效能損失
* 所以做了四個幾乎相同的func.
***************************/
error RenderPackList::Draw(unsigned stamp_mask, const std::string& rendererTechnique)
{
    if (m_packs.size())
    {
        //DebugPrintf("draw %d s render list size %d\n", (int)this, m_stationaryElementList.size());
        if (m_isListModified)
        {
            if (m_isSortBeforeDraw) m_packs.sort(m_compareFunc);
            m_isListModified = false;
        }
        PackList::iterator iter = m_packs.begin();
        while (iter != m_packs.end())
        {
            if (!iter->GetRenderElement())
            {
                ++iter;
                continue;
            }
            if (!(iter->GetRenderElement()->GetActiveFrameFlag() & stamp_mask))  // element is out of date
            {
                // remove from element list
                iter->GetRenderElement()->RemoveRenderStamp(stamp_mask);
                iter = m_packs.erase(iter);  // remove and go to next
                continue;
            }
            iter->GetRenderElement()->RemoveActiveFrameFlag(stamp_mask);  // mark this element as out of date
            error er_draw = iter->GetRenderElement()->Draw(iter->GetWorldTransform(), iter->GetRenderLightingState(), rendererTechnique);
            LOG_IF(Error, er_draw.value() != 0);
            ++iter;
        }
    }
    return ErrorCode::ok;
}

// draw with remove out of date element, 把過期的刪掉，但是不把現有的加上過期記號
error RenderPackList::DrawWithRemoveDated(unsigned stamp_mask, const std::string& rendererTechnique)
{
    if (m_packs.size())
    {
        //DebugPrintf("draw %d s render list size %d\n", (int)this, m_stationaryElementList.size());
        if (m_isListModified)
        {
            if (m_isSortBeforeDraw) m_packs.sort(m_compareFunc);
            m_isListModified = false;
        }
        PackList::iterator iter = m_packs.begin();
        while (iter != m_packs.end())
        {
            if (!iter->GetRenderElement())
            {
                ++iter;
                continue;
            }
            if (!(iter->GetRenderElement()->GetActiveFrameFlag() & stamp_mask))  // element is out of date
            {
                // remove from element list
                iter->GetRenderElement()->RemoveRenderStamp(stamp_mask);
                iter = m_packs.erase(iter);  // remove and go to next
                continue;
            }
            // 不設定為過期
            //iter->GetRenderElement()->RemoveActiveFrameFlag(stamp_mask);  // mark this element as out of date
            error er_draw = iter->GetRenderElement()->Draw(iter->GetWorldTransform(), iter->GetRenderLightingState(), rendererTechnique);
            LOG_IF(Error, er_draw.value() != 0);
            ++iter;
        }
    }
    return ErrorCode::ok;
}

// draw only native, 不做過期element檢查, 也不把現有的element做記號
error RenderPackList::DrawOnlyNative(unsigned stamp_mask, const std::string& rendererTechnique)
{
    if (m_packs.size())
    {
        //DebugPrintf("draw %d s render list size %d\n", (int)this, m_stationaryElementList.size());
        if (m_isListModified)
        {
            if (m_isSortBeforeDraw) m_packs.sort(m_compareFunc);
            m_isListModified = false;
        }
        PackList::iterator iter = m_packs.begin();
        while (iter != m_packs.end())
        {
            if (!iter->GetRenderElement())
            {
                ++iter;
                continue;
            }
            error er_draw = iter->GetRenderElement()->Draw(iter->GetWorldTransform(), iter->GetRenderLightingState(), rendererTechnique);
            LOG_IF(Error, er_draw.value() != 0);
            ++iter;
        }
    }
    return ErrorCode::ok;
}

// draw with mark dated, 把每個element draw, 同時加上過期記號, 但是draw之前不檢查
error RenderPackList::DrawWithMarkDated(unsigned stamp_mask, const std::string& rendererTechnique)
{
    if (m_packs.size())
    {
        //DebugPrintf("draw %d s render list size %d\n", (int)this, m_stationaryElementList.size());
        if (m_isListModified)
        {
            if (m_isSortBeforeDraw) m_packs.sort(m_compareFunc);
            m_isListModified = false;
        }
        PackList::iterator iter = m_packs.begin();
        while (iter != m_packs.end())
        {
            if (!iter->GetRenderElement())
            {
                ++iter;
                continue;
            }
            iter->GetRenderElement()->RemoveActiveFrameFlag(stamp_mask);  // mark this element as out of date
            error er_draw = iter->GetRenderElement()->Draw(iter->GetWorldTransform(), iter->GetRenderLightingState(), rendererTechnique);
            LOG_IF(Error, er_draw.value() != 0);
            ++iter;
        }
    }
    return ErrorCode::ok;
}

void RenderPackList::FlushAll(unsigned stamp_mask)
{
    if (m_packs.size())
    {
        for (PackList::iterator iter = m_packs.begin(); iter != m_packs.end(); ++iter)
        {
            if (iter->GetRenderElement()) iter->GetRenderElement()->RemoveRenderStamp(stamp_mask);
        }
    }
    m_packs.clear();
}

void RenderPackList::SortByDistance()
{
    m_compareFunc = compare_render_pack_by_distance;
}

void RenderPackList::ClearList()
{
    FlushAll(0);
}
