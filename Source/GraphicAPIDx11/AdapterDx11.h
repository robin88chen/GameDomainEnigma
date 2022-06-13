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
#include "MemoryMacro.h"

namespace Enigma::Graphics
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

            OutputInfo() : m_adapterOrdinal(0), m_outputOrdinal(0), m_dxgiOutput(nullptr), m_uniqueDescription(nullptr) {};
            ~OutputInfo() { SAFE_RELEASE(m_dxgiOutput); SAFE_DELETE_ARRAY(m_uniqueDescription); };
        };

        struct AdapterInfo
        {
            IDXGIAdapter1* m_dxgiAdapter;
            unsigned int m_adapterOrdinal;
            DXGI_ADAPTER_DESC1 m_desc;
            char* m_uniqueDescription;
            std::vector<DeviceInfo*> m_deviceInfoList;
            std::vector<OutputInfo*> m_outputInfoList;

            AdapterInfo() : m_dxgiAdapter(nullptr), m_adapterOrdinal(0), m_uniqueDescription(nullptr) {};
            ~AdapterInfo()
            {
                ClearList();
                SAFE_DELETE_ARRAY(m_uniqueDescription);
                SAFE_RELEASE(m_dxgiAdapter);
            }
            void ClearList()
            {
                if (m_deviceInfoList.size())
                {
                    for (unsigned int i = 0; i < (unsigned int)m_deviceInfoList.size(); i++)
                    {
                        SAFE_DELETE(m_deviceInfoList[i]);
                    }
                    m_deviceInfoList.clear();
                }
                if (m_outputInfoList.size())
                {
                    for (unsigned int i = 0; i < (unsigned int)m_outputInfoList.size(); i++)
                    {
                        SAFE_DELETE(m_outputInfoList[i]);
                    }
                    m_outputInfoList.clear();
                }
            }
        };

    public:
        AdapterDx11();
        AdapterDx11(const AdapterDx11&) = delete;
        ~AdapterDx11();
        AdapterDx11& operator=(const AdapterDx11&) = delete;

        unsigned int GetAdapterCount() { return (unsigned int)m_adapterInfoList.size(); };
        unsigned int GetCurrentAdapterIndex() { return m_currentAdapterIndex; };
        void SelectAdapter(unsigned int index);
        AdapterInfo* GetSelectedAdapterInfo();

    protected:
        friend DeviceCreatorDx11;

        std::vector<AdapterInfo*> m_adapterInfoList;
        unsigned int m_currentAdapterIndex;
    };

};


#endif // ADAPTER_DX11_H
