/*********************************************************************
 * \file   optional_ref.hpp
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef OPTIONAL_REF_HPP
#define OPTIONAL_REF_HPP

#include <optional>

namespace stdext
{
    template <typename T> using optional_ref = std::optional<std::reference_wrapper<T>>;
}

#endif // OPTIONAL_REF_HPP
