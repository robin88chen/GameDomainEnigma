/*********************************************************************
 * \file   LinkageResolver.h
 * \brief  Contracted Factory linkage resolver
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _LINKAGE_RESOLVER_H
#define _LINKAGE_RESOLVER_H

#include <functional>
#include <memory>
#include <list>
#include <unordered_map>
#include <string>

namespace Enigma::Engine
{
    template <class T> using LinkageResolver = std::function<void(const std::shared_ptr<T>& spawned)>;
    template <class T> using FactoryQuery = std::function<std::shared_ptr<T>(const std::string& name)>;

    template <class T> class ContractedLinkageResolver
    {
    public:
        ContractedLinkageResolver() = default;
        ContractedLinkageResolver(const FactoryQuery<T>& query) : m_query(query) {}
        ~ContractedLinkageResolver() = default;

        void TryResolveLinkage(const std::string& name, const LinkageResolver<T>& resolver)
        {
            if (auto spawned = m_query(name))
            {
                resolver(spawned);
            }
            else
            {
                auto iter = m_linkageResolverTable.find(name);
                if (iter != m_linkageResolverTable.end())
                {
                    iter->second.emplace_back(resolver);
                }
                else
                {
                    m_linkageResolverTable.emplace(name, std::list<LinkageResolver<T>>{ resolver });
                }
            }
        }

        void InvokeLinkageResolver(const std::string& name, const std::shared_ptr<T>& spawned)
        {
            auto iter = m_linkageResolverTable.find(name);
            if (iter == m_linkageResolverTable.end()) return;
            for (auto resolver : iter->second)
            {
                resolver(spawned);
            }
        }
    protected:
        FactoryQuery<T> m_query;
        using LinkageResolverTable = std::unordered_map<std::string, std::list<LinkageResolver<T>>>;
        LinkageResolverTable m_linkageResolverTable;
    };
}

#endif // _LINKAGE_RESOLVER_H
