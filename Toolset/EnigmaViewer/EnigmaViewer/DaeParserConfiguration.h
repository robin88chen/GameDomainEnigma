/*********************************************************************
 * \file   DaeParserConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef DAE_PARSER_CONFIGURATION_H
#define DAE_PARSER_CONFIGURATION_H

#include <string>
#include "GameEngine/GenericDto.h"

namespace EnigmaViewer
{
    class DaeParserConfiguration
    {
    public:
        DaeParserConfiguration() = default;

        void loadConfig();

        std::string defaultColorMeshEffectName();
        std::string defaultTexturedMeshEffectName();
        std::string defaultTexturedSkinMeshEffectName();

    private:
        Enigma::Engine::GenericDto m_configDto;
    };
}

#endif // DAE_PARSER_CONFIGURATION_H
