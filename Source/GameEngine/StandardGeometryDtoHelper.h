/*********************************************************************
 * \file   StandardGeometryDtoHelper.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef STANDARD_GEOMETRY_DTO_HELPER_H
#define STANDARD_GEOMETRY_DTO_HELPER_H

#include <string>
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "GeometryDataDto.h"
#include "GraphicKernel/VertexDescription.h"

namespace Enigma::Engine
{
    class SquareQuadDtoHelper
    {
    public:
        SquareQuadDtoHelper(const std::string& name);
        SquareQuadDtoHelper& XYQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top);
        SquareQuadDtoHelper& Normal();
        SquareQuadDtoHelper& TextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top);

        GenericDto ToGenericDto();

    protected:
        TriangleListDto m_dto;
        Graphics::VertexFormatCode m_format;
    };
}

#endif // STANDARD_GEOMETRY_DTO_HELPER_H
