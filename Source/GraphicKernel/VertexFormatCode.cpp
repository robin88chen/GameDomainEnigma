#include "VertexFormatCode.h"
#include "VertexDescription.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/StringFormat.h"
#include <cassert>
#include <cstring>


using namespace Enigma::Graphics;

VertexFormatCode::VertexFormatCode()
{
    m_hasPosition3 = false;
    m_hasPosition4 = false;
    m_hasPaletteIndex = false;
    m_weightCount = 0;
    m_fvfCode = 0;
    m_texCount = 0;
    memset(&m_texCoordSize, 0, sizeof(m_texCoordSize));
}

bool VertexFormatCode::operator==(const VertexFormatCode& data) const
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
}

bool VertexFormatCode::operator !=(const VertexFormatCode& data) const
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
}

bool VertexFormatCode::operator <(const VertexFormatCode& data) const
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
}

void VertexFormatCode::addPosition3()
{
    assert(!m_hasPosition4);
    m_hasPosition3 = true;
    updateFvfCodeOfPaletteWeight();
}

void VertexFormatCode::addPosition4()
{
    assert(!m_hasPosition3);
    m_hasPosition4 = true;
    m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZW;
}

void VertexFormatCode::addNormal()
{
    m_fvfCode |= NORMAL;
}

void VertexFormatCode::addPointSize()
{
    m_fvfCode |= PSIZE;
}

void VertexFormatCode::addDiffuse()
{
    m_fvfCode |= DIFFUSE;
}

void VertexFormatCode::addSpecular()
{
    m_fvfCode |= SPECULAR;
}

void VertexFormatCode::addTangent()
{
    m_fvfCode |= TANGENT;
}

void VertexFormatCode::addBinormal()
{
    m_fvfCode |= BINORMAL;
}

void VertexFormatCode::addPaletteIndex()
{
    m_hasPaletteIndex = true;
    updateFvfCodeOfPaletteWeight();
}

void VertexFormatCode::addWeights(unsigned count)
{
    assert(count <= 4);
    m_weightCount = count;
    updateFvfCodeOfPaletteWeight();
}

void VertexFormatCode::addTextureCoord(unsigned stage, unsigned size)
{
    assert(stage < MAX_TEX_COORD);
    assert(size <= 4);
    m_texCoordSize[stage] = static_cast<unsigned char>(size);
    if (m_texCount <= stage) m_texCount = stage + 1;
}

void VertexFormatCode::addFloatDiffuse()
{
    m_fvfCode |= FLOAT_DIFFUSE;
}

void VertexFormatCode::addFloatSpecular()
{
    m_fvfCode |= FLOAT_SPECULAR;
}

void VertexFormatCode::updateFvfCodeOfPaletteWeight()
{
    if (!m_hasPosition3) return;
    if (m_hasPaletteIndex)
    {
        switch (m_weightCount)
        {
        case 0:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB1;
            break;
        case 1:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB2;
            break;
        case 2:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB3;
            break;
        case 3:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB4;
            break;
        case 4:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB5;
            break;
        default:
            assert(false);
            break;
        }
        m_fvfCode |= LASTBETA_UBYTE4;
    }
    else
    {
        switch (m_weightCount)
        {
        case 0:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZ;
            break;
        case 1:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB1;
            break;
        case 2:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB2;
            break;
        case 3:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB3;
            break;
        case 4:
            m_fvfCode = (m_fvfCode & (~POSITION_MASK)) | XYZB4;
            break;
        default:
            assert(false);
            break;
        }
    }
}

