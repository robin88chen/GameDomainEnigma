#include "GraphicAssetStash.h"

using namespace Enigma::Graphics;

AssetStash::AssetStash()
{
}

AssetStash::~AssetStash()
{
    m_dataValues.clear();
}

void AssetStash::clear()
{
    std::lock_guard locker{ m_lock };
    m_dataValues.clear();
}

void AssetStash::remove(const std::string& key)
{
    std::lock_guard locker{ m_lock };
    if (!HasData(key)) return;
    m_dataValues.erase(key);
}

bool AssetStash::HasData(const std::string& key)
{
    std::lock_guard locker{ m_lock };
    return m_dataValues.find(key) != m_dataValues.end();
}
