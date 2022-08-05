/*********************************************************************
 * \file   ShaderManager.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "Frameworks/SystemService.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <system_error>
#include <memory>
#include <unordered_map>

#include "Frameworks/EventSubscriber.h"

namespace Enigma::Graphics
{
    class IVertexShader;
    class IPixelShader;
}
namespace Enigma::Engine
{
    using error = std::error_code;
    struct ShaderProgramPolicy;

    class ShaderManager : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        ShaderManager();
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager(ShaderManager&&) = delete;
        virtual ~ShaderManager();
        ShaderManager& operator=(const ShaderManager&) = delete;
        ShaderManager& operator=(ShaderManager&&) = delete;

        error BuildShaderProgram(const ShaderProgramPolicy& policy);

        bool HasVertexShader(const std::string& name);
        std::shared_ptr<Graphics::IVertexShader> QueryVertexShader(const std::string& name);

        bool HasPixelShader(const std::string& name);
        std::shared_ptr<Graphics::IPixelShader> QueryPixelShader(const std::string& name);

        /** query vertex shader \n if shader already in table, return shader, otherwise create new shader */
        error QueryVertexShader(const std::string& name, std::shared_ptr<Graphics::IVertexShader>* shader);
        future_error AsyncQueryVertexShader(const std::string& name, std::shared_ptr<Graphics::IVertexShader>* shader);

        /** query pixel shader \n if shader already in table, return shader, otherwise create new shader */
        error QueryPixelShader(const std::string& name, std::shared_ptr<Graphics::IPixelShader>* shader);
        future_error AsyncQueryPixelShader(const std::string& name, std::shared_ptr<Graphics::IPixelShader>* shader);

        /** 同樣的名字的 Shader 會複製，所以改放在 multimap 裡 */
        using VertexShaderTable = std::unordered_multimap<std::string, std::weak_ptr<Graphics::IVertexShader>>;
        /** 同樣的名字的 Shader 會複製，所以改放在 multimap 裡 */
        using PixelShaderTable = std::unordered_multimap<std::string, std::weak_ptr<Graphics::IPixelShader>>;
        /** shader code file path ID */
        static const std::string& GetShaderCodePathID();

    protected:
        void OnBuilderShaderProgramBuilt(const Frameworks::IEventPtr& e);

    protected:
        Frameworks::EventSubscriberPtr m_onBuilderShaderProgramBuilt;

        VertexShaderTable m_vtxShaderTable;
        PixelShaderTable m_pixShaderTable;

        std::mutex m_vtxShaderTableLock;  ///< thread locker for vtx shader resource table
        std::mutex m_pixShaderTableLock;  ///< thread locker for pix shader resource table
    };
}


#endif // SHADER_MANAGER_H
