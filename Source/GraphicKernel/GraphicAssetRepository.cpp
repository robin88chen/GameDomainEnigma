#include "GraphicAssetRepository.h"

using namespace Enigma::Graphics;

AssetRepository::AssetRepository()
{
}

AssetRepository::~AssetRepository()
{
    m_dataValues.clear();
}

void AssetRepository::Clear()
{
    m_dataValues.clear();
}

void AssetRepository::Remove(const std::string& key)
{
    if (!HasData(key)) return;
    m_dataValues.erase(key);
}

bool AssetRepository::HasData(const std::string& key)
{
    return m_dataValues.find(key) != m_dataValues.end();
}
