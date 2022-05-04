#include "AssetNameList.h"
#include <string>
#include <cassert>
#include <vector>

using namespace Enigma::AssetPackage;

using string = std::string;
const string empty_string = { "" };

AssetNameList::AssetNameList()
{
    m_assetNames.clear();
}

AssetNameList::~AssetNameList()
{
    m_assetNames.clear();
}

const string& AssetNameList::GetAssetName(unsigned int name_idx)
{
    if (name_idx >= m_assetNames.size()) return empty_string;
    return m_assetNames[name_idx];
}

unsigned int AssetNameList::SearchAssetName(const string& name)
{
    if (name.empty()) return INVALID_NAME_IDX;
    for (unsigned int i = 0; i < m_assetNames.size(); i++)
    {
        if (m_assetNames[i] == name) return i;
    }
    return INVALID_NAME_IDX;
}

unsigned int AssetNameList::AppendAssetName(const string& name)
{
    if (name.empty()) return INVALID_NAME_IDX;
    m_assetNames.emplace_back(name);
    return (unsigned int)m_assetNames.size() - 1;
}

const std::vector<string>& AssetNameList::GetAssetNameArray()
{
    return m_assetNames;
}

size_t AssetNameList::CalcNameListDataBytes() const
{
    size_t sum = 0;
    for (const string& name : m_assetNames)
    {
        sum += (name.length() + 1);
    }
    return sum;
}

std::vector<char> AssetNameList::ExportToByteBuffer()
{
    size_t size = CalcNameListDataBytes();
    if (size == 0) return std::vector<char>();

    std::vector<char> buff;
    buff.resize(size, 0);

    size_t index = 0;
    for (const string& name : m_assetNames)
    {
        assert(index + name.length() + 1 <= size);
        memcpy(&buff[index], name.c_str(), name.length());
        index += (name.length() + 1);
    }
    return buff;
}

void AssetNameList::ImportFromByteBuffer(const std::vector<char>& buff)
{
    if (buff.empty()) return;
    m_assetNames.clear();
    size_t index = 0;
    while (index < buff.size())
    {
        string name{ &buff[index] };
        index += (name.length() + 1);
        m_assetNames.emplace_back(name);
    }
}
