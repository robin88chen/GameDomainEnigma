#include "GeometryCreationHelper.h"
#include "Geometries/StandardGeometryAssemblers.h"
#include "Geometries/TriangleList.h"
#include "Frameworks/CommandBus.h"
#include "Geometries/GeometryCommands.h"
#include "Geometries/GeometryDataQueries.h"

using namespace EnigmaViewer;
using namespace Enigma::Geometries;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

std::shared_ptr<GeometryData> GeometryCreationHelper::createSquareXZQuad(const GeometryId& id)
{
    auto geometry = GeometryData::queryGeometryData(id);
    if (geometry) return geometry;

    SquareQuadAssembler helper(id);
    helper.xzQuad(Vector3(-5.0f, 0.0f, -5.0f), Vector3(5.0f, 0.0f, 5.0f)).normal().textureCoord(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f));
    auto dto = helper.toGenericDto();
    dto.addRtti(FactoryDesc(TriangleList::TYPE_RTTI.getName()).ClaimAsResourceAsset(id.name(), id.name() + ".geo", "APK_PATH"));
    const auto request = std::make_shared<RequestGeometryConstitution>(id, dto);
    request->dispatch();
    return request->getResult();
}
