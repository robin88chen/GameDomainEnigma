/*********************************************************************
 * \file   PawnEditService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PAWN_EDIT_SERVICE_H
#define PAWN_EDIT_SERVICE_H

#include "Frameworks/ServiceManager.h"
#include <system_error>
#include <MathLib/MathGlobal.h>

namespace LevelEditor
{
    using error = std::error_code;

    class PawnEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        PawnEditService(Enigma::Frameworks::ServiceManager* srv_mngr);
        PawnEditService(const PawnEditService&) = delete;
        PawnEditService& operator=(const PawnEditService&) = delete;
        PawnEditService(PawnEditService&&) = delete;
        PawnEditService& operator=(PawnEditService&&) = delete;
        virtual ~PawnEditService() override;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

        error PutCandidatePawn(const std::string& full_path, const Enigma::MathLib::Vector3& position);
    };
}

#endif // PAWN_EDIT_SERVICE_H
