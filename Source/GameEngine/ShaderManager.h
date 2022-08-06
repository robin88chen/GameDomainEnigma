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
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/Command.h"
#include "ShaderBuilder.h"
#include <system_error>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <queue>

#include "Frameworks/CommandSubscriber.h"

namespace Enigma::Graphics
{
    class IVertexShader;
    class IPixelShader;
}
namespace Enigma::Engine
{
    using error = std::error_code;

    class ShaderManager : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        ShaderManager(Frameworks::ServiceManager* srv_mngr);
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager(ShaderManager&&) = delete;
        virtual ~ShaderManager();
        ShaderManager& operator=(const ShaderManager&) = delete;
        ShaderManager& operator=(ShaderManager&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult OnInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult OnTick() override;
        /// On Term
        virtual Frameworks::ServiceResult OnTerm() override;

        error BuildShaderProgram(const ShaderProgramPolicy& policy);

        bool HasVertexShader(const std::string& name);
        std::shared_ptr<Graphics::IVertexShader> QueryVertexShader(const std::string& name);

        bool HasPixelShader(const std::string& name);
        std::shared_ptr<Graphics::IPixelShader> QueryPixelShader(const std::string& name);

        bool HasShaderProgram(const std::string& name);
        std::shared_ptr<Graphics::IShaderProgram> QueryShaderProgram(const std::string& name);

        using VertexShaderTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IVertexShader>>;
        using PixelShaderTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IPixelShader>>;
        using ShaderProgramTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IShaderProgram>>;

        /** shader code file path ID */
        static const std::string& GetShaderCodePathID();

    protected:
        void OnBuilderShaderProgramBuilt(const Frameworks::IEventPtr& e);
        void OnShaderProgramBuildFailed(const Frameworks::IEventPtr& e);
        void HandleBuildingShaderProgram(const Frameworks::ICommandPtr& c);

    protected:
        Frameworks::EventSubscriberPtr m_onBuilderShaderProgramBuilt;
        Frameworks::EventSubscriberPtr m_onShaderProgramBuildFailed;
        Frameworks::CommandSubscriberPtr m_handleBuildingShaderProgram;

        ShaderBuilder* m_builder;
        std::queue<ShaderProgramPolicy> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;

        VertexShaderTable m_vtxShaderTable;
        PixelShaderTable m_pixShaderTable;
        ShaderProgramTable m_programTable;

        std::recursive_mutex m_vtxShaderTableLock;  ///< thread locker for vtx shader resource table
        std::recursive_mutex m_pixShaderTableLock;  ///< thread locker for pix shader resource table
        std::recursive_mutex m_programTableLock;
    };
}


#endif // SHADER_MANAGER_H
