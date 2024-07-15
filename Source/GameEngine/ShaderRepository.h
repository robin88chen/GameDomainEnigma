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

    //! ADR : 沒有StoreMapper, 只是可以重複使用 shader 資源
    class ShaderRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        ShaderRepository(Frameworks::ServiceManager* srv_mngr);
        ShaderRepository(const ShaderRepository&) = delete;
        ShaderRepository(ShaderRepository&&) = delete;
        virtual ~ShaderRepository() override;
        ShaderRepository& operator=(const ShaderRepository&) = delete;
        ShaderRepository& operator=(ShaderRepository&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult onInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult onTick() override;
        /// On Term
        virtual Frameworks::ServiceResult onTerm() override;

        error buildShaderProgram(const ShaderProgramPolicy& policy);

        bool hasVertexShader(const std::string& name);
        std::shared_ptr<Graphics::IVertexShader> queryVertexShader(const std::string& name);

        bool hasVertexLayout(const std::string& name);
        std::shared_ptr<Graphics::IVertexDeclaration> queryVertexLayout(const std::string& name);

        bool hasPixelShader(const std::string& name);
        std::shared_ptr<Graphics::IPixelShader> queryPixelShader(const std::string& name);

        bool hasShaderProgram(const std::string& name);
        std::shared_ptr<Graphics::IShaderProgram> queryShaderProgram(const std::string& name);

        using VertexShaderTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IVertexShader>>;
        using VertexLayoutTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IVertexDeclaration>>;
        using PixelShaderTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IPixelShader>>;
        using ShaderProgramTable = std::unordered_map<std::string, std::weak_ptr<Graphics::IShaderProgram>>;

        /** shader code file path ID */
        static const std::string& getShaderCodePathID();

    protected:
        void onBuilderShaderProgramBuilt(const Frameworks::IEventPtr& e);
        void onBuildShaderProgramFailed(const Frameworks::IEventPtr& e);
        void buildShaderProgram(const Frameworks::ICommandPtr& c);

    protected:
        Frameworks::EventSubscriberPtr m_onBuilderShaderProgramBuilt;
        Frameworks::EventSubscriberPtr m_onBuildShaderProgramFailed;
        Frameworks::CommandSubscriberPtr m_buildShaderProgram;

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
