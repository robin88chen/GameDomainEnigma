/*********************************************************************
 * \file   ExtentTypesDefine.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef EXTENT_TYPES_DEFINE_H
#define EXTENT_TYPES_DEFINE_H

#include <vector>
#include <future>
#include <type_traits>
#include <array>
#include <system_error>

using byte_buffer = std::vector<unsigned char>;
using uint_buffer = std::vector<unsigned int>;
using ushort_buffer = std::vector<unsigned short>;
using float_buffer = std::vector<float>;
using future_error = std::future<std::error_code>;

template <class T>
std::vector<typename std::enable_if<std::is_scalar<T>::value, T>::type> make_data_buffer(T* src, size_t count)
{
    std::vector<T> buffer;
    buffer.resize(count);
    memcpy(&buffer[0], src, count * sizeof(T));
    return buffer;
}

future_error make_future_err(std::error_code er);

template<typename E, class T, std::size_t N>
class enum_array : public std::array<T, N>
{
public:
    T& operator[] (E e) {
        return std::array<T, N>::operator[]((std::size_t)e);
    }

    const T& operator[] (E e) const {
        return std::array<T, N>::operator[]((std::size_t)e);
    }
};


#endif // EXTENT_TYPES_DEFINE_H
