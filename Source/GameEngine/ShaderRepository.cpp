#include "ShaderRepository.h"
#include "EngineErrors.h"
#include "ShaderEvents.h"
#include "ShaderCommands.h"
#include "ShaderBuilder.h"
#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IVertexDeclaration.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, ShaderRepository);

static std::string shaderCodePathID = std::string("SHADER_CODE_PATH");

ShaderRepository::ShaderRepository(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, Engine, ShaderRepository, ISystemService);
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_builder = menew ShaderBuilder(this);
}

ShaderRepository::~ShaderRepository()
{
    SAFE_DELETE(m_builder);
}

Enigma::Frameworks::ServiceResult ShaderRepository::OnInit()
{
    m_onBuilderShaderProgramBuilt =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnBuilderShaderProgramBuilt(c); });
    Frameworks::EventPublisher::Subscribe(typeid(ShaderBuilder::ShaderProgramBuilt), m_onBuilderShaderProgramBuilt);
    m_onBuildShaderProgramFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->OnBuildShaderProgramFailed(c); });
    Frameworks::EventPublisher::Subscribe(typeid(BuildShaderProgramFailed), m_onBuildShaderProgramFailed);
    m_doBuildingShaderProgram =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBuildingShaderProgram(c); });
    Frameworks::CommandBus::Subscribe(typeid(Engine::BuildShaderProgram), m_doBuildingShaderProgram);
    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult ShaderRepository::OnTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_builder);
    m_builder->BuildShaderProgram(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult ShaderRepository::OnTerm()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(ShaderBuilder::ShaderProgramBuilt), m_onBuilderShaderProgramBuilt);
    m_onBuilderShaderProgramBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(BuildShaderProgramFailed), m_onBuildShaderProgramFailed);
    m_onBuildShaderProgramFailed = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Engine::BuildShaderProgram), m_doBuildingShaderProgram);
    m_doBuildingShaderProgram = nullptr;

    return Frameworks::ServiceResult::Complete;
}

error ShaderRepository::BuildShaderProgram(const ShaderProgramPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

bool ShaderRepository::HasVertexShader(const std::string& name)
{
    std::lock_guard locker{ m_vtxShaderTableLock };
    auto it = m_vtxShaderTable.find(name);
    return ((it != m_vtxShaderTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IVertexShaderPtr ShaderRepository::QueryVertexShader(const std::string& name)
{
    std::lock_guard locker{ m_vtxShaderTableLock };
    auto it = m_vtxShaderTable.find(name);
    if (it == m_vtxShaderTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

bool ShaderRepository::HasVertexLayout(const std::string& name)
{
    std::lock_guard locker{ m_vtxLayoutTableLock };
    auto it = m_vtxLayoutTable.find(name);
    return ((it != m_vtxLayoutTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IVertexDeclarationPtr ShaderRepository::QueryVertexLayout(const std::string& name)
{
    std::lock_guard locker{ m_vtxLayoutTableLock };
    auto it = m_vtxLayoutTable.find(name);
    if (it == m_vtxLayoutTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

bool ShaderRepository::HasPixelShader(const std::string& name)
{
    std::lock_guard locker{ m_pixShaderTableLock };
    auto it = m_pixShaderTable.find(name);
    return ((it != m_pixShaderTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IPixelShaderPtr ShaderRepository::QueryPixelShader(const std::string& name)
{
    std::lock_guard locker{ m_pixShaderTableLock };
    auto it = m_pixShaderTable.find(name);
    if (it == m_pixShaderTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

bool ShaderRepository::HasShaderProgram(const std::string& name)
{
    std::lock_guard locker{ m_programTableLock };
    auto it = m_programTable.find(name);
    return ((it != m_programTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IShaderProgramPtr ShaderRepository::QueryShaderProgram(const std::string& name)
{
    std::lock_guard locker{ m_programTableLock };
    auto it = m_programTable.find(name);
    if (it == m_programTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

const std::string& ShaderRepository::GetShaderCodePathID()
{
    return shaderCodePathID;
}

void ShaderRepository::OnBuilderShaderProgramBuilt(const Frameworks::IEventPtr& e)
{
    assert(m_builder);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<ShaderBuilder::ShaderProgramBuilt, Frameworks::IEvent>(e);
    if (!ev) return;

    Graphics::IShaderProgramPtr program = m_builder->GetProgram();
    if (!program)
    {
        Platforms::Debug::ErrorPrintf("get null program on builder shader program built!!\n");
        return;
    }
    {
        std::lock_guard locker{ m_vtxShaderTableLock };
        m_vtxShaderTable.try_emplace(program->GetVertexShader()->GetName(), program->GetVertexShader());
    }
    {
        std::lock_guard locker{ m_vtxLayoutTableLock };
        m_vtxLayoutTable.try_emplace(program->GetVertexDeclaration()->GetName(), program->GetVertexDeclaration());
    }
    {
        std::lock_guard locker{ m_pixShaderTableLock };
        m_pixShaderTable.try_emplace(program->GetPixelShader()->GetName(), program->GetPixelShader());
    }
    {
        std::lock_guard locker{ m_programTableLock };
        m_programTable.try_emplace(program->GetName(), program);
    }
    Frameworks::EventPublisher::Post(std::make_shared<ShaderProgramBuilt>(program->GetName(), program));
    m_isCurrentBuilding = false;
}

void ShaderRepository::OnBuildShaderProgramFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildShaderProgramFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("shader program %s build failed : %s\n",
        ev->GetShaderName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}
void ShaderRepository::DoBuildingShaderProgram(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Engine::BuildShaderProgram, Frameworks::ICommand>(c);
    if (!cmd) return;
    BuildShaderProgram(cmd->GetPolicy());
}
