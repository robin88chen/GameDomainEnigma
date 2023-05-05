/*********************************************************************
 * \file   InputHandlerService.h
 * \brief  input handler service
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _INPUT_HANDLER_SERVICE_H
#define _INPUT_HANDLER_SERVICE_H

#include "Platforms/PlatformConfig.h"
#include "Frameworks/SystemService.h"
#include "MouseKeyButtons.h"
#include "GestureThreshold.h"
#include <vector>

namespace Enigma::InputHandlers
{
    class InputHandlerService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        InputHandlerService(Frameworks::ServiceManager* mngr);
        InputHandlerService(const InputHandlerService&) = delete;
        InputHandlerService(InputHandlerService&&) = delete;
        virtual ~InputHandlerService() override;
        InputHandlerService& operator=(const InputHandlerService&) = delete;
        InputHandlerService& operator=(InputHandlerService&&) = delete;

        //static Frameworks::ISystemService* Create(Frameworks::ServiceManager* mngr);

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        /// 記錄鍵盤Async Key所要抓取的Key
        void RegisterKeyboardAsyncKey(unsigned int vk);

#if TARGET_PLATFORM == PLATFORM_WIN32
        /** @name process windows message */
        //@{
        void ProcessWinMouseRightButtonDown(bool isDoubleClick, int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseRightButtonUp(int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseLeftButtonDown(bool isDoubleClick, int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseLeftButtonUp(int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseMiddleButtonDown(bool isDoubleClick, int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseMiddleButtonUp(int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseMove(int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinMouseWheel(int x, int y, const MouseKeyFlags& keyFlags);
        void ProcessWinKeyChar(char ch, int repeat);
        void ProcessWinKeyPressDown(unsigned int vk, unsigned int keyFlags);
        void ProcessWinKeyPressUp(unsigned int vk, unsigned int keyFlags);
        void DetectAsyncKeyPress();
        //@}
#endif
#if TARGET_PLATFORM == PLATFORM_ANDROID
        /** @name process android gesture message */
        //@{
        void ProcessGestureSingleTap(float x, float y);
        void ProcessGestureDoubleTap(float x, float y);
        void ProcessGestureShowPress(float x, float y);
        void ProcessGestureLongPress(float x, float y);
        void ProcessGestureScroll(float startX, float startY, float currentX, float currentY,
            float deltaX, float deltaY);
        void ProcessGestureFling(float startX, float startY, float currentX, float currentY,
            float velocityX, float velocityY);
        void ProcessGestureScale(float factor, float focusX, float focusY);
        void ProcessGestureScaleBegin(float spanX, float spanY, float focusX, float focusY);
        void ProcessGestureScaleEnd(float spanX, float spanY, float focusX, float focusY);
        void ProcessGestureShove(float focusX, float focusY, float deltaX, float deltaY);
        void ProcessGestureShoveBegin(float focusX, float focusY);
        void ProcessGestureShoveEnd(float focusX, float focusY);
        //@}
        /** @name gesture threshold */
        //@{
        void SetGestureThreshold(const GestureThreshold& threshold) { m_gestureThreshold = threshold; };
        const GestureThreshold& GetGestureThreshold() { return m_gestureThreshold; };
        //@}
        /** @name process android text editing message */
        //@{
        void ProcessTextEditingBegin(int index);
        void ProcessTextEditingChanged(int index, const std::string& text);
        void ProcessTextEditingEnd(int index, const std::string& text);
        //@}
#endif
    protected:
        struct DragMouseParam
        {
            bool m_isDraging;
            bool m_isDragLeftBtn;
            bool m_isDragRightBtn;
            bool m_isDragMiddleBtn;
            int m_dragBeginX;
            int m_dragBeginY;
        };
    protected:
        std::vector<unsigned int> m_asyncKeyArray;
        DragMouseParam m_dragMouseParam;
        bool m_hasLastAsyncPressed;
        GestureThreshold m_gestureThreshold;
    };
}
#endif // _INPUT_HANDLER_SERVICE_H
