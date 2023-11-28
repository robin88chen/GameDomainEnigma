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
        None = 0x00,
        Read = 0x01,
        Write = 0x02,
        OpenAlways = 0x04,
        Binary = 0x08,
    };
    constexpr ReadWriteOption ReadWriteOptionNone = None;
    constexpr ReadWriteOption ReadWriteOptionRead = Read;
    constexpr ReadWriteOption ReadWriteOptionWrite = Write;
    constexpr ReadWriteOption ReadWriteOptionOpenAlways = OpenAlways;
    constexpr ReadWriteOption ReadWriteOptionBinary = Binary;
}

#endif // READ_WRITE_OPTION_H
