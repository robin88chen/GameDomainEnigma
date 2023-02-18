/*********************************************************************
 * \file   LazyNodeIOService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _LAZY_NODE_IO_SERVICE_H
#define _LAZY_NODE_IO_SERVICE_H

#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"

namespace Enigma::SceneGraph
{
    class LazyNodeIOService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        LazyNodeIOService(Frameworks::ServiceManager* mngr);
        LazyNodeIOService(const LazyNodeIOService&) = delete;
        LazyNodeIOService(LazyNodeIOService&&) = delete;
        virtual ~LazyNodeIOService() override;
        LazyNodeIOService& operator=(const LazyNodeIOService&) = delete;
        LazyNodeIOService& operator=(LazyNodeIOService&&) = delete;

    private:
        void DoInstancingLazyNode(const Frameworks::ICommandPtr& c);

    private:
        Frameworks::CommandSubscriberPtr m_doInstancingLazyNode;
    };
}

#endif // _LAZY_NODE_IO_SERVICE_H
