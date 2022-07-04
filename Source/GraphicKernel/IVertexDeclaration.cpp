#include "IVertexDeclaration.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/StringFormat.h"
#include <cassert>
#include <cstring>

using namespace Enigma::Graphics;

IVertexDeclaration::IVertexDeclaration(const std::string& vertex_format)
{
    m_formatString = vertex_format;
    m_dataVertexFormatCode.FromString(vertex_format);
    m_dataVertexFormatCode.CalculateVertexSize(&m_dataVertexFormatDesc);
}

IVertexDeclaration::~IVertexDeclaration()
{
}

//-------------------------- Vertex Format Code --------------------------------------------------
IVertexDeclaration::VertexFormatCode::VertexFormatCode()
{
    m_fvfCode = 0;
    m_texCount = 0;
    memset(&m_texCoordSize, 0, sizeof(m_texCoordSize));
};

IVertexDeclaration::VertexFormatCode::VertexFormatCode(unsigned int code, unsigned int tex_count,
    unsigned char size0, unsigned char size1, unsigned char size2, unsigned char size3,
    unsigned char size4, unsigned char size5, unsigned char size6, unsigned char size7)
{
    m_fvfCode = code; m_texCount = tex_count;
    m_texCoordSize[0] = size0; m_texCoordSize[1] = size1;
    m_texCoordSize[2] = size2; m_texCoordSize[3] = size3;
    m_texCoordSize[4] = size4; m_texCoordSize[5] = size5;
    m_texCoordSize[6] = size6; m_texCoordSize[7] = size7;
};

bool IVertexDeclaration::VertexFormatCode::operator==(const IVertexDeclaration::VertexFormatCode& data) const
{
    if (m_fvfCode != data.m_fvfCode) return false;
    if (m_texCount != data.m_texCount) return false;
    // for loop展開，比較快
    if (m_texCoordSize[0] != data.m_texCoordSize[0]) return false;
    if (m_texCoordSize[1] != data.m_texCoordSize[1]) return false;
    if (m_texCoordSize[2] != data.m_texCoordSize[2]) return false;
    if (m_texCoordSize[3] != data.m_texCoordSize[3]) return false;
    if (m_texCoordSize[4] != data.m_texCoordSize[4]) return false;
    if (m_texCoordSize[5] != data.m_texCoordSize[5]) return false;
    if (m_texCoordSize[6] != data.m_texCoordSize[6]) return false;
    if (m_texCoordSize[7] != data.m_texCoordSize[7]) return false;
    return true;
};

bool IVertexDeclaration::VertexFormatCode::operator !=(const IVertexDeclaration::VertexFormatCode& data) const
{
    if (m_fvfCode != data.m_fvfCode) return true;
    if (m_texCount != data.m_texCount) return true;
    // for loop展開，比較快
    if (m_texCoordSize[0] != data.m_texCoordSize[0]) return true;
    if (m_texCoordSize[1] != data.m_texCoordSize[1]) return true;
    if (m_texCoordSize[2] != data.m_texCoordSize[2]) return true;
    if (m_texCoordSize[3] != data.m_texCoordSize[3]) return true;
    if (m_texCoordSize[4] != data.m_texCoordSize[4]) return true;
    if (m_texCoordSize[5] != data.m_texCoordSize[5]) return true;
    if (m_texCoordSize[6] != data.m_texCoordSize[6]) return true;
    if (m_texCoordSize[7] != data.m_texCoordSize[7]) return true;
    return false;
};

bool IVertexDeclaration::VertexFormatCode::operator <(const IVertexDeclaration::VertexFormatCode& data) const
{
    if (m_fvfCode < data.m_fvfCode) return true;
    if (m_fvfCode > data.m_fvfCode) return false;
    if (m_texCount < data.m_texCount) return true;
    if (m_texCount > data.m_texCount) return false;
    // for loop展開，比較快
    if (m_texCoordSize[0] < data.m_texCoordSize[0]) return true;
    if (m_texCoordSize[0] > data.m_texCoordSize[0]) return false;
    if (m_texCoordSize[1] < data.m_texCoordSize[1]) return true;
    if (m_texCoordSize[1] > data.m_texCoordSize[1]) return false;
    if (m_texCoordSize[2] < data.m_texCoordSize[2]) return true;
    if (m_texCoordSize[2] > data.m_texCoordSize[2]) return false;
    if (m_texCoordSize[3] < data.m_texCoordSize[3]) return true;
    if (m_texCoordSize[3] > data.m_texCoordSize[3]) return false;
    if (m_texCoordSize[4] < data.m_texCoordSize[4]) return true;
    if (m_texCoordSize[4] > data.m_texCoordSize[4]) return false;
    if (m_texCoordSize[5] < data.m_texCoordSize[5]) return true;
    if (m_texCoordSize[5] > data.m_texCoordSize[5]) return false;
    if (m_texCoordSize[6] < data.m_texCoordSize[6]) return true;
    if (m_texCoordSize[6] > data.m_texCoordSize[6]) return false;
    if (m_texCoordSize[7] < data.m_texCoordSize[7]) return true;
    if (m_texCoordSize[7] > data.m_texCoordSize[7]) return false;
    return false;
};

