#include "DtoJsonGateway.h"
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
#include "MathLib/Vector2.h"
#include <any>

constexpr const char* TYPE_TOKEN = "Type";
constexpr const char* VALUE_TOKEN = "Value";
constexpr const char* DATA_OBJECT_TOKEN = "DataObject";
constexpr const char* DATA_OBJECT_ARRAY_TOKEN = "DataObjectArray";
constexpr const char* UINT64_TOKEN = "Uint64";
constexpr const char* UINT32_TOKEN = "Uint32";
constexpr const char* FLOAT_TOKEN = "Float";
constexpr const char* STRING_TOKEN = "String";
constexpr const char* BOOLEAN_TOKEN = "Boolean";
constexpr const char* FACTORY_DESC_TOKEN = "FactoryDesc";
constexpr const char* COLOR_RGBA_TOKEN = "ColorRGBA";
constexpr const char* COLOR_RGB_TOKEN = "ColorRGB";
constexpr const char* VECTOR2_TOKEN = "Vector2";
constexpr const char* VECTOR3_TOKEN = "Vector3";
constexpr const char* VECTOR4_TOKEN = "Vector4";
constexpr const char* BOX3_TOKEN = "Box3";
constexpr const char* MATRIX4_TOKEN = "Matrix4";
constexpr const char* STRING_ARRAY_TOKEN = "StringArray";
constexpr const char* UINT32_ARRAY_TOKEN = "Uint32Array";
constexpr const char* FLOAT_ARRAY_TOKEN = "FloatArray";
constexpr const char* VECTOR2_ARRAY_TOKEN = "Vector2Array";
constexpr const char* VECTOR3_ARRAY_TOKEN = "Vector3Array";
constexpr const char* VECTOR4_ARRAY_TOKEN = "Vector4Array";
constexpr const char* MATRIX4_ARRAY_TOKEN = "Matrix4Array";
constexpr const char* RTTI_NAME = "RttiName";
constexpr const char* INSTANCE_TYPE = "InstanceType";
constexpr const char* RESOURCE_NAME = "ResourceName";
constexpr const char* RESOURCE_FILENAME = "ResourceFilename";
constexpr const char* PREFAB_FILENAME = "PrefabFilename";

using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

