/*****************************************************************
 * \file   GeometryFactoryDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef GEOMETRY_FACTORY_DELEGATE_H
#define GEOMETRY_FACTORY_DELEGATE_H

#include <functional>
#include <memory>

namespace Enigma::Engine
{
    class GenericDto;
}

namespace Enigma::Geometries
{
    class GeometryId;
    class GeometryData;
    using GeometryCreator = std::function<std::shared_ptr<GeometryData>(const GeometryId& id)>;
    using GeometryConstitutor = std::function<std::shared_ptr<GeometryData>(const GeometryId& id, const Engine::GenericDto& dto)>;
}

#endif // GEOMETRY_FACTORY_DELEGATE_H