int IVertexDeclaration::VertexFormatCode::CalculateVertexSize(IVertexDeclaration::VertexDesc* desc)
{
    if (desc) desc->Initialize();
    int size = 0;
    int offset = 0;
    int num_elements = 0;
    unsigned int pos_mask = m_fvfCode & POSITION_MASK;
    switch (pos_mask)
    {
    case XYZ:
        size += (3 * sizeof(float));
        offset += 3;
        if (desc)
        {
            desc->m_posVecDimension = 3;
        }
        num_elements++;
        break;
    case XYZRHW:
        size += (4 * sizeof(float));
        offset += 4;
        if (desc)
        {
            desc->m_posVecDimension = 4;
        }
        num_elements++;
        break;
    case XYZW:
        size += (4 * sizeof(float));
        offset += 4;
        if (desc)
        {
            desc->m_posVecDimension = 4;
        }
        num_elements++;
        break;
    case XYZB1:
        size += (4 * sizeof(float));
        offset += 4;
        if (desc)
        {
            desc->m_posVecDimension = 3;
            if (m_fvfCode & LASTBETA_UBYTE4)
            {
                desc->m_paletteIndexOffset = 3;
            }
            else
            {
                desc->m_weightOffset = 3;
                desc->m_blendWeightCount = 1;
            }
            num_elements += 2;  // pos, (weight or index)
        }
        break;
    case XYZB2:
        size += (5 * sizeof(float));
        offset += 5;
        if (desc)
        {
            desc->m_posVecDimension = 3;
            desc->m_weightOffset = 3;
            if (m_fvfCode & LASTBETA_UBYTE4)
            {
                desc->m_paletteIndexOffset = 4;
                desc->m_blendWeightCount = 1;
                num_elements += 3;  // pos, weight, index
            }
            else
            {
                desc->m_blendWeightCount = 2;
                num_elements += 2;  // pos, weight
            }
            //num_elements += 3;
        }
        break;
    case XYZB3:
        size += (6 * sizeof(float));
        offset += 6;
        if (desc)
        {
            desc->m_posVecDimension = 3;
            desc->m_weightOffset = 3;
            if (m_fvfCode & LASTBETA_UBYTE4)
            {
                desc->m_paletteIndexOffset = 5;
                desc->m_blendWeightCount = 2;
                num_elements += 3;
            }
            else
            {
                desc->m_blendWeightCount = 3;
                num_elements += 2;
            }
        }
        //num_elements += 4;
        break;
    case XYZB4:
        size += (7 * sizeof(float));
        offset += 7;
        if (desc)
        {
            desc->m_posVecDimension = 3;
            desc->m_weightOffset = 3;
            if (m_fvfCode & LASTBETA_UBYTE4)
            {
                desc->m_paletteIndexOffset = 6;
                desc->m_blendWeightCount = 3;
                num_elements += 3;
            }
            else
            {
                desc->m_blendWeightCount = 4;
                num_elements += 2;
            }
        }
        //num_elements += 5;
        break;
    case XYZB5:
        size += (8 * sizeof(float));
        offset += 8;
        if (desc)
        {
            desc->m_posVecDimension = 3;
            desc->m_weightOffset = 3;
            if (m_fvfCode & LASTBETA_UBYTE4)
            {
                desc->m_paletteIndexOffset = 7;
                desc->m_blendWeightCount = 4;
                num_elements += 3;
            }
            else
            {
                desc->m_blendWeightCount = 5;
                num_elements += 2;
            }
        }
        //num_elements += 6;
        break;
    }

    if (m_fvfCode & NORMAL)
    {
        if (desc)
        {
            desc->m_normalOffset = offset;
        }
        size += (3 * sizeof(float));
        offset += 3;
        num_elements++;
    }
    /*if (m_fvfCode & PSIZE)
    {
        size += sizeof(float);
        offset += 1;
        num_elements++;
    }*/
    if (m_fvfCode & DIFFUSE)
    {
        if (desc)
        {
            desc->m_colorOffset = offset;
            desc->m_colorDimension = 1;
        }
        size += sizeof(unsigned int);
        offset += 1;
        num_elements++;
    }
    if (m_fvfCode & SPECULAR)
    {
        if (desc)
        {
            desc->m_specularOffset = offset;
            desc->m_specularDimension = 1;
        }
        size += sizeof(float);
        offset += 1;
        num_elements++;
    }
    if (m_fvfCode & F_DIFFUSE)
    {
        if (desc)
        {
            desc->m_colorOffset = offset;
            desc->m_colorDimension = 4;
        }
        size += 4 * sizeof(float);
        offset += 4;
        num_elements++;
    }
    if (m_fvfCode & F_SPECULAR)
    {
        if (desc)
        {
            desc->m_specularOffset = offset;
            desc->m_specularDimension = 4;
        }
        size += 4 * sizeof(float);
        offset += 4;
        num_elements++;
    }
    // texture coord. size
    if (m_texCount > 0)
    {
        for (unsigned int stage = 0; stage < m_texCount; stage++)
        {
            if (desc)
            {
                desc->m_texCoordOffset[stage] = offset;
                desc->m_texCoordSize[stage] = m_texCoordSize[stage];
            }
            size += (m_texCoordSize[stage] * sizeof(float));
            offset += m_texCoordSize[stage];
            if (m_texCoordSize[stage] > 0) num_elements++;
        }
    }

    // tangent & binormal
    if (m_fvfCode & TANGENT)
    {
        if (m_fvfCode & BINORMAL)
        {
            if (desc)
            {
                desc->m_tangentOffset = offset;
                desc->m_tangentDimension = 3;
            }
            size += (3 * sizeof(float));
            offset += 3;
            num_elements++;
        }
        else
        {
            if (desc)
            {
                desc->m_tangentOffset = offset;
                desc->m_tangentDimension = 4;
            }
            size += (4 * sizeof(float));
            offset += 4;
            num_elements++;
        }
    }
    if (m_fvfCode & BINORMAL)
    {
        if (desc)
        {
            desc->m_biNormalOffset = offset;
        }
        size += (3 * sizeof(float));
        offset += 3;
        num_elements++;
    }

    if (desc) desc->m_totalVertexSize = size;
    if (desc) desc->m_numElements = num_elements;
    return size;
}

