/*********************************************************************
 * \file   menew_make_shared.hpp
 * \brief make shared 換成 menew, 還不知道能不能用
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef MENEW_MAKE_SHARED_HPP
#define MENEW_MAKE_SHARED_HPP

#include "Platforms/MemoryMacro.h"
#include <memory>
#include <utility>

namespace stdext
{
    template<typename T, typename... Args>
    std::shared_ptr<T> make_shared(Args&&... args)
    {
        return std::shared_ptr<T>(menew T(std::forward<Args>(args)...));
    }
}

#endif // MENEW_MAKE_SHARED_HPP
