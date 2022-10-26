#include "ContractJsonGateway.h"
#include "Frameworks/StringFormat.h"
#include "GameEngine/FactoryDesc.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "MathLib/Box3.h"
#include <any>

constexpr const char* TYPE_TOKEN = "Type";
constexpr const char* VALUE_TOKEN = "Value";
constexpr const char* UINT64_TOKEN = "Uint64";
constexpr const char* UINT32_TOKEN = "Uint32";
constexpr const char* STRING_TOKEN = "String";
constexpr const char* FACTORY_DESC = "FactoryDesc";
constexpr const char* BOX3 = "Box3";
constexpr const char* RTTI_NAME = "RttiName";

using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

//------------------------------------------------------------------------
Contract DeserializeJsonValue(const rapidjson::Value& value);
void DeserializeAttribute(Contract& contract, const std::string& attribute, const rapidjson::Value& value);
FactoryDesc DeserializeFactoryDesc(const rapidjson::Value& value);
std::uint64_t DeserializeUInt64(const rapidjson::Value& value);
std::uint32_t DeserializeUInt32(const rapidjson::Value& value);
std::string DeserializeString(const rapidjson::Value& value);
Box3 DeserializeBox3Values(const rapidjson::Value& value);
//------------------------------------------------------------------------
rapidjson::Value SerializeContract(const Contract& contract, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeObject(std::any ob, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeFactoryDesc(const FactoryDesc& desc, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeUInt64(const std::uint64_t n);
rapidjson::Value SerializeUInt32(const std::uint32_t n);
rapidjson::Value SerializeString(const std::string& s, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeBox3Values(const Box3& box, rapidjson::MemoryPoolAllocator<>& allocator);

std::vector<Contract> ContractJsonGateway::Deserialize(const std::string& json)
{
    std::vector<Contract> contracts;
    rapidjson::Document json_doc;
    json_doc.Parse<0>(json.c_str());
    if (FATAL_LOG_EXPR(json_doc.HasParseError()))
    {
        std::string ss = string_format("error %d, @ %d", json_doc.GetParseError(), json_doc.GetErrorOffset());
        LOG(Info, ss);
        return contracts;
    }
    if (FATAL_LOG_EXPR(!json_doc.IsArray()))
    {
        std::string ss = string_format("json is not a contract array");
        LOG(Info, ss);
        return contracts;
    }
    for (auto json_it = json_doc.GetArray().Begin(); json_it != json_doc.GetArray().End(); ++json_it)
    {
        contracts.emplace_back(DeserializeJsonValue(*json_it));
    }

    return contracts;
}

std::string ContractJsonGateway::Serialize(const std::vector<Contract>& contracts)
{
    if (contracts.empty()) return "";
    rapidjson::Document json_doc;
    json_doc.SetArray();
    for (auto contract : contracts)
    {
        json_doc.PushBack(SerializeContract(contract, json_doc.GetAllocator()), json_doc.GetAllocator());
    }

    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter writer(buf);
    json_doc.Accept(writer);
    std::string json(buf.GetString(), buf.GetSize());
    return json;
}

//-------------------------------------------------------------------------
Contract DeserializeJsonValue(const rapidjson::Value& value_ob)
{
    Contract contract;
    for (auto json_it = value_ob.MemberBegin(); json_it != value_ob.MemberEnd(); ++json_it)
    {
        if (json_it->value.IsNull()) continue;
        DeserializeAttribute(contract, json_it->name.GetString(), json_it->value);
    }
    return contract;
}

void DeserializeAttribute(Contract& contract, const std::string& attribute, const rapidjson::Value& value)
{
    if (!value.HasMember(TYPE_TOKEN)) return;
    if (!value.HasMember(VALUE_TOKEN)) return;
    std::string type = value[TYPE_TOKEN].GetString();
    if (type == FACTORY_DESC)
        contract.AddOrUpdate(attribute, DeserializeFactoryDesc(value[VALUE_TOKEN]));
    else if (type == UINT64_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeUInt64(value[VALUE_TOKEN]));
    else if (type == UINT32_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeUInt32(value[VALUE_TOKEN]));
    else if (type == STRING_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeString(value[VALUE_TOKEN]));
    else if (type == BOX3)
        contract.AddOrUpdate(attribute, DeserializeBox3Values(value[VALUE_TOKEN]));
}

FactoryDesc DeserializeFactoryDesc(const rapidjson::Value& value)
{
    //FactoryDesc::InstanceType instance_type;
    //std::string resource_name;
    //std::string resource_filename;
    std::string rtti;
    //std::string prefab;
    //if (value.HasMember(INSTANCE_TYPE)) instance_type = static_cast<FactoryDesc::InstanceType>(value[INSTANCE_TYPE].GetInt());
    //if (value.HasMember(RESOURCE_NAME)) resource_name = value[RESOURCE_NAME].GetString();
    //if (value.HasMember(RESOURCE_FILENAME)) resource_filename = value[RESOURCE_FILENAME].GetString();
    if (value.HasMember(RTTI_NAME)) rtti = value[RTTI_NAME].GetString();
    //if (value.HasMember(PREFAB_FILENAME)) prefab = value[PREFAB_FILENAME].GetString();
    FactoryDesc desc(rtti);
    /*desc.SetRtti(rtti);
    switch (instance_type)
    {
    case FactoryDesc::InstanceType::Native:
        desc.ClaimAsNative(resource_name);
        break;
    case FactoryDesc::InstanceType::ByPrefab:
        desc.ClaimByPrefab(prefab);
        break;
    case FactoryDesc::InstanceType::Deferred:
        desc.ClaimAsDeferred(prefab);
        break;
    case FactoryDesc::InstanceType::Instanced:
        desc.ClaimAsInstanced(prefab);
        break;
    case FactoryDesc::InstanceType::FromResource:
        desc.ClaimFromResource(resource_name, resource_filename);
        break;
    case FactoryDesc::InstanceType::ResourceAsset:
        desc.ClaimAsResourceAsset(resource_name, resource_filename);
        break;
    }*/
    return desc;
}

std::uint64_t DeserializeUInt64(const rapidjson::Value& value)
{
    return value.GetUint64();
}

std::uint32_t DeserializeUInt32(const rapidjson::Value& value)
{
    return value.GetUint();
}

std::string DeserializeString(const rapidjson::Value& value)
{
    return value.GetString();
}

Box3 DeserializeBox3Values(const rapidjson::Value& value)
{
    Box3 box;
    if (!value.IsArray()) return box;
    auto values = value.GetArray();
    box.Center() = Vector3(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat());
    box.Axis(0) = Vector3(values[3].GetFloat(), values[4].GetFloat(), values[5].GetFloat());
    box.Axis(1) = Vector3(values[6].GetFloat(), values[7].GetFloat(), values[8].GetFloat());
    box.Axis(2) = Vector3(values[9].GetFloat(), values[10].GetFloat(), values[11].GetFloat());
    box.Extent(0) = values[12].GetFloat();
    box.Extent(1) = values[13].GetFloat();
    box.Extent(2) = values[14].GetFloat();

    return box;
}

//--------------------------------------------------------------------------
rapidjson::Value SerializeContract(const Contract& contract, rapidjson::MemoryPoolAllocator<>& allocator)
{
    if (contract.IsEmpty()) return rapidjson::Value();
    rapidjson::Value json{ rapidjson::kObjectType };
    for (auto it : contract)
    {
        json.AddMember(SerializeString(it.first, allocator), SerializeObject(it.second, allocator), allocator);
    }

    return json;
}

rapidjson::Value SerializeObject(std::any any_ob, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value node{ rapidjson::kObjectType };
    if (any_ob.type() == typeid(FactoryDesc))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(FACTORY_DESC), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeFactoryDesc(std::any_cast<FactoryDesc>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::uint64_t))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(UINT64_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeUInt64(std::any_cast<std::uint64_t>(any_ob)), allocator);
    }
    else if (any_ob.type() == typeid(std::uint32_t))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(UINT32_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeUInt32(std::any_cast<std::uint32_t>(any_ob)), allocator);
    }
    else if (any_ob.type() == typeid(std::string))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(STRING_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeString(std::any_cast<std::string>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Box3))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(BOX3), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeBox3Values(std::any_cast<Box3>(any_ob), allocator), allocator);
    }
    return node;
}

