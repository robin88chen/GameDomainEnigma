#include "GenericDto.h"

using namespace Enigma::Engine;

std::string TOKEN_RTTI = "Rtti";
std::string TOKEN_TOP_LEVEL = "TopLevel";
std::string TOKEN_NAME = "Name";

GenericDto::GenericDto()
{
    m_ruid = Frameworks::Ruid::Generate();
}

GenericDto::~GenericDto()
{
    m_values.clear();
}

bool GenericDto::operator==(const GenericDto& c) const
{
    return m_ruid == c.m_ruid;
}

bool GenericDto::HasValue(const std::string& attribute) const
{
    return m_values.find(attribute) != m_values.end();
}

void GenericDto::Remove(const std::string& attribute)
{
    if (!HasValue(attribute)) return;
    m_values.erase(attribute);
}

void GenericDto::AddRtti(const FactoryDesc& rtti)
{
    AddOrUpdate(TOKEN_RTTI, rtti);
}

FactoryDesc GenericDto::GetRtti() const
{
    return Get<FactoryDesc>(TOKEN_RTTI);
}

void GenericDto::AddName(const std::string& name)
{
   AddOrUpdate(TOKEN_NAME, name);
}

std::string GenericDto::GetName() const
{
   return Get<std::string>(TOKEN_NAME);
}

void GenericDto::AsTopLevel(bool is_top)
{
    AddOrUpdate(TOKEN_TOP_LEVEL, is_top);
}

bool GenericDto::IsTopLevel() const
{
    if (!HasValue(TOKEN_TOP_LEVEL)) return false;
    return Get<bool>(TOKEN_TOP_LEVEL);
}
