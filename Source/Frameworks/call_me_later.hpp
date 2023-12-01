/*********************************************************************
 * \file   call_me_later.hpp
 * \brief  https://stackoverflow.com/questions/14650885/how-to-create-timer-events-using-c-11
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef CALL_ME_LATER_HPP
#define CALL_ME_LATER_HPP

#include <functional>
#include <chrono>
#include <future>

namespace stdext
{
    class call_me_later
    {
    public:
        template <class callable, class... arguments>
        call_me_later(int after, bool async, callable&& f, arguments&&... args)
        {
            std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

            if (async)
            {
                std::thread([after, task]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(after));
                    task();
                    }).detach();
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
            }
        }

    };
}

#endif // CALL_ME_LATER_HPP
