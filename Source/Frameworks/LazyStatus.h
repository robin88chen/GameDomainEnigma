/*********************************************************************
 * \file   LazyStatus.h
 * \brief  Lazy Loading 的狀態標示
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _LAZY_STATUS_H
#define _LAZY_STATUS_H

#include <atomic>

namespace Enigma::Frameworks
{
    class LazyStatus
    {
    public:
        enum class Status
        {
            Ghost,
            InQueue,
            Loading,
            Ready,
            Failed,
        };
    public:
        LazyStatus();

        bool isReady() const;
        bool isGhost() const;
        bool isInQueue() const;
        bool isLoading() const;
        bool isFailed() const;
        void changeStatus(Status s);

    private:
        std::atomic<Status> m_status;
    };
}

#endif // _LAZY_STATUS_H
