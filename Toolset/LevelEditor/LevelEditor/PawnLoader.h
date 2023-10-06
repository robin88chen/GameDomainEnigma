/*********************************************************************
 * \file   PawnLoader.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PAWN_LOADER_H
#define PAWN_LOADER_H

#include <system_error>

namespace LevelEditor
{
    using error = std::error_code;

    class PawnLoader
    {
    public:
        PawnLoader();
        ~PawnLoader();

        error StartLoadingPawn(const std::string& full_path);
    };
}

#endif // PAWN_LOADER_H
