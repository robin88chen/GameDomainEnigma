#include "GraphicThread.h"
#include "Platforms/PlatformLayer.h"
#include <thread>
#include <cassert>

using namespace Enigma::Graphics;

GraphicThread* GraphicThread::m_self = nullptr;
std::atomic_bool GraphicThread::m_isExisting{ false };

GraphicThread::GraphicThread()
{
    m_self = this;
    m_isExisting = false;
    m_thread = new std::thread{ ThreadProcedure };
    Platforms::Debug::Printf("graphic thread id %d\n", m_thread->get_id());
}

GraphicThread::~GraphicThread()
{
    if (m_thread)
    {
        assert(m_thread->joinable());
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }
    m_self = nullptr;
}

std::thread::id GraphicThread::GetThreadId()
{
    if (!m_thread) return std::thread::id{};
    return m_thread->get_id();
}

future_error GraphicThread::PushTask(const std::function<std::error_code()>& task)
{
    std::lock_guard<std::mutex> locker{ m_taskLocker };
    std::packaged_task<std::error_code()> tp{ task };
    future_error f = tp.get_future();
    m_tasks.emplace_back(std::move(tp));
    return f;
}

void GraphicThread::Terminate()
{
    std::lock_guard<std::mutex> locker{ m_taskLocker };
    m_tasks.clear();
    m_isExisting = true;
}

void GraphicThread::ThreadProcedure()
{
    if (!m_self) return;
    while (!m_isExisting)
    {
        // 每次執行一個 task的效率差不多
        /*std::packaged_task<er_code()> t;
        {
            std::lock_guard<std::mutex> locker{ m_self->m_taskLocker };
            if (m_self->m_tasks.empty()) continue;
            t = std::move(m_self->m_tasks.front());
            m_self->m_tasks.pop_front();
        }
        t();*/
        bool is_task_empty = false;
        {
            std::lock_guard<std::mutex> locker{ m_self->m_taskLocker };
            is_task_empty = m_self->m_tasks.empty();
        }
        if (is_task_empty)
        {
            //std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 可以不用 sleep
            continue;
        }
        std::packaged_task<std::error_code()> t;
        {
            std::lock_guard<std::mutex> locker{ m_self->m_taskLocker };
            if (m_self->m_tasks.empty()) continue;
            t = std::move(m_self->m_tasks.front());
            m_self->m_tasks.pop_front();
        }
        t();
    }
}
