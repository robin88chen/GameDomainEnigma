#include "TokenVector.h"

TokenVector En_Split_Token(const std::string& src, const std::string& sep)
{
    TokenVector token_vec;
    token_vec.reserve(10);

    std::string::size_type pos0 = src.find_first_not_of(sep, 0);
    while (pos0 != std::string::npos)
    {
        std::string::size_type pos1 = src.find_first_of(sep, pos0);
        token_vec.emplace_back(src.substr(pos0, pos1 - pos0));
        pos0 = src.find_first_not_of(sep, pos1);
    }
    return token_vec;
}
