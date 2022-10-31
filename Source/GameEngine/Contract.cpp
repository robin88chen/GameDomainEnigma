#include "Contract.h"

using namespace Enigma::Engine;

std::string TOKEN_RTTI = "Rtti";
std::string TOKEN_TOP_LEVEL = "TopLevel";

Contract::Contract()
{
    m_ruid = Platforms::Ruid::Generate();
}

Contract::~Contract()
{
    m_values.clear();
}

bool Contract::operator==(const Contract& c) const
{
    return m_ruid == c.m_ruid;
}

bool Contract::HasValue(const std::string& attribute) const
{
    return m_values.find(attribute) != m_values.end();
}

void Contract::Remove(const std::string& attribute)
{
    if (!HasValue(attribute)) return;
    m_values.erase(attribute);
}

void Contract::AddRtti(const FactoryDesc& rtti)
{
    AddOrUpdate(TOKEN_RTTI, rtti);
}

FactoryDesc Contract::GetRtti() const
{
    return Get<FactoryDesc>(TOKEN_RTTI);
}

void Contract::AsTopLevel(bool is_top)
{
    AddOrUpdate(TOKEN_TOP_LEVEL, is_top);
}

bool Contract::IsTopLevel() const
{
    if (!HasValue(TOKEN_TOP_LEVEL)) return false;
    return Get<bool>(TOKEN_TOP_LEVEL);
}
