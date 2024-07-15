﻿#include "ShaderRepository.h"
#include "EngineErrors.h"
#include "ShaderEvents.h"
#include "ShaderCommands.h"
#include "ShaderBuilder.h"
#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IVertexDeclaration.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI(Engine, ShaderRepository, ISystemService);

static std::string shaderCodePathID = std::string("SHADER_CODE_PATH");

ShaderRepository::ShaderRepository(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_isCurrentBuilding = false;
    m_builder = menew ShaderBuilder(this);
}

ShaderRepository::~ShaderRepository()
{
    SAFE_DELETE(m_builder);
}

Enigma::Frameworks::ServiceResult ShaderRepository::onInit()
{
    m_onBuilderShaderProgramBuilt =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->onBuilderShaderProgramBuilt(c); });
    Frameworks::EventPublisher::subscribe(typeid(ShaderBuilder::ShaderProgramBuilt), m_onBuilderShaderProgramBuilt);
    m_onBuildShaderProgramFailed =
        std::make_shared<Frameworks::EventSubscriber>([=](auto c) { this->onBuildShaderProgramFailed(c); });
    Frameworks::EventPublisher::subscribe(typeid(BuildShaderProgramFailed), m_onBuildShaderProgramFailed);
    m_buildShaderProgram =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->buildShaderProgram(c); });
    Frameworks::CommandBus::subscribe(typeid(Engine::BuildShaderProgram), m_buildShaderProgram);
    return Frameworks::ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult ShaderRepository::onTick()
{
    if (m_isCurrentBuilding) return Frameworks::ServiceResult::Pendding;
    std::lock_guard locker{ m_policiesLock };
    if (m_policies.empty())
    {
        m_needTick = false;
        return Frameworks::ServiceResult::Pendding;
    }
    assert(m_builder);
    m_builder->buildShaderProgram(m_policies.front());
    m_policies.pop();
    m_isCurrentBuilding = true;
    return Frameworks::ServiceResult::Pendding;
}

Enigma::Frameworks::ServiceResult ShaderRepository::onTerm()
{
    Frameworks::EventPublisher::unsubscribe(typeid(ShaderBuilder::ShaderProgramBuilt), m_onBuilderShaderProgramBuilt);
    m_onBuilderShaderProgramBuilt = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(BuildShaderProgramFailed), m_onBuildShaderProgramFailed);
    m_onBuildShaderProgramFailed = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Engine::BuildShaderProgram), m_buildShaderProgram);
    m_buildShaderProgram = nullptr;

    return Frameworks::ServiceResult::Complete;
}

error ShaderRepository::buildShaderProgram(const ShaderProgramPolicy& policy)
{
    std::lock_guard locker{ m_policiesLock };
    m_policies.push(policy);
    m_needTick = true;
    return ErrorCode::ok;
}

bool ShaderRepository::hasVertexShader(const std::string& name)
{
    std::lock_guard locker{ m_vtxShaderTableLock };
    auto it = m_vtxShaderTable.find(name);
    return ((it != m_vtxShaderTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IVertexShaderPtr ShaderRepository::queryVertexShader(const std::string& name)
{
    std::lock_guard locker{ m_vtxShaderTableLock };
    auto it = m_vtxShaderTable.find(name);
    if (it == m_vtxShaderTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

bool ShaderRepository::hasVertexLayout(const std::string& name)
{
    std::lock_guard locker{ m_vtxLayoutTableLock };
    auto it = m_vtxLayoutTable.find(name);
    return ((it != m_vtxLayoutTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IVertexDeclarationPtr ShaderRepository::queryVertexLayout(const std::string& name)
{
    std::lock_guard locker{ m_vtxLayoutTableLock };
    auto it = m_vtxLayoutTable.find(name);
    if (it == m_vtxLayoutTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

bool ShaderRepository::hasPixelShader(const std::string& name)
{
    std::lock_guard locker{ m_pixShaderTableLock };
    auto it = m_pixShaderTable.find(name);
    return ((it != m_pixShaderTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IPixelShaderPtr ShaderRepository::queryPixelShader(const std::string& name)
{
    std::lock_guard locker{ m_pixShaderTableLock };
    auto it = m_pixShaderTable.find(name);
    if (it == m_pixShaderTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

bool ShaderRepository::hasShaderProgram(const std::string& name)
{
    std::lock_guard locker{ m_programTableLock };
    auto it = m_programTable.find(name);
    return ((it != m_programTable.end()) && (!it->second.expired()));
}

Enigma::Graphics::IShaderProgramPtr ShaderRepository::queryShaderProgram(const std::string& name)
{
    std::lock_guard locker{ m_programTableLock };
    auto it = m_programTable.find(name);
    if (it == m_programTable.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

const std::string& ShaderRepository::getShaderCodePathID()
{
    return shaderCodePathID;
}

void ShaderRepository::onBuilderShaderProgramBuilt(const Frameworks::IEventPtr& e)
{
    assert(m_builder);
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<ShaderBuilder::ShaderProgramBuilt, Frameworks::IEvent>(e);
    if (!ev) return;

    Graphics::IShaderProgramPtr program = m_builder->getProgram();
    if (!program)
    {
        Platforms::Debug::ErrorPrintf("get null program on builder shader program built!!\n");
        return;
    }
    {
        std::lock_guard locker{ m_vtxShaderTableLock };
        m_vtxShaderTable.insert_or_assign(program->GetVertexShader()->getName(), program->GetVertexShader());
    }
    {
        std::lock_guard locker{ m_vtxLayoutTableLock };
        m_vtxLayoutTable.insert_or_assign(program->GetVertexDeclaration()->getName(), program->GetVertexDeclaration());
    }
    {
        std::lock_guard locker{ m_pixShaderTableLock };
        m_pixShaderTable.insert_or_assign(program->GetPixelShader()->getName(), program->GetPixelShader());
    }
    {
        std::lock_guard locker{ m_programTableLock };
        m_programTable.insert_or_assign(program->getName(), program);
    }
    Frameworks::EventPublisher::enqueue(std::make_shared<ShaderProgramBuilt>(program->getName(), program));
    m_isCurrentBuilding = false;
}

void ShaderRepository::onBuildShaderProgramFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildShaderProgramFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Platforms::Debug::ErrorPrintf("shader program %s build failed : %s\n",
        ev->GetShaderName().c_str(), ev->GetErrorCode().message().c_str());
    m_isCurrentBuilding = false;
}
void ShaderRepository::buildShaderProgram(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Engine::BuildShaderProgram, Frameworks::ICommand>(c);
    if (!cmd) return;
    buildShaderProgram(cmd->GetPolicy());
}
