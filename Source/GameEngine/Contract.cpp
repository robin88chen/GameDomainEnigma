#include "Contract.h"

using namespace Enigma::Engine;

std::string TOKEN_RTTI = "Rtti";

Contract::Contract()
{
    m_isTopLevel = false;
    m_memoryId = reinterpret_cast<size_t>(this);
}

Contract::~Contract()
{
    m_values.clear();
}

bool Contract::operator==(const Contract& c) const
{
    return m_memoryId == c.m_memoryId;
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