#pragma warning(disable:4130)
void IVertexDeclaration::VertexFormatCode::FromString(const std::string& fvf_string)
{
    m_fvfCode = 0;
    m_texCount = 0;
    TokenVector fvf_tokens = En_Split_Token(fvf_string, "_");
    for (auto token : fvf_tokens)
    {
        if (token == "xyz")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZ;
        }
        else if (token == "xyzrhw")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZRHW;
        }
        else if (token == "xyzw")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZW;
        }
        else if (token == "xyzb1")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB1;
        }
        else if (token == "xyzb2")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB2;
        }
        else if (token == "xyzb3")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB3;
        }
        else if (token == "xyzb4")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB4;
        }
        else if (token == "xyzb5")
        {
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB5;
        }
        else if (token == "nor")
        {
            m_fvfCode |= NORMAL;
        }
        else if (token == "psize")
        {
            m_fvfCode |= PSIZE;
        }
        else if (token == "diffuse")
        {
            m_fvfCode |= DIFFUSE;
        }
        else if (token == "specular")
        {
            m_fvfCode |= SPECULAR;
        }
        else if (token == "fdiffuse")
        {
            m_fvfCode |= F_DIFFUSE;
        }
        else if (token == "fspecular")
        {
            m_fvfCode |= F_SPECULAR;
        }
        else if (token == "tangent")
        {
            m_fvfCode |= TANGENT;
        }
        else if (token == "binormal")
        {
            m_fvfCode |= BINORMAL;
        }
        else if (token == "betabyte")
        {
            m_fvfCode |= LASTBETA_UBYTE4;
        }
        else if (token == "betacolor")
        {
            m_fvfCode |= LASTBETA_COLOR;
        }
        else
        {
            std::size_t pos = token.find_first_of("tex");
            if (pos == 0)
            {
                char c = token[3];
                int count = c - '0';
                if ((count > 0) && (count <= 8))
                {
                    m_texCount = (unsigned int)count;
                    char* p = &token[4];
                    if (*p == '(')
                    {
                        for (int ti = 0; ti < count; ti++)
                        {
                            p++; // next coord size
                            int tex_size = *p - '0';
                            if ((tex_size > 0) && (tex_size <= 4))
                            {
                                m_texCoordSize[ti] = (unsigned char)tex_size;
                            }
                            else
                            {
                                assert("invalid tex coord size in fvf string" == 0);  // C1430
                            }
                            p++;  // skip ','
                        }

                    }
                    else
                    {
                        assert("fvf string tex coord parse fail" == 0); // C4130
                    }

                }
                else
                {
                    assert("tex count invalid in fvf string" == 0); // C1430
                }

            }
            else
            {
                assert("unknown fvf string token" == 0); // C1430
            }
        }
    }
}
#pragma warning(default:4130)

