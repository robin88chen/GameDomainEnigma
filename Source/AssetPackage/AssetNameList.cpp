#include "AssetNameList.h"
#include "AssetErrors.h"
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

bool AssetNameList::HasAssetName(const string& name)
{
    if (name.empty()) return false;
    return m_assetNames.find(name) != m_assetNames.end();
}

error AssetNameList::AppendAssetName(const string& name)
{
    if (name.empty()) return make_error_code(ErrorCode::EmptyKey);
    if (HasAssetName(name)) return make_error_code(ErrorCode::DuplicatedKey);
    m_assetNames.emplace(name);
    return make_error_code(ErrorCode::OK);
}

error AssetNameList::RemoveAssetName(const std::string& name)
{
    if (name.empty()) return make_error_code(ErrorCode::EmptyKey);
    if (!HasAssetName(name)) return make_error_code(ErrorCode::NotExistedKey);
    m_assetNames.erase(name);
    return make_error_code(ErrorCode::OK);
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

error AssetNameList::ImportFromByteBuffer(const std::vector<char>& buff)
{
    if (buff.empty()) return make_error_code(ErrorCode::EmptyBuffer);
    m_assetNames.clear();
    size_t index = 0;
    error er;
    while ((index < buff.size()) && (!er))
    {
        string name{ &buff[index] };
        index += (name.length() + 1);
        er = AppendAssetName(name);
    }
    return er;
}
