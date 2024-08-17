/*********************************************************************
 * \file   TextureAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_ASSEMBLER_H
#define TEXTURE_ASSEMBLER_H

#include "GenericDto.h"
#include "TextureId.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/AlgebraBasicTypes.h"
#include <string>
#include <vector>

namespace Enigma::Engine
{
    class TextureAssembler
    {
    public:
        TextureAssembler(const TextureId& id);

        void factoryDesc(const FactoryDesc& fd) { m_factoryDesc = fd; }
        void format(const Graphics::GraphicFormat& fmt) { m_format = fmt; }
        void dimension(const MathLib::Dimension<unsigned>& dim) { m_dimension = dim; }
        void isCubeTexture(bool is_cube) { m_isCubeTexture = is_cube; }
        void surfaceCount(unsigned count) { m_surfaceCount = count; }
        void filePaths(const std::vector<std::string>& paths) { m_filePaths = paths; }

        void dimensionOfCreation(const MathLib::Dimension<unsigned>& dimension) { m_dimensionOfCreation = dimension; }
        void imageFilenamesOfLoad(const std::vector<std::string>& filenames) { m_imageFilenamesOfLoad = filenames; }

        Engine::GenericDto assemble() const;

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

    class TextureDisassembler
    {
    public:
        TextureDisassembler();

        [[nodiscard]] const TextureId& id() const { return m_id; }
        [[nodiscard]] const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] const Graphics::GraphicFormat& format() const { return m_format; }
        [[nodiscard]] const MathLib::Dimension<unsigned>& dimension() const { return m_dimension; }
        [[nodiscard]] bool isCubeTexture() const { return m_isCubeTexture; }
        [[nodiscard]] unsigned surfaceCount() const { return m_surfaceCount; }
        [[nodiscard]] const std::vector<std::string>& filePaths() const { return m_filePaths; }

        [[nodiscard]] const std::optional<MathLib::Dimension<unsigned>>& dimensionOfCreation() const { return m_dimensionOfCreation; }
        [[nodiscard]] const std::optional<std::vector<std::string>>& imageFilenamesOfLoad() const { return m_imageFilenamesOfLoad; }

        void disassemble(const Engine::GenericDto& dto);

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

#endif // TEXTURE_ASSEMBLER_H
