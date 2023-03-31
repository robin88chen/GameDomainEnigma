/*********************************************************************
 * \file   TextConverter.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _TEXT_CONVERTER_H
#define _TEXT_CONVERTER_H

#include "PlatformConfig.h"
#include <string>

namespace Enigma::Platforms
{
    /** Text Converter */
    class TextConverter
    {
    public:
#if TARGET_PLATFORM == PLATFORM_WIN32
        static void LocalCharacterToUtf16(char* local_ch, size_t byteSize, unsigned short* utf16_ch);
        static void LocalStringToUtf16(char* local_str, size_t str_len,
            unsigned short* utf16_str, size_t utf16_buff_size_in_words, size_t* ret_size);
        static void LocalStringToUtf8(char* local_str, size_t str_len, char* utf8_str, size_t utf8_buff_size);
        static std::string LocalStringToUtf8(const std::string& local_str);

        static void Utf16CharacterToLocal(unsigned short utf16_ch, char* local_ch, size_t local_buff_size);
        static void Utf16StringToLocal(unsigned short* utf16_str, size_t str_len, char* local_str, size_t local_buff_size);
        static size_t CalcLocalStringSize(unsigned short* utf16_str, size_t str_len);
        static void Utf8StringToLocal(char* utf8_str, size_t str_len, char* local_str, size_t local_str_size);
        static std::string Utf8StringToLocal(const std::string& utf8_str);
        static size_t CalcUtf8StringSize(char* local_str, size_t str_len);
#endif
        static void Utf8StringToUtf16(char* utf8_str, size_t utf8_bytes_len, unsigned short* utf16_str, size_t utf16_buff_size);
        static void Utf16StringToUtf8(unsigned short* utf16_str, size_t str_len, char* utf8_str, size_t utf8_buff_size);
        static size_t CalcUtf8StringSize(unsigned short* utf16_str, size_t str_len);
        static size_t CalcUtf16StringSize(char* utf8_str, size_t str_len);
    };
};

#endif // !_TEXT_CONVERTER_H
