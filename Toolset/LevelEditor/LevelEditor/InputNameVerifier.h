/*****************************************************************
 * \file   InputNameVerifier.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef INPUT_NAME_VERIFIER_H
#define INPUT_NAME_VERIFIER_H

#include <string>

namespace LevelEditor
{
    class InputNameVerifier
    {
    public:
        static bool isWorldNameDuplicated(const std::string& world_name);
        static bool isSpatialNameDuplicated(const std::string& spatial_name);
    };
}

#endif // INPUT_NAME_VERIFIER_H
