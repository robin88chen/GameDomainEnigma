/*********************************************************************
 * \file   ReadWriteOption.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef READ_WRITE_OPTION_H
#define READ_WRITE_OPTION_H

#include <bitset>

namespace Enigma::FileSystem
{
    using ReadWriteOption = std::bitset<8>;
    enum ReadWriteOptionBits
    {
        none = 0x00,
        read = 0x01,
        write = 0x02,
        openAlways = 0x04,
        binary = 0x08,
    };
    constexpr ReadWriteOption ReadWriteOptionNone = none;
    constexpr ReadWriteOption ReadWriteOptionRead = read;
    constexpr ReadWriteOption ReadWriteOptionWrite = write;
    constexpr ReadWriteOption ReadWriteOptionOpenAlways = openAlways;
    constexpr ReadWriteOption ReadWriteOptionBinary = binary;
}

#endif // READ_WRITE_OPTION_H
