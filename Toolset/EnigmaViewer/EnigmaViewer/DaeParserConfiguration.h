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

        void LoadConfig();

        std::string DefaultColorMeshEffectName();
        std::string DefaultColorMeshEffectFilename();

        std::string DefaultTexturedMeshEffectName();
        std::string DefaultTexturedMeshEffectFilename();

        std::string DefaultTexturedSkinMeshEffectName();
        std::string DefaultTexturedSkinMeshEffectFilename();

    private:
        Enigma::Engine::GenericDto m_configDto;
    };
}

#endif // DAE_PARSER_CONFIGURATION_H