rapidjson::Value SerializeFactoryDesc(const FactoryDesc& desc, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value{ rapidjson::kObjectType };
    //value.AddMember(rapidjson::StringRef(INSTANCE_TYPE), static_cast<int>(desc.GetInstanceType()), allocator);
    //value.AddMember(rapidjson::StringRef(RESOURCE_NAME), SerializeString(desc.GetResourceName(), allocator), allocator);
    //value.AddMember(rapidjson::StringRef(RESOURCE_FILENAME), SerializeString(desc.GetResourceFilename(), allocator), allocator);
    value.AddMember(rapidjson::StringRef(RTTI_NAME), SerializeString(desc.GetRttiName(), allocator), allocator);
    //value.AddMember(rapidjson::StringRef(PREFAB_FILENAME), SerializeString(desc.GetPrefab(), allocator), allocator);
    return value;
}

rapidjson::Value SerializeUInt64(const std::uint64_t n)
{
    return rapidjson::Value(n);
}

rapidjson::Value SerializeUInt32(const std::uint32_t n)
{
    return rapidjson::Value(n);
}

rapidjson::Value SerializeString(const std::string& s, rapidjson::MemoryPoolAllocator<>& allocator)
{
    return rapidjson::Value(s, allocator);
}

rapidjson::Value SerializeBox3Values(const Box3& box, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(box.Center().X(), allocator);
    value.PushBack(box.Center().Y(), allocator);
    value.PushBack(box.Center().Z(), allocator);
    for (int i = 0; i < 3; i++)
    {
        value.PushBack(box.Axis(i).X(), allocator);
        value.PushBack(box.Axis(i).Y(), allocator);
        value.PushBack(box.Axis(i).Z(), allocator);
    }
    value.PushBack(box.Extent(0), allocator);
    value.PushBack(box.Extent(1), allocator);
    value.PushBack(box.Extent(2), allocator);

    return value;
}