VertexDescription VertexFormatCode::calculateVertexSize()
{
    VertexDescription desc;
    desc.initialize();
    int size = 0;
    int offset = 0;
    int num_elements = 0;
    unsigned int pos_mask = m_fvfCode & POSITION_MASK;
    switch (pos_mask)
    {
    case XYZ:
        size += (3 * sizeof(float));
        offset += 3;
        desc.m_posVecDimension = 3;
        num_elements++;
        break;
    case XYZRHW:
        size += (4 * sizeof(float));
        offset += 4;
        desc.m_posVecDimension = 4;
        num_elements++;
        break;
    case XYZW:
        size += (4 * sizeof(float));
        offset += 4;
        desc.m_posVecDimension = 4;
        num_elements++;
        break;
    case XYZB1:
        size += (4 * sizeof(float));
        offset += 4;
        desc.m_posVecDimension = 3;
        if (m_fvfCode & LASTBETA_UBYTE4)
        {
            desc.m_paletteIndexOffset = 3;
        }
        else
        {
            desc.m_weightOffset = 3;
            desc.m_blendWeightCount = 1;
        }
        num_elements += 2;  // pos, (weight or index)
        break;
    case XYZB2:
        size += (5 * sizeof(float));
        offset += 5;
        desc.m_posVecDimension = 3;
        desc.m_weightOffset = 3;
        if (m_fvfCode & LASTBETA_UBYTE4)
        {
            desc.m_paletteIndexOffset = 4;
            desc.m_blendWeightCount = 1;
            num_elements += 3;  // pos, weight, index
        }
        else
        {
            desc.m_blendWeightCount = 2;
            num_elements += 2;  // pos, weight
        }
        //num_elements += 3;
        break;
    case XYZB3:
        size += (6 * sizeof(float));
        offset += 6;
        desc.m_posVecDimension = 3;
        desc.m_weightOffset = 3;
        if (m_fvfCode & LASTBETA_UBYTE4)
        {
            desc.m_paletteIndexOffset = 5;
            desc.m_blendWeightCount = 2;
            num_elements += 3;
        }
        else
        {
            desc.m_blendWeightCount = 3;
            num_elements += 2;
        }
        //num_elements += 4;
        break;
    case XYZB4:
        size += (7 * sizeof(float));
        offset += 7;
        desc.m_posVecDimension = 3;
        desc.m_weightOffset = 3;
        if (m_fvfCode & LASTBETA_UBYTE4)
        {
            desc.m_paletteIndexOffset = 6;
            desc.m_blendWeightCount = 3;
            num_elements += 3;
        }
        else
        {
            desc.m_blendWeightCount = 4;
            num_elements += 2;
        }
        //num_elements += 5;
        break;
    case XYZB5:
        size += (8 * sizeof(float));
        offset += 8;
        desc.m_posVecDimension = 3;
        desc.m_weightOffset = 3;
        if (m_fvfCode & LASTBETA_UBYTE4)
        {
            desc.m_paletteIndexOffset = 7;
            desc.m_blendWeightCount = 4;
            num_elements += 3;
        }
        else
        {
            desc.m_blendWeightCount = 5;
            num_elements += 2;
        }
        //num_elements += 6;
        break;
    default:
        break;
    }

    if (m_fvfCode & NORMAL)
    {
        desc.m_normalOffset = offset;
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
        desc.m_colorOffset = offset;
        desc.m_colorDimension = 1;
        size += sizeof(unsigned int);
        offset += 1;
        num_elements++;
    }
    if (m_fvfCode & SPECULAR)
    {
        desc.m_specularOffset = offset;
        desc.m_specularDimension = 1;
        size += sizeof(float);
        offset += 1;
        num_elements++;
    }
    if (m_fvfCode & FLOAT_DIFFUSE)
    {
        desc.m_colorOffset = offset;
        desc.m_colorDimension = 4;
        size += 4 * sizeof(float);
        offset += 4;
        num_elements++;
    }
    if (m_fvfCode & FLOAT_SPECULAR)
    {
        desc.m_specularOffset = offset;
        desc.m_specularDimension = 4;
        size += 4 * sizeof(float);
        offset += 4;
        num_elements++;
    }
    // texture coord. size
    if (m_texCount > 0)
    {
        for (unsigned int stage = 0; stage < m_texCount; stage++)
        {
            desc.m_texCoordOffset[stage] = offset;
            desc.m_texCoordSize[stage] = m_texCoordSize[stage];
            size += static_cast<int>(m_texCoordSize[stage] * sizeof(float));
            offset += m_texCoordSize[stage];
            if (m_texCoordSize[stage] > 0) num_elements++;
        }
    }

    // tangent & binormal
    if (m_fvfCode & TANGENT)
    {
        if (m_fvfCode & BINORMAL)
        {
            desc.m_tangentOffset = offset;
            desc.m_tangentDimension = 3;
            size += (3 * sizeof(float));
            offset += 3;
            num_elements++;
        }
        else
        {
            desc.m_tangentOffset = offset;
            desc.m_tangentDimension = 4;
            size += (4 * sizeof(float));
            offset += 4;
            num_elements++;
        }
    }
    if (m_fvfCode & BINORMAL)
    {
        desc.m_biNormalOffset = offset;
        size += (3 * sizeof(float));
        offset += 3;
        num_elements++;
    }

    desc.m_totalVertexSize = size;
    desc.m_numElements = num_elements;
    return desc;
}

#pragma warning(disable:4130)
void VertexFormatCode::fromString(const std::string& fvf_string)
{
    m_fvfCode = 0;
    m_texCount = 0;
    TokenVector fvf_tokens = split_token(fvf_string, "_");
    for (const auto& token : fvf_tokens)
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
            m_fvfCode |= FLOAT_DIFFUSE;
        }
        else if (token == "fspecular")
        {
            m_fvfCode |= FLOAT_SPECULAR;
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
                    m_texCount = static_cast<unsigned int>(count);
                    char* p = &token[4];
                    if (*p == '(')
                    {
                        for (int ti = 0; ti < count; ti++)
                        {
                            p++; // next coord size
                            int tex_size = *p - '0';
                            if ((tex_size > 0) && (tex_size <= 4))
                            {
                                m_texCoordSize[ti] = static_cast<unsigned char>(tex_size);
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

std::string VertexFormatCode::toString() const
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
    default: assert("invalid position mask" == 0); // C1430
    }

    if (m_fvfCode & NORMAL) ret += "_nor";
    if (m_fvfCode & PSIZE) ret += "_psize";
    if (m_fvfCode & DIFFUSE) ret += "_diffuse";
    if (m_fvfCode & SPECULAR) ret += "_specular";
    if (m_fvfCode & FLOAT_DIFFUSE) ret += "_fdiffuse";
    if (m_fvfCode & FLOAT_SPECULAR) ret += "_fspecular";
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
