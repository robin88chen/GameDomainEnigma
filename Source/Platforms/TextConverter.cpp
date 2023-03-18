// 2017.2.14 : Android 上的 wchar_t 是 4 bytes, 所以utf16相關改用 unsigned short
#include <cstdlib>
#include "TextConverter.h"
#include "MemoryMacro.h"
#include <cassert>
#include <clocale>

using namespace Enigma::Platforms;

typedef unsigned char byte_t;
typedef unsigned int uint32_t;

// Reference: http://en.wikipedia.org/wiki/Utf8
static const byte_t cUtf8Limits[] = {
 0xC0, // Start of a 2-byte sequence
 0xE0, // Start of a 3-byte sequence
 0xF0, // Start of a 4-byte sequence
 0xF8, // Start of a 5-byte sequence
 0xFC, // Start of a 6-byte sequence
 0xFE // Invalid: not defined by original UTF-8 specification
};

/*! Usually it is a 2 steps process to convert the string, invoke utf8ToUtf16() with
 dest equals to null so that it gives you destLen (not including null terminator),
 then allocate the destination with that amount of memory and call utf8ToUtf16() once
 again to perform the actual conversion. You can skip the first call if you sure
 the destination buffer is large enough to store the data.

 \note Here we assum sizeof(wchar_t) == 2
 \ref Modify from 7zip LZMA sdk
 */
bool Utf8ToUtf16(unsigned short* dest, size_t& destLen, const char* src, size_t maxSrcLen)
{
    //static_assert(sizeof(wchar_t) == 2, "sizeof wchar_t must 2");
    size_t destPos = 0, srcPos = 0;

    while (true)
    {
        byte_t c; // Note that byte_t should be unsigned
        size_t numAdds;

        if (srcPos == maxSrcLen || src[srcPos] == '\0')
        {
            if (dest && destLen != destPos)
            {
                assert(false && "The provided destLen should equals to what we calculated here");
                return false;
            }

            destLen = destPos;
            return true;
        }

        c = src[srcPos++];

        if (c < 0x80)
        { // 0-127, US-ASCII (single byte)
            if (dest) dest[destPos] = (unsigned short)c;
            ++destPos;
            continue;
        }

        if (c < 0xC0) // The first octet for each code point should within 0-191
            break;

        for (numAdds = 1; numAdds < 5; ++numAdds)
        {
            if (c < cUtf8Limits[numAdds]) break;
        }
        uint32_t value = c - cUtf8Limits[numAdds - 1];

        do
        {
            byte_t c2;
            if (srcPos == maxSrcLen || src[srcPos] == '\0') break;
            c2 = src[srcPos++];
            if (c2 < 0x80 || c2 >= 0xC0) break;
            value <<= 6;
            value |= (c2 - 0x80);
        } while (--numAdds != 0);

        if (value < 0x10000)
        {
            if (dest) dest[destPos] = (unsigned short)value;
            ++destPos;
        }
        else
        {
            value -= 0x10000;
            if (value >= 0x100000) break;
            if (dest)
            {
                dest[destPos + 0] = (unsigned short)(0xD800 + (value >> 10));
                dest[destPos + 1] = (unsigned short)(0xDC00 + (value & 0x3FF));
            }
            destPos += 2;
        }
    }

    destLen = destPos;
    return false;
}

//! See the documentation for utf8ToUtf16()
bool Utf16ToUtf8(char* dest, size_t& destLen, const unsigned short* src, size_t maxSrcLen)
{
    size_t destPos = 0, srcPos = 0;

    while (true)
    {
        uint32_t value;
        size_t numAdds;

        if (srcPos == maxSrcLen || src[srcPos] == L'\0')
        {
            if (dest && destLen != destPos)
            {
                assert(false && "The provided destLen should equals to what we calculated here");
                return false;
            }
            destLen = destPos;
            return true;
        }

        value = src[srcPos++];

        if (value < 0x80)
        { // 0-127, US-ASCII (single byte)
            if (dest) dest[destPos] = char(value);
            ++destPos;
            continue;
        }

        if (value >= 0xD800 && value < 0xE000)
        {
            if (value >= 0xDC00 || srcPos == maxSrcLen) break;
            uint32_t c2 = src[srcPos++];
            if (c2 < 0xDC00 || c2 >= 0xE000) break;
            value = ((value - 0xD800) << 10) | (c2 - 0xDC00);
        }

        for (numAdds = 1; numAdds < 5; ++numAdds)
        {
            if (value < (uint32_t(1) << (numAdds * 5 + 6))) break;
        }

        if (dest) dest[destPos] = char(cUtf8Limits[numAdds - 1] + (value >> (6 * numAdds)));
        ++destPos;

        do
        {
            --numAdds;
            if (dest) dest[destPos] = char(0x80 + ((value >> (6 * numAdds)) & 0x3F));
            ++destPos;
        } while (numAdds != 0);
    }

    destLen = destPos;
    return false;
}