std::string IVertexDeclaration::VertexFormatCode::ToString() const
{
    std::string ret;
    ret.reserve(256);
    unsigned int pos = m_fvfCode & POSITION_MASK;
    switch (pos)
    {
    case XYZ: ret = "xyz"; break;
    case XYZRHW: ret = "xyzrhw"; break;
    case XYZW: ret = "xyzw"; break;
    case XYZB1: ret = "xyzb1"; break;
    case XYZB2: ret = "xyzb2"; break;
    case XYZB3: ret = "xyzb3"; break;
    case XYZB4: ret = "xyzb4"; break;
    case XYZB5: ret = "xyzb5"; break;
    }

    if (m_fvfCode & NORMAL) ret += "_nor";
    if (m_fvfCode & PSIZE) ret += "_psize";
    if (m_fvfCode & DIFFUSE) ret += "_diffuse";
    if (m_fvfCode & SPECULAR) ret += "_specular";
    if (m_fvfCode & F_DIFFUSE) ret += "_fdiffuse";
    if (m_fvfCode & F_SPECULAR) ret += "_fspecular";
    if (m_fvfCode & TANGENT) ret += "_tangent";
    if (m_fvfCode & BINORMAL) ret += "_binormal";
    if (m_fvfCode & LASTBETA_UBYTE4) ret += "_betabyte";
    if (m_fvfCode & LASTBETA_COLOR) ret += "_betacolor";

    if (m_texCount)
    {
        std::string count = string_format("%d", m_texCount);
        ret += "_tex" + count + "(";
        for (unsigned int ti = 0; ti < m_texCount; ti++)
        {
            std::string size = string_format("%d", m_texCoordSize[ti]);
            ret += size;
            if (ti < m_texCount - 1) ret += ","; else ret += ")";
        }
    }

    return ret;
}

//------------------------- Vertex Desc ---------------------------------------------------
IVertexDeclaration::VertexDesc::VertexDesc()
{
    Initialize();

}

void IVertexDeclaration::VertexDesc::Initialize()
{
    m_totalVertexSize = 0;
    m_numElements = 0;
    m_positionOffset = 0;
    m_weightOffset = -1;
    m_paletteIndexOffset = -1;
    m_blendWeightCount = 0;
    m_normalOffset = -1;
    m_colorOffset = -1;
    m_specularOffset = -1;
    m_texCoordOffset[0] = -1;
    m_texCoordOffset[1] = -1;
    m_texCoordOffset[2] = -1;
    m_texCoordOffset[3] = -1;
    m_texCoordOffset[4] = -1;
    m_texCoordOffset[5] = -1;
    m_texCoordOffset[6] = -1;
    m_texCoordOffset[7] = -1;
    m_tangentOffset = -1;
    m_biNormalOffset = -1;
    m_texCoordSize[0] = 0;
    m_texCoordSize[1] = 0;
    m_texCoordSize[2] = 0;
    m_texCoordSize[3] = 0;
    m_texCoordSize[4] = 0;
    m_texCoordSize[5] = 0;
    m_texCoordSize[6] = 0;
    m_texCoordSize[7] = 0;
}

