/*********************************************************************
 * \file   TerrainEditService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_EDIT_SERVICE_H
#define TERRAIN_EDIT_SERVICE_H

#include "Frameworks/ServiceManager.h"

namespace LevelEditor
{
    class TerrainEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TerrainEditService(Enigma::Frameworks::ServiceManager* srv_mngr);
        TerrainEditService(const TerrainEditService&) = delete;
        TerrainEditService& operator=(const TerrainEditService&) = delete;
        virtual ~TerrainEditService() override;
        TerrainEditService(TerrainEditService&&) = delete;
        TerrainEditService& operator=(TerrainEditService&&) = delete;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

    public:
        static constexpr inline unsigned TextureLayerNum = 4;
    };
}

#endif // TERRAIN_EDIT_SERVICE_H
