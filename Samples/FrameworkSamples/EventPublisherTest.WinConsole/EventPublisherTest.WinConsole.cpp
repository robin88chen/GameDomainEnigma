// EventPublisherTest.WinConsole.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include "Event.h"
#include "MemoryAllocMacro.h"
#include "EventPublisher.h"
#include "EventSubscriber.h"
#include "ServiceManager.h"

using namespace Enigma::Frameworks;

static bool endLoop = false;
class FakeTestEvent : public IEvent
{
public:
    FakeTestEvent(uint32_t v) { m_value = v; }
    //virtual const std::type_info& TypeInfo() { return typeid(*this); }
    uint32_t m_value;
};

void FakeMessageHandler(const IEventPtr& ev)
{
    if (!ev)
    {
        std::cout << "Fake Message is null" << std::endl;
    }
    else
    {
        FakeTestEvent* fm = dynamic_cast<FakeTestEvent*>(ev.get());
        std::cout << "Fake message with value " << fm->m_value << std::endl;
        if (fm->m_value == 7) endLoop = true;
        EventPublisher::Post(IEventPtr{ menew FakeTestEvent{ 100 + fm->m_value } });
    }

}

class TestSubscriber
{
public:
    void TestHandler(const IEventPtr& ev)
    {
        if (!ev)
        {
            std::cout << "Test Subscriber: " << "Fake Message is null" << std::endl;
        }
        else
        {
            FakeTestEvent* fm = dynamic_cast<FakeTestEvent*>(ev.get());
            std::cout << "Test Subscriber: " << "Fake message with value " << fm->m_value << std::endl;
            if (fm->m_value == 7) endLoop = true;
            EventPublisher::Post(IEventPtr{ menew FakeTestEvent{ 100 + fm->m_value } });
        }

    }

};
int main()
{
    std::cout << "Hello World!\n";

    ServiceManager* service = new ServiceManager{};
    service->RegisterSystemService(new EventPublisher{ service });

    TestSubscriber* sub = new TestSubscriber;
    EventSubscriberPtr on_fake_msg = std::make_shared<EventSubscriber>(FakeMessageHandler);
    EventSubscriberPtr on_sub_event = std::make_shared<EventSubscriber>([=](auto e) {sub->TestHandler(e); });
    EventPublisher::Subscribe(typeid(FakeTestEvent), on_fake_msg);
    EventPublisher::Subscribe(typeid(FakeTestEvent), on_sub_event);
    EventPublisher::Unsubscribe(typeid(FakeTestEvent), on_fake_msg);
    EventPublisher::Unsubscribe(typeid(FakeTestEvent), on_sub_event);

    EventPublisher::Subscribe(typeid(FakeTestEvent), on_fake_msg);
    EventPublisher::Subscribe(typeid(FakeTestEvent), on_sub_event);

    EventPublisher::Send(IEventPtr{ menew FakeTestEvent{ 1 } });
    EventPublisher::Post(IEventPtr{ menew FakeTestEvent{ 3 } });
    EventPublisher::Post(IEventPtr{ menew FakeTestEvent{ 8 } });
    EventPublisher::Post(IEventPtr{ menew FakeTestEvent{ 7 } });
    EventPublisher::Post(IEventPtr{ menew FakeTestEvent{ 10} });

    while (!endLoop)
    {
        service->RunOnce();
    }

    // service 直接 shutdown
    //service->ShutdownSystemService(MessageService::TYPE_RTTI);
    // or
    // unregister 後還要再 run幾次才會 shutdown, 
    service->UnregisterSystemService(EventPublisher::TYPE_RTTI);  // state = waiting shutdown
    service->RunOnce(); // state = shuting down
    service->RunOnce(); // state = completed
    service->RunOnce(); // state = deleted & service is deleted
    delete service;
    delete sub;
    //delete MemoryManager::Instance();
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
