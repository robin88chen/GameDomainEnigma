/*********************************************************************
 * \file   ShaderRepository.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef SHADER_REPOSITORY_H
#define SHADER_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/Command.h"
#include "Frameworks/CommandSubscriber.h"
#include "ShaderBuildingPolicies.h"
#include <system_error>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <queue>

namespace Enigma::Graphics
{
    class IVertexShader;
    class IPixelShader;
    class IShaderProgram;
    class IVertexDeclaration;
}
namespace Enigma::Engine
{
    using error = std::error_code;

    class ShaderBuilder;

    class ShaderRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        ShaderRepository(Frameworks::ServiceManager* srv_mngr);
        ShaderRepository(const ShaderRepository&) = delete;
        ShaderRepository(ShaderRepository&&) = delete;
        virtual ~ShaderRepository();
        ShaderRepository& operator=(const ShaderRepository&) = delete;
        ShaderRepository& operator=(ShaderRepository&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult OnInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult OnTick() override;
        /// On Term
        virtual Frameworks::ServiceResult OnTerm() override;

        error BuildShaderProgram(const ShaderProgramPolicy& policy);

        bool HasVertexShader(const std::string& name);
        std::shared_ptr<Graphics::IVertexShader> QueryVertexShader(const std::string& name);

        bool HasVertexLayout(const std::string& name);
        std::shared_ptr<Graphics::IVertexDeclaration> QueryVertexLayout(const std::string& name);

        bool HasPixelShader(const std::string& name);
        std::shared_ptr<Graphics::IPixelShader> QueryPixelShader(const std::string& name);

        bool HasShaderProgram(const std::string& name);
        std::shared_ptr<Graphics::IShaderProgram> QueryShaderProgram(const std::string& name);

        using VertexShaderTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IVertexShader>>;
        using VertexLayoutTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IVertexDeclaration>>;
        using PixelShaderTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IPixelShader>>;
        using ShaderProgramTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IShaderProgram>>;

        /** shader code file path ID */
        static const std::string& GetShaderCodePathID();

    protected:
        void OnBuilderShaderProgramBuilt(const Frameworks::IEventPtr& e);
        void OnBuildShaderProgramFailed(const Frameworks::IEventPtr& e);
        void DoBuildingShaderProgram(const Frameworks::ICommandPtr& c);

    protected:
        Frameworks::EventSubscriberPtr m_onBuilderShaderProgramBuilt;
        Frameworks::EventSubscriberPtr m_onBuildShaderProgramFailed;
        Frameworks::CommandSubscriberPtr m_doBuildingShaderProgram;

        ShaderBuilder* m_builder;
        std::queue<ShaderProgramPolicy> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;

        VertexShaderTable m_vtxShaderTable;
        VertexLayoutTable m_vtxLayoutTable;
        PixelShaderTable m_pixShaderTable;
        ShaderProgramTable m_programTable;

        std::recursive_mutex m_vtxShaderTableLock;
        std::recursive_mutex m_vtxLayoutTableLock;
        std::recursive_mutex m_pixShaderTableLock;
        std::recursive_mutex m_programTableLock;
    };
}


#endif // SHADER_REPOSITORY_H