#if TARGET_PLATFORM == PLATFORM_WIN32
void TextConverter::LocalCharacterToUtf16(char* local_ch, size_t byteSize, unsigned short* utf16_ch)
{
    _locale_t lt = _create_locale(LC_ALL, ""); // use OS locale
    _mbtowc_l((wchar_t*)utf16_ch, local_ch, byteSize, lt); //_create_locale(LC_ALL,"cht"));
    _free_locale(lt);
    //wchar_t wch[8];
    //size_t sizeConvert;
    //_mbstowcs_s_l(&sizeConvert,wch,local_ch,byteSize,_create_locale(LC_ALL,"cht"));
    //*utf16_ch=wch[0];
}

void TextConverter::LocalStringToUtf16(char* local_str, size_t str_len,
    unsigned short* utf16_str, size_t utf16_buff_size_in_words, size_t* ret_size)
{
    _locale_t lt = _create_locale(LC_ALL, ""); // use OS locale
    _mbstowcs_s_l(ret_size, (wchar_t*)utf16_str, utf16_buff_size_in_words, local_str, str_len, lt);
    _free_locale(lt);
    /* 這可以得到同樣結果，不過沒有跨平台需求，先不換，留著 */
    /*std::setlocale(LC_ALL, "");
    wchar_t wstr[5];
    std::mbstowcs(wstr, local_str, 5);*/
}

size_t TextConverter::CalcUtf8StringSize(char* local_str, size_t str_len)
{
    unsigned short* utf16_str = memalloc(unsigned short, str_len + 1);
    memset(utf16_str, 0, (str_len + 1) * sizeof(unsigned short));
    LocalStringToUtf16(local_str, str_len, utf16_str, str_len + 1, 0);
    size_t utf8_size;
    Utf16ToUtf8(0, utf8_size, (const unsigned short*)utf16_str, str_len);
    mefree(utf16_str);
    return utf8_size;
}

void TextConverter::LocalStringToUtf8(char* local_str, size_t str_len, char* utf8_str, size_t /*utf8_buff_size*/)
{
    unsigned short* utf16_str = memalloc(unsigned short, str_len + 1);
    memset(utf16_str, 0, (str_len + 1) * sizeof(unsigned short));
    LocalStringToUtf16(local_str, str_len, utf16_str, str_len + 1, 0);
    size_t dest_len = CalcUtf8StringSize(utf16_str, str_len);
    Utf16ToUtf8(utf8_str, dest_len, (const unsigned short*)utf16_str, str_len);
    mefree(utf16_str);
}

std::string TextConverter::LocalStringToUtf8(const std::string& local_str)
{
    size_t str_len = local_str.length();
    unsigned short* utf16_str = memalloc(unsigned short, str_len + 1);
    memset(utf16_str, 0, (str_len + 1) * sizeof(unsigned short));
    LocalStringToUtf16((char*)local_str.c_str(), str_len, utf16_str, str_len + 1, nullptr);
    size_t utf8_size;
    Utf16ToUtf8(0, utf8_size, (const unsigned short*)utf16_str, str_len);
    unsigned char* utf8_str = memalloc(unsigned char, utf8_size + 1);
    memset(utf8_str, 0, (utf8_size + 1) * sizeof(unsigned char));
    Utf16ToUtf8((char*)utf8_str, utf8_size, (const unsigned short*)utf16_str, str_len);
    mefree(utf16_str);
    std::string ret_utf8_str = std::string((char*)utf8_str);
    mefree(utf8_str);
    return ret_utf8_str;
}

