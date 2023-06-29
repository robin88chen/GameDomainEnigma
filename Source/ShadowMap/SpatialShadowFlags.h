/*********************************************************************
 * \file   SpatialShadowFlags.h
 * \brief  先把 shadow 相關 flag 放這裡，enum 沒辦法繼承或擴展，
 * 再想想
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef SPATIAL_SHADOW_FLAGS_H
#define SPATIAL_SHADOW_FLAGS_H

namespace Enigma::ShadowMap
{
    class SpatialShadowFlags
    {
    public:
        enum SpatialBit
        {
            Spatial_ShadowCaster = 0x10,
            Spatial_ShadowReceiver = 0x20,
        };
    };
}

#endif // SPATIAL_SHADOW_FLAGS_H
