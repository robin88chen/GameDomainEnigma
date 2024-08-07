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
        TextureDto();
        TextureDto(const GenericDto& dto);

        [[nodiscard]] const TextureId& id() const { return m_id; }
        void id(const TextureId& id) { m_id = id; }
        [[nodiscard]] const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const FactoryDesc& fd) { m_factoryDesc = fd; }
        [[nodiscard]] const Graphics::GraphicFormat& format() const { return m_format; }
        void format(const Graphics::GraphicFormat& fmt) { m_format = fmt; }
        [[nodiscard]] const MathLib::Dimension<unsigned>& dimension() const { return m_dimension; }
        void dimension(const MathLib::Dimension<unsigned>& dim) { m_dimension = dim; }
        [[nodiscard]] bool isCubeTexture() const { return m_isCubeTexture; }
        void isCubeTexture(bool is_cube) { m_isCubeTexture = is_cube; }
        [[nodiscard]] unsigned surfaceCount() const { return m_surfaceCount; }
        void surfaceCount(unsigned count) { m_surfaceCount = count; }
        [[nodiscard]] const std::vector<std::string>& filePaths() const { return m_filePaths; }
        void filePaths(const std::vector<std::string>& paths) { m_filePaths = paths; }

        [[nodiscard]] const std::optional<MathLib::Dimension<unsigned>>& dimensionOfCreation() const { return m_dimensionOfCreation; }
        void dimensionOfCreation(const MathLib::Dimension<unsigned>& dimension) { m_dimensionOfCreation = dimension; }
        [[nodiscard]] const std::optional<std::vector<std::string>>& imageFilenamesOfLoad() const { return m_imageFilenamesOfLoad; }
        void imageFilenamesOfLoad(const std::vector<std::string>& filenames) { m_imageFilenamesOfLoad = filenames; }

        GenericDto toGenericDto() const;

    protected:
        TextureId m_id;
        FactoryDesc m_factoryDesc;
        Graphics::GraphicFormat m_format;
        MathLib::Dimension<unsigned> m_dimension;
        bool m_isCubeTexture = false;
        unsigned m_surfaceCount = 1;
        std::vector<std::string> m_filePaths;

        std::optional<MathLib::Dimension<unsigned>> m_dimensionOfCreation;
        std::optional<std::vector<std::string>> m_imageFilenamesOfLoad;
    };
}

#endif // TEXTURE_DTO_H
