/*********************************************************************
 * \file   AdapterDx11.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef ADAPTER_DX11_H
#define ADAPTER_DX11_H

#include <vector>
#include <DXGI.h>
#include <D3D11.h>
#include "Platforms/MemoryMacro.h"

namespace Enigma::Devices
{
    class DeviceCreatorDx11;

    class AdapterDx11
    {
    public:
        struct DeviceInfo
        {
            unsigned int m_adapterOrdinal;
            D3D_DRIVER_TYPE m_d3dDriverType;
        };

        struct OutputInfo
        {
            unsigned int m_adapterOrdinal;
            unsigned int m_outputOrdinal;
            IDXGIOutput* m_dxgiOutput;
            DXGI_OUTPUT_DESC m_desc;
            char* m_uniqueDescription;
            std::vector<DXGI_MODE_DESC> m_displayModeDescList;

            OutputInfo() : m_adapterOrdinal(0), m_outputOrdinal(0), m_dxgiOutput(nullptr), m_uniqueDescription(nullptr) {}
            OutputInfo(const OutputInfo&) = delete;
            OutputInfo(OutputInfo&&) = delete;
            ~OutputInfo() { SAFE_RELEASE(m_dxgiOutput); SAFE_DELETE_ARRAY(m_uniqueDescription); }
            OutputInfo& operator=(const OutputInfo&) = delete;
            OutputInfo& operator=(OutputInfo&&) = delete;
        };

        struct AdapterInfo
        {
            IDXGIAdapter1* m_dxgiAdapter;
            unsigned int m_adapterOrdinal;
            DXGI_ADAPTER_DESC1 m_desc;
            char* m_uniqueDescription;
            std::vector<DeviceInfo*> m_deviceInfoList;
            std::vector<OutputInfo*> m_outputInfoList;

            AdapterInfo() : m_dxgiAdapter(nullptr), m_adapterOrdinal(0), m_uniqueDescription(nullptr) {}
            AdapterInfo(const AdapterInfo&) = delete;
            AdapterInfo(AdapterInfo&&) = delete;
            ~AdapterInfo()
            {
                clearList();
                SAFE_DELETE_ARRAY(m_uniqueDescription);
                SAFE_RELEASE(m_dxgiAdapter);
            }
            AdapterInfo& operator=(const AdapterInfo&) = delete;
            AdapterInfo& operator=(AdapterInfo&&) = delete;
            void clearList()
            {
                if (!m_deviceInfoList.empty())
                {
                    for (auto& i : m_deviceInfoList)
                    {
                        SAFE_DELETE(i);
                    }
                    m_deviceInfoList.clear();
                }
                if (!m_outputInfoList.empty())
                {
                    for (auto& i : m_outputInfoList)
                    {
                        SAFE_DELETE(i);
                    }
                    m_outputInfoList.clear();
                }
            }
        };

    public:
        AdapterDx11();
        AdapterDx11(const AdapterDx11&) = delete;
        AdapterDx11(AdapterDx11&&) = delete;
        ~AdapterDx11();
        AdapterDx11& operator=(const AdapterDx11&) = delete;
        AdapterDx11& operator=(AdapterDx11&&) = delete;

        unsigned int GetAdapterCount() { return static_cast<unsigned int>(m_adapterInfoList.size()); }
        unsigned int GetCurrentAdapterIndex() { return m_currentAdapterIndex; }
        void SelectAdapter(unsigned int index);
        AdapterInfo* GetSelectedAdapterInfo();

    protected:
        friend DeviceCreatorDx11;

        std::vector<AdapterInfo*> m_adapterInfoList;
        unsigned int m_currentAdapterIndex;
    };

};


#endif // ADAPTER_DX11_H
