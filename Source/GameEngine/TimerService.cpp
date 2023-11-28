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
    if (m_gameTimer) m_gameTimer->reset();
    if (m_realLifeTimer) m_realLifeTimer->reset();

    return ServiceResult::Complete;
}

ServiceResult TimerService::onTick()
{
    if (m_gameTimer) m_gameTimer->update();
    if (m_realLifeTimer) m_realLifeTimer->update();

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
    if (m_gameTimer) m_gameTimer->setScale(scale);
}

void TimerService::PauseGameTimer()
{
    if (m_gameTimer) m_gameTimer->pause();
}

void TimerService::ResumeGameTimer()
{
    if (m_gameTimer) m_gameTimer->resume();
}

void TimerService::ReverseGameTimer()
{
    if (m_gameTimer) m_gameTimer->setScale(-m_gameTimer->getScale());
}

void TimerService::SetGameTimerStep(bool enable, float step)
{
    if (m_gameTimer) m_gameTimer->setFrameStep(enable, step);
}
