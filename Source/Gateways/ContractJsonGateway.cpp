#include "ContractJsonGateway.h"
#include "Frameworks/StringFormat.h"
#include "GameEngine/FactoryDesc.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "MathLib/Box3.h"
#include "MathLib/Matrix4.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/ColorRGB.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector4.h"
#include <any>

constexpr const char* TYPE_TOKEN = "Type";
constexpr const char* VALUE_TOKEN = "Value";
constexpr const char* CONTRACT_TOKEN = "Contract";
constexpr const char* UINT64_TOKEN = "Uint64";
constexpr const char* UINT32_TOKEN = "Uint32";
constexpr const char* FLOAT_TOKEN = "Float";
constexpr const char* STRING_TOKEN = "String";
constexpr const char* BOOLEAN_TOKEN = "Boolean";
constexpr const char* FACTORY_DESC_TOKEN = "FactoryDesc";
constexpr const char* COLOR_RGBA_TOKEN = "ColorRGBA";
constexpr const char* COLOR_RGB_TOKEN = "ColorRGB";
constexpr const char* VECTOR3_TOKEN = "Vector3";
constexpr const char* VECTOR4_TOKEN = "Vector4";
constexpr const char* BOX3_TOKEN = "Box3";
constexpr const char* MATRIX4_TOKEN = "Matrix4";
constexpr const char* STRING_ARRAY_TOKEN = "StringArray";
constexpr const char* RTTI_NAME = "RttiName";

using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

