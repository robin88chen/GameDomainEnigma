/*********************************************************************
 * \file   AnimationAssetId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_ID_H
#define ANIMATION_ASSET_ID_H

#include <string>

namespace Enigma::Animators
{
    class AnimationAssetId
    {
    public:
        AnimationAssetId() = default;
        AnimationAssetId(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

        bool operator==(const AnimationAssetId& other) const { return m_name == other.m_name; }
        bool operator!=(const AnimationAssetId& other) const { return m_name != other.m_name; }

        class hash
        {
        public:
            size_t operator()(const AnimationAssetId& id) const
            {
                return std::hash<std::string>()(id.m_name);
            }
        };

    protected:
        std::string m_name;
    };
}

#endif // ANIMATION_ASSET_ID_H
