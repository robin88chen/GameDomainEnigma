#include "TimerService.h"
#include "Frameworks/Timer.h"

using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(Engine, TimerService, ISystemService);

TimerService::TimerService(ServiceManager* manager) : ISystemService(manager)
{
    m_needTick = true;
    m_gameTimer = std::make_unique<Timer>();
    m_realLifeTimer = std::make_unique<Timer>();
}

TimerService::~TimerService()
{
    m_gameTimer = nullptr;
    m_realLifeTimer = nullptr;
}

ServiceResult TimerService::onInit()
{
    if (m_gameTimer) m_gameTimer->Reset();
    if (m_realLifeTimer) m_realLifeTimer->Reset();

    return ServiceResult::Complete;
}

ServiceResult TimerService::onTick()
{
    if (m_gameTimer) m_gameTimer->Update();
    if (m_realLifeTimer) m_realLifeTimer->Update();

    return ServiceResult::Pendding;
}

ServiceResult TimerService::onTerm()
{
    m_gameTimer = nullptr;
    m_realLifeTimer = nullptr;

    return ServiceResult::Complete;
}

void TimerService::SetGameTimerScale(float scale)
{
    if (m_gameTimer) m_gameTimer->SetScale(scale);
}

void TimerService::PauseGameTimer()
{
    if (m_gameTimer) m_gameTimer->Pause();
}

void TimerService::ResumeGameTimer()
{
    if (m_gameTimer) m_gameTimer->Resume();
}

void TimerService::ReverseGameTimer()
{
    if (m_gameTimer) m_gameTimer->SetScale(-m_gameTimer->GetScale());
}

void TimerService::SetGameTimerStep(bool enable, float step)
{
    if (m_gameTimer) m_gameTimer->SetFrameStep(enable, step);
}
