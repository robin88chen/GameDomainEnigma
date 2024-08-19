#include "GenericDto.h"

using namespace Enigma::Engine;

std::string TOKEN_RTTI = "Rtti";
std::string TOKEN_TOP_LEVEL = "TopLevel";
std::string TOKEN_NAME = "Name";

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

void GenericDto::asTopLevel(bool is_top)
{
    addOrUpdate(TOKEN_TOP_LEVEL, is_top);
}

bool GenericDto::isTopLevel() const
{
    if (!hasValue(TOKEN_TOP_LEVEL)) return false;
    return get<bool>(TOKEN_TOP_LEVEL);
}
