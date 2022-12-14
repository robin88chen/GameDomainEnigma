/*********************************************************************
 * \file   unique_ptr_dynamic_cast.hpp
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _UNIQUE_PTR_DYNAMIC_CAST_HPP
#define _UNIQUE_PTR_DYNAMIC_CAST_HPP

#include <memory>

namespace stdext
{
    template<typename T, typename S>
    std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<S>&& p) noexcept
    {
        auto converted = std::unique_ptr<T>{ dynamic_cast<T*>(p.get()) };
        if (converted) {
            p.release();            // no longer owns the pointer
        }
        return converted;
    }
}

#endif // _UNIQUE_PTR_DYNAMIC_CAST_HPP
