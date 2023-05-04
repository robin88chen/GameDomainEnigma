/*********************************************************************
 * \file   GraphicThread.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_THREAD_H
#define GRAPHIC_THREAD_H

#include "Frameworks/ExtentTypesDefine.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <future>

namespace Enigma::Graphics
{
    class GraphicThread
    {
    public:
        GraphicThread();
        GraphicThread(const GraphicThread&) = delete;
        GraphicThread(GraphicThread&&) = delete;
        virtual ~GraphicThread();
        GraphicThread& operator=(const GraphicThread&) = delete;
        GraphicThread& operator=(GraphicThread&&) = delete;

        std::thread::id GetThreadId();
        /**
        @remark PushTask(...).wait(), 直接在函式回傳 wait, 而不要先把回傳值給到 future 變數上,
                這樣在 wait() 回傳時, future 也可以釋放
        */
        future_error PushTask(const std::function<std::error_code()>& task);
        void Terminate();
    protected:
        static void ThreadProcedure();
    protected:
        static GraphicThread* m_self;
        static std::atomic_bool m_isExisting;

        std::thread* m_thread;
        std::mutex m_taskLocker;
        std::list<std::packaged_task<std::error_code()>> m_tasks;
    };
}


#endif // GRAPHIC_THREAD_H