//------------------------------------------------------------------------
Contract DeserializeContract(const rapidjson::Value& value);
void DeserializeAttribute(Contract& contract, const std::string& attribute, const rapidjson::Value& value);
FactoryDesc DeserializeFactoryDesc(const rapidjson::Value& value);
std::uint64_t DeserializeUInt64(const rapidjson::Value& value);
std::uint32_t DeserializeUInt32(const rapidjson::Value& value);
float DeserializeFloat(const rapidjson::Value& value);
std::string DeserializeString(const rapidjson::Value& value);
bool DeserializeBoolean(const rapidjson::Value& value);
ColorRGBA DeserializeColorRGBAValues(const rapidjson::Value& value);
ColorRGB DeserializeColorRGBValues(const rapidjson::Value& value);
Vector3 DeserializeVector3Values(const rapidjson::Value& value);
Vector4 DeserializeVector4Values(const rapidjson::Value& value);
Box3 DeserializeBox3Values(const rapidjson::Value& value);
Matrix4 DeserializeMatrix4Values(const rapidjson::Value& value);
std::vector<std::string> DeserializeStringArrayValues(const rapidjson::Value& value);
//------------------------------------------------------------------------
rapidjson::Value SerializeContract(const Contract& contract, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeObject(std::any ob, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeFactoryDesc(const FactoryDesc& desc, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeUInt64(const std::uint64_t n);
rapidjson::Value SerializeUInt32(const std::uint32_t n);
rapidjson::Value SerializeFloat(const float v);
rapidjson::Value SerializeString(const std::string& s, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeBoolean(const bool b);
rapidjson::Value SerializeColorRGBAValues(const ColorRGBA& color, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeColorRGBValues(const ColorRGB& color, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeVector3Values(const Vector3& vec, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeVector4Values(const Vector4& vec, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeBox3Values(const Box3& box, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeMatrix4Values(const Matrix4& mx, rapidjson::MemoryPoolAllocator<>& allocator);
rapidjson::Value SerializeStringArrayValues(const std::vector<std::string>& ss, rapidjson::MemoryPoolAllocator<>& allocator);

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
        contracts.emplace_back(DeserializeContract(*json_it));
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
    writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    json_doc.Accept(writer);
    std::string json(buf.GetString(), buf.GetSize());
    return json;
}

//-------------------------------------------------------------------------
Contract DeserializeContract(const rapidjson::Value& value_ob)
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
    if (type == CONTRACT_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeContract(value[VALUE_TOKEN]));
    else if (type == FACTORY_DESC_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeFactoryDesc(value[VALUE_TOKEN]));
    else if (type == UINT64_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeUInt64(value[VALUE_TOKEN]));
    else if (type == UINT32_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeUInt32(value[VALUE_TOKEN]));
    else if (type == FLOAT_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeFloat(value[VALUE_TOKEN]));
    else if (type == STRING_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeString(value[VALUE_TOKEN]));
    else if (type == BOOLEAN_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeBoolean(value[VALUE_TOKEN]));
    else if (type == COLOR_RGBA_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeColorRGBAValues(value[VALUE_TOKEN]));
    else if (type == COLOR_RGB_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeColorRGBValues(value[VALUE_TOKEN]));
    else if (type == VECTOR3_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeVector3Values(value[VALUE_TOKEN]));
    else if (type == VECTOR4_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeVector4Values(value[VALUE_TOKEN]));
    else if (type == BOX3_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeBox3Values(value[VALUE_TOKEN]));
    else if (type == MATRIX4_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeMatrix4Values(value[VALUE_TOKEN]));
    else if (type == STRING_ARRAY_TOKEN)
        contract.AddOrUpdate(attribute, DeserializeStringArrayValues(value[VALUE_TOKEN]));
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

float DeserializeFloat(const rapidjson::Value& value)
{
    return value.GetFloat();
}

std::string DeserializeString(const rapidjson::Value& value)
{
    return value.GetString();
}
bool DeserializeBoolean(const rapidjson::Value& value)
{
    return value.GetBool();
}
ColorRGBA DeserializeColorRGBAValues(const rapidjson::Value& value)
{
    if (!value.IsArray()) return ColorRGBA::WHITE;
    auto values = value.GetArray();
    return ColorRGBA(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat());
}

ColorRGB DeserializeColorRGBValues(const rapidjson::Value& value)
{
    if (!value.IsArray()) return ColorRGB::WHITE;
    auto values = value.GetArray();
    return ColorRGB(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat());
}
Vector3 DeserializeVector3Values(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Vector3::ZERO;
    auto values = value.GetArray();
    return Vector3(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat());
}
Vector4 DeserializeVector4Values(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Vector4::ZERO;
    auto values = value.GetArray();
    return Vector4(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat());
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

Matrix4 DeserializeMatrix4Values(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Matrix4::IDENTITY;
    auto values = value.GetArray();
    return Matrix4(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat(),
        values[4].GetFloat(), values[5].GetFloat(), values[6].GetFloat(), values[7].GetFloat(),
        values[8].GetFloat(), values[9].GetFloat(), values[10].GetFloat(), values[11].GetFloat(),
        values[12].GetFloat(), values[13].GetFloat(), values[14].GetFloat(), values[15].GetFloat());
}

std::vector<std::string> DeserializeStringArrayValues(const rapidjson::Value& value)
{
    std::vector<std::string> ss;
    if (!value.IsArray()) return ss;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        ss.emplace_back(DeserializeString(*json_it));
    }
    return ss;
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
    if (any_ob.type() == typeid(Contract))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(CONTRACT_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeContract(std::any_cast<Contract>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(FactoryDesc))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(FACTORY_DESC_TOKEN), allocator);
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
    else if (any_ob.type() == typeid(float))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(FLOAT_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeFloat(std::any_cast<float>(any_ob)), allocator);
    }
    else if (any_ob.type() == typeid(std::string))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(STRING_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeString(std::any_cast<std::string>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(bool))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(BOOLEAN_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeBoolean(std::any_cast<bool>(any_ob)), allocator);
    }
    else if (any_ob.type() == typeid(ColorRGBA))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(COLOR_RGBA_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeColorRGBAValues(std::any_cast<ColorRGBA>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(ColorRGB))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(COLOR_RGB_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeColorRGBValues(std::any_cast<ColorRGB>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Vector3))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR3_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector3Values(std::any_cast<Vector3>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Vector4))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR4_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector4Values(std::any_cast<Vector4>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Box3))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(BOX3_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeBox3Values(std::any_cast<Box3>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Matrix4))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(MATRIX4_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeMatrix4Values(std::any_cast<Matrix4>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<std::string>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(STRING_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeStringArrayValues(std::any_cast<std::vector<std::string>>(any_ob), allocator), allocator);
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

rapidjson::Value SerializeFloat(const float v)
{
    return rapidjson::Value(v);
}

rapidjson::Value SerializeString(const std::string& s, rapidjson::MemoryPoolAllocator<>& allocator)
{
    return rapidjson::Value(s, allocator);
}

rapidjson::Value SerializeBoolean(const bool b)
{
    return rapidjson::Value(b);
}
rapidjson::Value SerializeColorRGBAValues(const ColorRGBA& color, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(color.R(), allocator);
    value.PushBack(color.G(), allocator);
    value.PushBack(color.B(), allocator);
    value.PushBack(color.A(), allocator);
    return value;
}

rapidjson::Value SerializeColorRGBValues(const ColorRGB& color, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(color.R(), allocator);
    value.PushBack(color.G(), allocator);
    value.PushBack(color.B(), allocator);
    return value;
}

rapidjson::Value SerializeVector3Values(const Vector3& vec, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(vec.X(), allocator);
    value.PushBack(vec.Y(), allocator);
    value.PushBack(vec.Z(), allocator);
    return value;
}

rapidjson::Value SerializeVector4Values(const Vector4& vec, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(vec.X(), allocator);
    value.PushBack(vec.Y(), allocator);
    value.PushBack(vec.Z(), allocator);
    value.PushBack(vec.W(), allocator);
    return value;
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

rapidjson::Value SerializeMatrix4Values(const Matrix4& mx, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (int i = 0; i < 4; i++)
    {
        value.PushBack(mx[i][0], allocator);
        value.PushBack(mx[i][1], allocator);
        value.PushBack(mx[i][2], allocator);
        value.PushBack(mx[i][3], allocator);
    }
    return value;
}

rapidjson::Value SerializeStringArrayValues(const std::vector<std::string>& ss, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto s : ss)
    {
        value.PushBack(SerializeString(s, allocator), allocator);
    }
    return value;
}
