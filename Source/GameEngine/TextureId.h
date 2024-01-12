/*********************************************************************
 * \file   TextureId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_ID_H
#define TEXTURE_ID_H

#include <string>

namespace Enigma::Engine
{
    class TextureId
    {
    public:
        TextureId() = default;
        TextureId(const std::string& name) : m_name(name) {}
        TextureId(const TextureId& other) = default;
        TextureId(TextureId&& other) noexcept = default;
        TextureId& operator=(const TextureId& other) = default;
        TextureId& operator=(TextureId&& other) noexcept = default;
        ~TextureId() = default;

        bool operator==(const TextureId& other) const { return m_name == other.m_name; }
        bool operator!=(const TextureId& other) const { return m_name != other.m_name; }

        const std::string& name() const { return m_name; }

        class hash
        {
        public:
            size_t operator()(const TextureId& id) const
            {
                return std::hash<std::string>()(id.m_name);
            }
        };

    private:
        std::string m_name;
    };
}

#endif // TEXTURE_ID_H
