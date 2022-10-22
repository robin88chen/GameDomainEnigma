#include "Contract.h"

using namespace Enigma::Frameworks;

Contract::Contract()
{
}

Contract::~Contract()
{
    m_values.clear();
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
