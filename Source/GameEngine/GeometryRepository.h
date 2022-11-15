/*********************************************************************
 * \file   GeometryRepository.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_REPOSITORY_H
#define _GEOMETRY_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include <memory>
#include <mutex>

namespace Enigma::Engine
{
    class GeometryData;

    class GeometryRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GeometryRepository(Frameworks::ServiceManager* srv_manager);
        GeometryRepository(const GeometryRepository&) = delete;
        GeometryRepository(GeometryRepository&&) = delete;
        ~GeometryRepository() override;
        GeometryRepository& operator=(const GeometryRepository&) = delete;
        GeometryRepository& operator=(GeometryRepository&&) = delete;

        bool HasGeometryData(const std::string& name);
        std::shared_ptr<GeometryData> QueryGeometryData(const std::string& name);

    protected:
        std::unordered_map<std::string, std::weak_ptr<GeometryData>> m_geometries;
        std::recursive_mutex m_geometryLock;
    };
}

#endif // _GEOMETRY_REPOSITORY_H