//------------------------------------------------------------------------
static GenericDto DeserializeDto(const rapidjson::Value& value);
static std::vector<GenericDto> DeserializeDtoArray(const rapidjson::Value& value);
static void DeserializeAttribute(GenericDto& dto, const std::string& attribute, const rapidjson::Value& value);
static FactoryDesc DeserializeFactoryDesc(const rapidjson::Value& value);
static std::uint64_t DeserializeUInt64(const rapidjson::Value& value);
static std::uint32_t DeserializeUInt32(const rapidjson::Value& value);
static float DeserializeFloat(const rapidjson::Value& value);
static std::string DeserializeString(const rapidjson::Value& value);
static bool DeserializeBoolean(const rapidjson::Value& value);
static ColorRGBA DeserializeColorRGBA(const rapidjson::Value& value);
static ColorRGB DeserializeColorRGB(const rapidjson::Value& value);
static Vector2 DeserializeVector2(const rapidjson::Value& value);
static Vector3 DeserializeVector3(const rapidjson::Value& value);
static Vector4 DeserializeVector4(const rapidjson::Value& value);
static Box3 DeserializeBox3(const rapidjson::Value& value);
static Matrix4 DeserializeMatrix4(const rapidjson::Value& value);
static std::vector<std::string> DeserializeStringArray(const rapidjson::Value& value);
static std::vector<std::uint32_t> DeserializeUInt32Array(const rapidjson::Value& value);
static std::vector<float> DeserializeFloatArray(const rapidjson::Value& value);
static std::vector<Vector2> DeserializeVector2Array(const rapidjson::Value& value);
static std::vector<Vector3> DeserializeVector3Array(const rapidjson::Value& value);
static std::vector<Vector4> DeserializeVector4Array(const rapidjson::Value& value);
static std::vector<Matrix4> DeserializeMatrix4Array(const rapidjson::Value& value);
//------------------------------------------------------------------------
static rapidjson::Value SerializeDto(const GenericDto& dto, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeDtoArray(const std::vector<GenericDto>& dtos, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeObject(std::any ob, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeFactoryDesc(const FactoryDesc& desc, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeUInt64(const std::uint64_t n);
static rapidjson::Value SerializeUInt32(const std::uint32_t n);
static rapidjson::Value SerializeFloat(const float v);
static rapidjson::Value SerializeString(const std::string& s, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeBoolean(const bool b);
static rapidjson::Value SerializeColorRGBA(const ColorRGBA& color, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeColorRGB(const ColorRGB& color, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeVector2(const Vector2& vec, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeVector3(const Vector3& vec, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeVector4(const Vector4& vec, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeBox3(const Box3& box, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeMatrix4(const Matrix4& mx, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeStringArray(const std::vector<std::string>& ss, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeUInt32Array(const std::vector<std::uint32_t>& ns, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeFloatArray(const std::vector<float>& vs, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeVector2Array(const std::vector<Vector2>& vecs, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeVector3Array(const std::vector<Vector3>& vecs, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeVector4Array(const std::vector<Vector4>& vecs, rapidjson::MemoryPoolAllocator<>& allocator);
static rapidjson::Value SerializeMatrix4Array(const std::vector<Matrix4>& mxs, rapidjson::MemoryPoolAllocator<>& allocator);

std::vector<GenericDto> DtoJsonGateway::Deserialize(const std::string& json)
{
    std::vector<GenericDto> dtos;
    rapidjson::Document json_doc;
    json_doc.Parse<0>(json.c_str());
    if (FATAL_LOG_EXPR(json_doc.HasParseError()))
    {
        std::string ss = string_format("error %d, @ %d", json_doc.GetParseError(), json_doc.GetErrorOffset());
        LOG(Info, ss);
        return dtos;
    }
    if (FATAL_LOG_EXPR(!json_doc.IsArray()))
    {
        std::string ss = string_format("json is not a contract array");
        LOG(Info, ss);
        return dtos;
    }
    for (auto json_it = json_doc.GetArray().Begin(); json_it != json_doc.GetArray().End(); ++json_it)
    {
        dtos.emplace_back(DeserializeDto(*json_it));
    }

    return dtos;
}

std::string DtoJsonGateway::Serialize(const std::vector<GenericDto>& dtos)
{
    if (dtos.empty()) return "";
    rapidjson::Document json_doc;
    json_doc.SetArray();
    for (auto dto : dtos)
    {
        json_doc.PushBack(SerializeDto(dto, json_doc.GetAllocator()), json_doc.GetAllocator());
    }

    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter writer(buf);
    writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    json_doc.Accept(writer);
    std::string json(buf.GetString(), buf.GetSize());
    return json;
}

//-------------------------------------------------------------------------
GenericDto DeserializeDto(const rapidjson::Value& value_ob)
{
    GenericDto dto;
    for (auto json_it = value_ob.MemberBegin(); json_it != value_ob.MemberEnd(); ++json_it)
    {
        if (json_it->value.IsNull()) continue;
        DeserializeAttribute(dto, json_it->name.GetString(), json_it->value);
    }
    return dto;
}

std::vector<GenericDto> DeserializeDtoArray(const rapidjson::Value& value)
{
    std::vector<GenericDto> dtos;
    if (!value.IsArray()) return dtos;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        dtos.emplace_back(DeserializeDto(*json_it));
    }
    return dtos;
}

void DeserializeAttribute(GenericDto& dto, const std::string& attribute, const rapidjson::Value& value)
{
    if (!value.HasMember(TYPE_TOKEN)) return;
    if (!value.HasMember(VALUE_TOKEN)) return;
    std::string type = value[TYPE_TOKEN].GetString();
    if (type == DATA_OBJECT_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeDto(value[VALUE_TOKEN]));
    else if (type == DATA_OBJECT_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeDtoArray(value[VALUE_TOKEN]));
    else if (type == FACTORY_DESC_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeFactoryDesc(value[VALUE_TOKEN]));
    else if (type == UINT64_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeUInt64(value[VALUE_TOKEN]));
    else if (type == UINT32_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeUInt32(value[VALUE_TOKEN]));
    else if (type == FLOAT_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeFloat(value[VALUE_TOKEN]));
    else if (type == STRING_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeString(value[VALUE_TOKEN]));
    else if (type == BOOLEAN_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeBoolean(value[VALUE_TOKEN]));
    else if (type == COLOR_RGBA_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeColorRGBA(value[VALUE_TOKEN]));
    else if (type == COLOR_RGB_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeColorRGB(value[VALUE_TOKEN]));
    else if (type == VECTOR2_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeVector2(value[VALUE_TOKEN]));
    else if (type == VECTOR3_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeVector3(value[VALUE_TOKEN]));
    else if (type == VECTOR4_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeVector4(value[VALUE_TOKEN]));
    else if (type == BOX3_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeBox3(value[VALUE_TOKEN]));
    else if (type == MATRIX4_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeMatrix4(value[VALUE_TOKEN]));
    else if (type == STRING_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeStringArray(value[VALUE_TOKEN]));
    else if (type == UINT32_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeUInt32Array(value[VALUE_TOKEN]));
    else if (type == FLOAT_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeFloatArray(value[VALUE_TOKEN]));
    else if (type == VECTOR2_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeVector2Array(value[VALUE_TOKEN]));
    else if (type == VECTOR3_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeVector3Array(value[VALUE_TOKEN]));
    else if (type == VECTOR4_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeVector4Array(value[VALUE_TOKEN]));
    else if (type == MATRIX4_ARRAY_TOKEN)
        dto.AddOrUpdate(attribute, DeserializeMatrix4Array(value[VALUE_TOKEN]));
}

FactoryDesc DeserializeFactoryDesc(const rapidjson::Value& value)
{
    FactoryDesc::InstanceType instance_type = FactoryDesc::InstanceType::Native;
    std::string resource_name;
    std::string resource_filename;
    std::string rtti;
    std::string prefab;
    if (value.HasMember(INSTANCE_TYPE)) instance_type = static_cast<FactoryDesc::InstanceType>(value[INSTANCE_TYPE].GetInt());
    if (value.HasMember(RESOURCE_NAME)) resource_name = value[RESOURCE_NAME].GetString();
    if (value.HasMember(RESOURCE_FILENAME)) resource_filename = value[RESOURCE_FILENAME].GetString();
    if (value.HasMember(RTTI_NAME)) rtti = value[RTTI_NAME].GetString();
    if (value.HasMember(PREFAB_FILENAME)) prefab = value[PREFAB_FILENAME].GetString();
    FactoryDesc desc(rtti);
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
    }
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
ColorRGBA DeserializeColorRGBA(const rapidjson::Value& value)
{
    if (!value.IsArray()) return ColorRGBA::WHITE;
    auto values = value.GetArray();
    return ColorRGBA(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat());
}

ColorRGB DeserializeColorRGB(const rapidjson::Value& value)
{
    if (!value.IsArray()) return ColorRGB::WHITE;
    auto values = value.GetArray();
    return ColorRGB(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat());
}
Vector2 DeserializeVector2(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Vector2::ZERO;
    auto values = value.GetArray();
    return Vector2(values[0].GetFloat(), values[1].GetFloat());
}
Vector3 DeserializeVector3(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Vector3::ZERO;
    auto values = value.GetArray();
    return Vector3(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat());
}
Vector4 DeserializeVector4(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Vector4::ZERO;
    auto values = value.GetArray();
    return Vector4(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat());
}

Box3 DeserializeBox3(const rapidjson::Value& value)
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

Matrix4 DeserializeMatrix4(const rapidjson::Value& value)
{
    if (!value.IsArray()) return Matrix4::IDENTITY;
    auto values = value.GetArray();
    return Matrix4(values[0].GetFloat(), values[1].GetFloat(), values[2].GetFloat(), values[3].GetFloat(),
        values[4].GetFloat(), values[5].GetFloat(), values[6].GetFloat(), values[7].GetFloat(),
        values[8].GetFloat(), values[9].GetFloat(), values[10].GetFloat(), values[11].GetFloat(),
        values[12].GetFloat(), values[13].GetFloat(), values[14].GetFloat(), values[15].GetFloat());
}

std::vector<std::string> DeserializeStringArray(const rapidjson::Value& value)
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

std::vector<std::uint32_t> DeserializeUInt32Array(const rapidjson::Value& value)
{
    std::vector<std::uint32_t> ns;
    if (!value.IsArray()) return ns;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        ns.emplace_back(DeserializeUInt32(*json_it));
    }
    return ns;
}

std::vector<float> DeserializeFloatArray(const rapidjson::Value& value)
{
    std::vector<float> vs;
    if (!value.IsArray()) return vs;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        vs.emplace_back(DeserializeFloat(*json_it));
    }
    return vs;
}

std::vector<Vector2> DeserializeVector2Array(const rapidjson::Value& value)
{
    std::vector<Vector2> vecs;
    if (!value.IsArray()) return vecs;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        vecs.emplace_back(DeserializeVector2(*json_it));
    }
    return vecs;
}

std::vector<Vector3> DeserializeVector3Array(const rapidjson::Value& value)
{
    std::vector<Vector3> vecs;
    if (!value.IsArray()) return vecs;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        vecs.emplace_back(DeserializeVector3(*json_it));
    }
    return vecs;
}

std::vector<Vector4> DeserializeVector4Array(const rapidjson::Value& value)
{
    std::vector<Vector4> vecs;
    if (!value.IsArray()) return vecs;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        vecs.emplace_back(DeserializeVector4(*json_it));
    }
    return vecs;
}

std::vector<Matrix4> DeserializeMatrix4Array(const rapidjson::Value& value)
{
    std::vector<Matrix4> mxs;
    if (!value.IsArray()) return mxs;
    auto values = value.GetArray();
    for (auto json_it = values.Begin(); json_it != values.End(); ++json_it)
    {
        mxs.emplace_back(DeserializeMatrix4(*json_it));
    }
    return mxs;
}

//--------------------------------------------------------------------------
rapidjson::Value SerializeDto(const GenericDto& dto, rapidjson::MemoryPoolAllocator<>& allocator)
{
    if (dto.IsEmpty()) return rapidjson::Value();
    rapidjson::Value json{ rapidjson::kObjectType };
    for (auto it : dto)
    {
        json.AddMember(SerializeString(it.first, allocator), SerializeObject(it.second, allocator), allocator);
    }

    return json;
}

rapidjson::Value SerializeDtoArray(const std::vector<GenericDto>& dtos, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto& o : dtos)
    {
        value.PushBack(SerializeDto(o, allocator), allocator);
    }
    return value;
}

rapidjson::Value SerializeObject(std::any any_ob, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value node{ rapidjson::kObjectType };
    if (any_ob.type() == typeid(GenericDto))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(DATA_OBJECT_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeDto(std::any_cast<GenericDto>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<GenericDto>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(DATA_OBJECT_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN),
            SerializeDtoArray(std::any_cast<std::vector<GenericDto>>(any_ob), allocator), allocator);
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
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeColorRGBA(std::any_cast<ColorRGBA>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(ColorRGB))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(COLOR_RGB_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeColorRGB(std::any_cast<ColorRGB>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Vector2))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR2_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector2(std::any_cast<Vector2>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Vector3))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR3_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector3(std::any_cast<Vector3>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Vector4))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR4_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector4(std::any_cast<Vector4>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Box3))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(BOX3_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeBox3(std::any_cast<Box3>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(Matrix4))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(MATRIX4_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeMatrix4(std::any_cast<Matrix4>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<std::string>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(STRING_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeStringArray(std::any_cast<std::vector<std::string>>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<std::uint32_t>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(UINT32_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeUInt32Array(std::any_cast<std::vector<std::uint32_t>>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<float>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(FLOAT_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeFloatArray(std::any_cast<std::vector<float>>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<Vector2>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR2_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector2Array(std::any_cast<std::vector<Vector2>>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<Vector3>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR3_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector3Array(std::any_cast<std::vector<Vector3>>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<Vector4>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(VECTOR4_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeVector4Array(std::any_cast<std::vector<Vector4>>(any_ob), allocator), allocator);
    }
    else if (any_ob.type() == typeid(std::vector<Matrix4>))
    {
        node.AddMember(rapidjson::StringRef(TYPE_TOKEN), rapidjson::StringRef(MATRIX4_ARRAY_TOKEN), allocator);
        node.AddMember(rapidjson::StringRef(VALUE_TOKEN), SerializeMatrix4Array(std::any_cast<std::vector<Matrix4>>(any_ob), allocator), allocator);
    }
    return node;
}

rapidjson::Value SerializeFactoryDesc(const FactoryDesc& desc, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value{ rapidjson::kObjectType };
    value.AddMember(rapidjson::StringRef(INSTANCE_TYPE), static_cast<int>(desc.GetInstanceType()), allocator);
    value.AddMember(rapidjson::StringRef(RESOURCE_NAME), SerializeString(desc.GetResourceName(), allocator), allocator);
    value.AddMember(rapidjson::StringRef(RESOURCE_FILENAME), SerializeString(desc.GetResourceFilename(), allocator), allocator);
    value.AddMember(rapidjson::StringRef(RTTI_NAME), SerializeString(desc.GetRttiName(), allocator), allocator);
    value.AddMember(rapidjson::StringRef(PREFAB_FILENAME), SerializeString(desc.GetPrefab(), allocator), allocator);
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
rapidjson::Value SerializeColorRGBA(const ColorRGBA& color, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(color.R(), allocator);
    value.PushBack(color.G(), allocator);
    value.PushBack(color.B(), allocator);
    value.PushBack(color.A(), allocator);
    return value;
}

rapidjson::Value SerializeColorRGB(const ColorRGB& color, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(color.R(), allocator);
    value.PushBack(color.G(), allocator);
    value.PushBack(color.B(), allocator);
    return value;
}

rapidjson::Value SerializeVector2(const Vector2& vec, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(vec.X(), allocator);
    value.PushBack(vec.Y(), allocator);
    return value;
}

rapidjson::Value SerializeVector3(const Vector3& vec, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(vec.X(), allocator);
    value.PushBack(vec.Y(), allocator);
    value.PushBack(vec.Z(), allocator);
    return value;
}

rapidjson::Value SerializeVector4(const Vector4& vec, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    value.PushBack(vec.X(), allocator);
    value.PushBack(vec.Y(), allocator);
    value.PushBack(vec.Z(), allocator);
    value.PushBack(vec.W(), allocator);
    return value;
}

rapidjson::Value SerializeBox3(const Box3& box, rapidjson::MemoryPoolAllocator<>& allocator)
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

rapidjson::Value SerializeMatrix4(const Matrix4& mx, rapidjson::MemoryPoolAllocator<>& allocator)
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

rapidjson::Value SerializeStringArray(const std::vector<std::string>& ss, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto s : ss)
    {
        value.PushBack(SerializeString(s, allocator), allocator);
    }
    return value;
}

rapidjson::Value SerializeUInt32Array(const std::vector<std::uint32_t>& ns, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto n : ns)
    {
        value.PushBack(SerializeUInt32(n), allocator);
    }
    return value;
}

rapidjson::Value SerializeFloatArray(const std::vector<float>& vs, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto v : vs)
    {
        value.PushBack(SerializeFloat(v), allocator);
    }
    return value;
}

rapidjson::Value SerializeVector2Array(const std::vector<Vector2>& vecs, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto vec : vecs)
    {
        value.PushBack(SerializeVector2(vec, allocator), allocator);
    }
    return value;
}

rapidjson::Value SerializeVector3Array(const std::vector<Vector3>& vecs, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto vec : vecs)
    {
        value.PushBack(SerializeVector3(vec, allocator), allocator);
    }
    return value;
}

rapidjson::Value SerializeVector4Array(const std::vector<Vector4>& vecs, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto vec : vecs)
    {
        value.PushBack(SerializeVector4(vec, allocator), allocator);
    }
    return value;
}

rapidjson::Value SerializeMatrix4Array(const std::vector<Matrix4>& mxs, rapidjson::MemoryPoolAllocator<>& allocator)
{
    rapidjson::Value value(rapidjson::kArrayType);
    for (auto mx : mxs)
    {
        value.PushBack(SerializeMatrix4(mx, allocator), allocator);
    }
    return value;
}
