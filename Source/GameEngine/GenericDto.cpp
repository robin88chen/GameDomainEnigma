#include "GenericDto.h"
#include "GenericPolicy.h"

using namespace Enigma::Engine;

std::string TOKEN_RTTI = "Rtti";
std::string TOKEN_TOP_LEVEL = "TopLevel";
std::string TOKEN_NAME = "Name";

std::unordered_map<std::string, GenericPolicyConverter> GenericDto::m_converters;

GenericDto::GenericDto() : m_ruid(Frameworks::Ruid::generate())
{
}

GenericDto::~GenericDto()
{
    m_values.clear();
}

bool GenericDto::operator==(const GenericDto& c) const
{
    return m_ruid == c.m_ruid;
}

bool GenericDto::hasValue(const std::string& attribute) const
{
    return m_values.find(attribute) != m_values.end();
}

void GenericDto::remove(const std::string& attribute)
{
    if (!hasValue(attribute)) return;
    m_values.erase(attribute);
}

void GenericDto::addRtti(const FactoryDesc& rtti)
{
    addOrUpdate(TOKEN_RTTI, rtti);
}

FactoryDesc GenericDto::getRtti() const
{
    return get<FactoryDesc>(TOKEN_RTTI);
}

void GenericDto::addName(const std::string& name)
{
   addOrUpdate(TOKEN_NAME, name);
}

std::string GenericDto::getName() const
{
   return get<std::string>(TOKEN_NAME);
}

/*void GenericDto::SetPolicyConverter(GenericPolicyConverter converter)
{
    m_converter = converter;
}

GenericPolicyConverter GenericDto::GetPolicyConverter() const
{
    return m_converter;
}*/

void GenericDto::asTopLevel(bool is_top)
{
    addOrUpdate(TOKEN_TOP_LEVEL, is_top);
}

bool GenericDto::isTopLevel() const
{
    if (!hasValue(TOKEN_TOP_LEVEL)) return false;
    return get<bool>(TOKEN_TOP_LEVEL);
}

std::shared_ptr<GenericPolicy> GenericDto::convertToPolicy(const std::shared_ptr<IDtoDeserializer>& d) const
{
    if (!hasValue(TOKEN_RTTI)) return nullptr;
    auto it = m_converters.find(getRtti().GetRttiName());
    if (it == m_converters.end()) return nullptr;
    if (it->second) return it->second(*this, d);
    return nullptr;
}

void GenericDto::registerConverter(const std::string& rtti, const GenericPolicyConverter& converter)
{
    m_converters.insert_or_assign(rtti, converter);
}

void GenericDto::unregisterConverter(const std::string& rtti)
{
    m_converters.erase(rtti);
}
