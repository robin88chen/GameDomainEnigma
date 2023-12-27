/*********************************************************************
 * \file   TextureDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_DTO_H
#define TEXTURE_DTO_H

#include "GenericDto.h"
#include "TextureId.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/AlgebraBasicTypes.h"
#include <string>
#include <vector>

namespace Enigma::Engine
{
    class TextureDto
    {
    public:
        [[nodiscard]] TextureId id() const { return m_id; }
        TextureId& id() { return m_id; }
        [[nodiscard]] const Graphics::GraphicFormat& format() const { return m_format; }
        Graphics::GraphicFormat& format() { return m_format; }
        [[nodiscard]] const MathLib::Dimension<unsigned>& dimension() const { return m_dimension; }
        MathLib::Dimension<unsigned>& dimension() { return m_dimension; }
        [[nodiscard]] bool isCubeTexture() const { return m_isCubeTexture; }
        bool& isCubeTexture() { return m_isCubeTexture; }
        [[nodiscard]] unsigned surfaceCount() const { return m_surfaceCount; }
        unsigned& surfaceCount() { return m_surfaceCount; }
        [[nodiscard]] const std::vector<std::string>& filePaths() const { return m_filePaths; }
        std::vector<std::string>& filePaths() { return m_filePaths; }

        static TextureDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto() const;

    protected:
        TextureId m_id;
        Graphics::GraphicFormat m_format;
        MathLib::Dimension<unsigned> m_dimension;
        bool m_isCubeTexture = false;
        unsigned m_surfaceCount = 1;
        std::vector<std::string> m_filePaths;
    };
}

#endif // TEXTURE_DTO_H
