#include "AdapterDx11.h"
#include <cassert>

using namespace Enigma::Devices;

AdapterDx11::AdapterDx11()
{
    m_currentAdapterIndex = 0;
}

AdapterDx11::~AdapterDx11()
{
    if (m_adapterInfoList.size())
    {
        for (unsigned int i = 0; i < m_adapterInfoList.size(); i++)
        {
            SAFE_DELETE(m_adapterInfoList[i]);
        }
    }
}

void AdapterDx11::SelectAdapter(unsigned int index)
{
    assert(index < m_adapterInfoList.size());
    m_currentAdapterIndex = index;
}

AdapterDx11::AdapterInfo* AdapterDx11::GetSelectedAdapterInfo()
{
    assert(m_currentAdapterIndex < m_adapterInfoList.size());
    return m_adapterInfoList[m_currentAdapterIndex];
}