void TextConverter::Utf16CharacterToLocal(unsigned short utf16_ch, char* local_ch, size_t local_buff_size)
{
    _locale_t lt = _create_locale(LC_ALL, ""); // use OS locale
    _wctomb_s_l(0, local_ch, local_buff_size, (wchar_t)utf16_ch, lt); //_create_locale(LC_ALL,"cht"));
    _free_locale(lt);
}

void TextConverter::Utf16StringToLocal(unsigned short* utf16_str, size_t str_len, char* local_str, size_t local_buff_size)
{
    _locale_t lt = _create_locale(LC_ALL, ""); // use OS locale
    _wcstombs_s_l(0, local_str, local_buff_size, (const wchar_t*)utf16_str, str_len * sizeof(unsigned short), lt);
    _free_locale(lt);
}

size_t TextConverter::CalcLocalStringSize(unsigned short* utf16_str, size_t str_len)
{
    _locale_t lt = _create_locale(LC_ALL, ""); // use OS locale
    size_t local_size;
    _wcstombs_s_l(&local_size, 0, 0, (const wchar_t*)utf16_str, str_len * sizeof(unsigned short), lt);
    _free_locale(lt);
    return local_size - 1;  // because local_size include null character  
}

void TextConverter::Utf8StringToLocal(char* utf8_str, size_t str_len, char* local_str, size_t local_str_size)
{
    size_t utf16_size;
    Utf8ToUtf16(0, utf16_size, (const char*)utf8_str, str_len);
    unsigned short* utf16_str = memalloc(unsigned short, utf16_size + 1);
    memset(utf16_str, 0, (utf16_size + 1) * sizeof(wchar_t));
    Utf8ToUtf16(utf16_str, utf16_size, utf8_str, str_len);
    //size_t local_size=CalcLocalStringSize((unsigned short*)utf16_str,utf16_size);
    Utf16StringToLocal((unsigned short*)utf16_str, utf16_size, local_str, local_str_size);
    mefree(utf16_str);
}

std::string TextConverter::Utf8StringToLocal(const std::string& utf8_str)
{
    size_t utf8_len = utf8_str.length();
    size_t utf16_size;
    Utf8ToUtf16(0, utf16_size, utf8_str.c_str(), utf8_len);
    unsigned short* utf16_str = memalloc(unsigned short, utf16_size + 1);
    memset(utf16_str, 0, (utf16_size + 1) * sizeof(unsigned short));
    Utf8ToUtf16(utf16_str, utf16_size, utf8_str.c_str(), utf8_len);
    size_t local_size = CalcLocalStringSize(utf16_str, (unsigned int)utf16_size);
    unsigned char* local_str = memalloc(unsigned char, local_size + 1);
    memset(local_str, 0, (local_size + 1) * sizeof(unsigned char));
    Utf16StringToLocal(utf16_str, utf16_size, (char*)local_str, local_size + 1);
    mefree(utf16_str);
    std::string ret_local_str = std::string((char*)local_str);
    mefree(local_str);
    return ret_local_str;
}
#endif

void TextConverter::Utf8StringToUtf16(char* utf8_str, size_t utf8_bytes_len, unsigned short* utf16_str, size_t utf16_buff_size)
{
    Utf8ToUtf16(utf16_str, utf16_buff_size, utf8_str, utf8_bytes_len);
}

void TextConverter::Utf16StringToUtf8(unsigned short* utf16_str, size_t str_len, char* utf8_str, size_t utf8_buff_size)
{
    Utf16ToUtf8(utf8_str, utf8_buff_size, (const unsigned short*)utf16_str, str_len);
}

size_t TextConverter::CalcUtf8StringSize(unsigned short* utf16_str, size_t str_len)
{
    size_t utf8_size;
    Utf16ToUtf8(0, utf8_size, (const unsigned short*)utf16_str, str_len);
    return utf8_size;
}

size_t TextConverter::CalcUtf16StringSize(char* utf8_str, size_t str_len)
{
    size_t utf16_size;
    Utf8ToUtf16(0, utf16_size, (const char*)utf8_str, str_len);
    return utf16_size;
}