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
#include <unordered_set>
#include <string>
#include <vector>

namespace Enigma::Engine
{
    template <class T> using LinkageResolver = std::function<void(const std::shared_ptr<T>& spawned)>;
    template <class T> using FactoryQuery = std::function<std::shared_ptr<T>(const std::string& name)>;

    template <class T> class FactoryLinkageResolver
    {
    public:
        FactoryLinkageResolver() = default;
        FactoryLinkageResolver(const FactoryQuery<T>& query) : m_query(query) {}
        FactoryLinkageResolver(const FactoryLinkageResolver&) = default;
        FactoryLinkageResolver(FactoryLinkageResolver&&) = default;
        ~FactoryLinkageResolver() = default;
        FactoryLinkageResolver& operator=(const FactoryLinkageResolver&) = default;
        FactoryLinkageResolver& operator=(FactoryLinkageResolver&&) = default;

        void tryResolveLinkage(const std::string& name, const LinkageResolver<T>& resolver)
        {
            if (auto spawned = m_query(name))
            {
                resolver(spawned);
            }
            else
            {
                m_unresolvedNames.insert(name);
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

        void invokeLinkageResolver(const std::string& name, const std::shared_ptr<T>& spawned)
        {
            auto iter = m_linkageResolverTable.find(name);
            if (iter == m_linkageResolverTable.end()) return;
            m_unresolvedNames.erase(name);
            for (auto resolver : iter->second)
            {
                resolver(spawned);
            }
        }

        std::vector<std::string> getUnresolvedNames() const
        {
            return std::vector<std::string>(m_unresolvedNames.begin(), m_unresolvedNames.end());
        }
    protected:
        FactoryQuery<T> m_query;
        using LinkageResolverTable = std::unordered_map<std::string, std::list<LinkageResolver<T>>>;
        LinkageResolverTable m_linkageResolverTable;
        std::unordered_set<std::string> m_unresolvedNames;
    };
}

#endif // _LINKAGE_RESOLVER_H
