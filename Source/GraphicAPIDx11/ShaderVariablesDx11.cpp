#include "ShaderVariablesDx11.h"
#include "DeviceSamplerStateDx11.h"
#include "GraphicAPIDx11.h"
#include "MultiTextureDx11.h"
#include "TextureDx11.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "GraphicKernel/GraphicThread.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

ShaderVariableDx11_Resource::ShaderVariableDx11_Resource(IShaderVariable::VarOwner var_of,
    const std::string& name, const std::string& semantic,unsigned int bindPoint, unsigned int bindCount)
    : IShaderVariable(name, semantic)
{
    m_varOf = var_of;
    m_bindPoint = bindPoint;
    m_bindCount = bindCount;
}

ShaderVariableDx11_Resource::~ShaderVariableDx11_Resource()
{
}

//--------------------------------------------------------------------------------------
ShaderVariableDx11_ConstBuffer::ShaderVariableDx11_ConstBuffer(IShaderVariable::VarOwner var_of,
    const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount)
    : ShaderVariableDx11_Resource(var_of, name, semantic, bindPoint, bindCount)
{
    m_constBuffer = nullptr;
    m_childVariableCount = 0;
    m_isDirty = false;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::constBuffer));
}

ShaderVariableDx11_ConstBuffer::~ShaderVariableDx11_ConstBuffer()
{
    m_childVariables.clear();
    m_constBuffer = nullptr;
}

error ShaderVariableDx11_ConstBuffer::Create(const D3D11_SHADER_BUFFER_DESC& cb_desc,
    const std::string& shader_name)
{
    std::string cb_name = cb_desc.Name;
    if (cb_name == "$Globals") cb_name = shader_name + cb_name;
    m_constBuffer = std::shared_ptr<DeviceConstBufferDx11>{ menew DeviceConstBufferDx11{cb_name} };
    return m_constBuffer->CreateDataBuffer(cb_desc.Size);
}

error ShaderVariableDx11_ConstBuffer::CreateChildVariables(ID3D11ShaderReflectionConstantBuffer* cb_reflect,
    unsigned int var_count, const SemanticNameTable& semantic_table)
{
    assert(cb_reflect);
    m_childVariables.reserve(var_count);
    m_childVariableCount = var_count;

    for (unsigned int i = 0; i < var_count; i++)
    {
        ID3D11ShaderReflectionVariable* var_reflect = cb_reflect->GetVariableByIndex(i);
        if (!var_reflect) continue;
        D3D11_SHADER_VARIABLE_DESC var_desc;
        var_reflect->GetDesc(&var_desc);
        std::string var_semantic = "";
        if (!semantic_table.empty())
        {
            auto iter = semantic_table.find(var_desc.Name);
            if (iter != semantic_table.end())
            {
                var_semantic = iter->second;
            }
        }
        ID3D11ShaderReflectionType* var_type_reflect = var_reflect->GetType();
        D3D11_SHADER_TYPE_DESC type_desc;
        var_type_reflect->GetDesc(&type_desc);
        if (type_desc.Class == D3D_SVC_SCALAR)
        {
            if (type_desc.Type == D3D_SVT_FLOAT)
            {
                auto child_var = std::shared_ptr<ShaderVariableDx11_Base>{ menew ShaderVariableDx11_Float(var_desc.Name, var_semantic,
                    ThisSharedPtr(), var_desc.StartOffset, type_desc.Elements) };
                m_childVariables.emplace_back(child_var);
            }
            else if (type_desc.Type == D3D_SVT_INT)
            {
                auto child_var = std::shared_ptr<ShaderVariableDx11_Base>{ menew ShaderVariableDx11_Int(var_desc.Name, var_semantic,
                    ThisSharedPtr(), var_desc.StartOffset, type_desc.Elements) };
                m_childVariables.emplace_back(child_var);

            }
            else if (type_desc.Type == D3D_SVT_BOOL)
            {
                auto child_var = std::shared_ptr<ShaderVariableDx11_Base>{ menew ShaderVariableDx11_Boolean(var_desc.Name, var_semantic,
                    ThisSharedPtr(), var_desc.StartOffset, type_desc.Elements) };
                m_childVariables.emplace_back(child_var);
            }
        }
        else if (type_desc.Class == D3D_SVC_MATRIX_COLUMNS)
        {
            auto child_var = std::shared_ptr<ShaderVariableDx11_Base>{ menew ShaderVariableDx11_Matrix(var_desc.Name, var_semantic,
                ThisSharedPtr(), var_desc.StartOffset, type_desc.Elements, true) };
            m_childVariables.emplace_back(child_var);
        }
        else if (type_desc.Class == D3D_SVC_MATRIX_ROWS)
        {
            auto child_var = std::shared_ptr<ShaderVariableDx11_Base>{ menew ShaderVariableDx11_Matrix(var_desc.Name, var_semantic,
                ThisSharedPtr(), var_desc.StartOffset, type_desc.Elements, false) };
            m_childVariables.emplace_back(child_var);
        }
        else if (type_desc.Class == D3D_SVC_VECTOR)
        {
            //! 因為 shader var array 不處理 pack ，都是以 4 component 存放的關係
            if (type_desc.Columns != 4)
            {
                Platforms::Debug::ErrorPrintf("Vector Variable Must be float4!!");
            }
            auto child_var = std::shared_ptr<ShaderVariableDx11_Base>{ menew ShaderVariableDx11_Vector(var_desc.Name, var_semantic,
                ThisSharedPtr(), var_desc.StartOffset, type_desc.Elements) };
            m_childVariables.emplace_back(child_var);
        }
    }

    return ErrorCode::ok;
}
Enigma::Graphics::IShaderVariablePtr ShaderVariableDx11_ConstBuffer::GetChildVariableByName(const std::string& name)
{
    if (m_childVariableCount == 0) return nullptr;
    for (unsigned int i = 0; i < m_childVariableCount; i++)
    {
        if ((m_childVariables[i]) && (m_childVariables[i]->GetVariableName() == name))
        {
            return m_childVariables[i];
        }
    }
    return nullptr;
}

Enigma::Graphics::IShaderVariablePtr ShaderVariableDx11_ConstBuffer::GetChildVariableBySemantic(const std::string& semantic)
{
    if (m_childVariableCount == 0) return nullptr;
    for (unsigned int i = 0; i < m_childVariableCount; i++)
    {
        if ((m_childVariables[i]) && (m_childVariables[i]->GetVariableSemantic() == semantic))
        {
            return m_childVariables[i];
        }
    }
    return nullptr;
}

unsigned int ShaderVariableDx11_ConstBuffer::GetChildVariableCount()
{
    return m_childVariableCount;
}

Enigma::Graphics::IShaderVariablePtr ShaderVariableDx11_ConstBuffer::GetChildVariableByIndex(unsigned int index)
{
    if (m_childVariableCount == 0) return nullptr;
    return m_childVariables[index];
}

/*void ShaderVariableDx11_ConstBuffer::Commit()
{
    if (m_childVariableCount == 0) return;
    for (unsigned int i = 0; i < m_childVariableCount; i++)
    {
        if (m_childVariables[i]) m_childVariables[i]->Commit();
    }
}*/

error ShaderVariableDx11_ConstBuffer::Apply()
{
    assert(m_constBuffer);
    if (!m_isDirty)
    {
        return m_constBuffer->BindToShader(m_varOf, m_bindPoint);
    }
    m_isDirty = false;
    m_constBuffer->Apply();
    return m_constBuffer->BindToShader(m_varOf, m_bindPoint);
}

future_error ShaderVariableDx11_ConstBuffer::AsyncApply()
{
    assert(m_constBuffer);
    if (!m_isDirty)
    {
        return m_constBuffer->AsyncBindToShader(m_varOf, m_bindPoint);
    }
    m_isDirty = false;
    m_constBuffer->AsyncApply();
    return m_constBuffer->AsyncBindToShader(m_varOf, m_bindPoint);
}

//---------------------------------------------------
ShaderVariableDx11_Base::ShaderVariableDx11_Base(const std::string& name, const std::string& semantic,
    const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner) : IShaderVariable(name, semantic)
{
    m_owner = owner;
}

ShaderVariableDx11_Base::~ShaderVariableDx11_Base()
{
}

//------------------------------------------------------
ShaderVariableDx11_Float::ShaderVariableDx11_Float(const std::string& name, const std::string& semantic,
    const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements)
    : ShaderVariableDx11_Base(name, semantic, owner)
{
    m_offset = offset;
    if (elements == 0) elements = 1;
    m_numElements = elements;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::floatValue));
}

ShaderVariableDx11_Float::~ShaderVariableDx11_Float()
{
}

void ShaderVariableDx11_Float::SetValue(std::any data)
{
    if (m_owner.expired()) return;
    try
    {
        float value = std::any_cast<float>(data);
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, sizeof(float), &value, sizeof(float));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to float : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}
void ShaderVariableDx11_Float::SetValues(std::any data_array, unsigned int count)
{
    if (m_owner.expired()) return;
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        float* values;
        std::vector<float> value_vec;
        if (data_array.type() == typeid(std::vector<float>))
        {
            value_vec = std::any_cast<std::vector<float>>(data_array);
            values = &(value_vec[0]);
        }
        else
        {
            values = std::any_cast<float*>(data_array);
        }
        assert(values);
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, count * sizeof(float), values, count * sizeof(float));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to float array : %s", m_name.c_str(), m_semantic.c_str(), data_array.type().name(), e.what());
    }
}

//------------------------------------------------------
ShaderVariableDx11_Int::ShaderVariableDx11_Int(const std::string& name, const std::string& semantic,
    const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements)
    : ShaderVariableDx11_Base(name, semantic, owner)
{
    m_offset = offset;
    if (elements == 0) elements = 1;
    m_numElements = elements;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::intValue));
}

ShaderVariableDx11_Int::~ShaderVariableDx11_Int()
{
}

void ShaderVariableDx11_Int::SetValue(std::any data)
{
    if (m_owner.expired()) return;
    try
    {
        int value = std::any_cast<int>(data);
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, sizeof(int), &value, sizeof(int));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to int : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}
void ShaderVariableDx11_Int::SetValues(std::any data_array, unsigned int count)
{
    if (m_owner.expired()) return;
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        int* values;
        std::vector<int> value_vec;
        if (data_array.type() == typeid(std::vector<int>))
        {
            value_vec = std::any_cast<std::vector<int>>(data_array);
            values = &(value_vec[0]);
        }
        else
        {
            values = std::any_cast<int*>(data_array);
        }
        assert(values);
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, count * sizeof(int), values, count * sizeof(int));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to int array : %s", m_name.c_str(), m_semantic.c_str(), data_array.type().name(), e.what());
    }
}

//------------------------------------------------------
ShaderVariableDx11_Boolean::ShaderVariableDx11_Boolean(const std::string& name, const std::string& semantic,
    const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements)
    : ShaderVariableDx11_Base(name, semantic, owner)
{
    m_offset = offset;
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_value = memalloc(BOOL, m_numElements);
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::booleanValue));
}

ShaderVariableDx11_Boolean::~ShaderVariableDx11_Boolean()
{
    SAFE_FREE(m_value);
}

void ShaderVariableDx11_Boolean::SetValue(std::any data)
{
    if (m_owner.expired()) return;
    try
    {
        bool data_value = std::any_cast<bool>(data);
        m_value[0] = data_value ? TRUE : FALSE;
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, sizeof(BOOL), m_value, sizeof(BOOL));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to bool : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}
void ShaderVariableDx11_Boolean::SetValues(std::any data_array, unsigned int count)
{
    if (m_owner.expired()) return;
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        if (data_array.type() == typeid(std::vector<bool>))
        {
            auto data_values = std::any_cast<std::vector<bool>>(data_array);
            for (unsigned int i = 0; i < count; i++)
            {
                m_value[i] = data_values[i] ? TRUE : FALSE;
            }
        }
        else
        {
            bool* data_values = std::any_cast<bool*>(data_array);
            for (unsigned int i = 0; i < count; i++)
            {
                m_value[i] = data_values[i] ? TRUE : FALSE;
            }
        }
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, count * sizeof(BOOL), m_value, count * sizeof(BOOL));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to bool array : %s", m_name.c_str(), m_semantic.c_str(), data_array.type().name(), e.what());
    }
}

//------------------------------------------------------
ShaderVariableDx11_Matrix::ShaderVariableDx11_Matrix(const std::string& name, const std::string& semantic,
    const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner,
    unsigned int offset, unsigned int elements, bool isColumnMajor)
    : ShaderVariableDx11_Base(name, semantic, owner)
{
    m_isColumnMajor = isColumnMajor;
    m_offset = offset;
    if (elements == 0) elements = 1;
    m_numElements = elements;
    m_transposMatrixValue = nullptr;
    if (!m_isColumnMajor)
    {
        m_transposMatrixValue = menew MathLib::Matrix4[m_numElements];
    }
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::matrixValue));
}

ShaderVariableDx11_Matrix::~ShaderVariableDx11_Matrix()
{
    SAFE_DELETE_ARRAY(m_transposMatrixValue);
}

void ShaderVariableDx11_Matrix::SetValue(std::any data)
{
    if (m_owner.expired()) return;
    try
    {
        MathLib::Matrix4 value = std::any_cast<MathLib::Matrix4>(data);
        if (!m_isColumnMajor)
        {
            value = value.Transpose();
        }
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, sizeof(MathLib::Matrix4), &value, sizeof(MathLib::Matrix4));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to matrix4 : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}
void ShaderVariableDx11_Matrix::SetValues(std::any data_array, unsigned int count)
{
    if (m_owner.expired()) return;
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        const MathLib::Matrix4* values;
        std::vector<MathLib::Matrix4> mx_vector;
        if (data_array.type() == typeid(std::vector<MathLib::Matrix4>))
        {
            mx_vector = std::any_cast<std::vector<MathLib::Matrix4>>(data_array);
            values = &(mx_vector[0]);
        }
        else
        {
            values = std::any_cast<MathLib::Matrix4*>(data_array);
        }
        assert(values);
        if (m_isColumnMajor)
        {
            unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
            memcpy_s(dest, count * sizeof(MathLib::Matrix4), values, count * sizeof(MathLib::Matrix4));
        }
        else
        {  // shader variable is row major, the data Matrix is column major
            assert(m_transposMatrixValue);
            if (count == 1)
            {
                m_transposMatrixValue[0] = values[0].Transpose();
            }
            else
            {
                for (unsigned int i = 0; i < count; i++)
                {
                    m_transposMatrixValue[i] = values[i].Transpose();
                }
            }
            unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
            memcpy_s(dest, count * sizeof(MathLib::Matrix4), m_transposMatrixValue, count * sizeof(MathLib::Matrix4));
        }
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to matrix4 array : %s", m_name.c_str(), m_semantic.c_str(), data_array.type().name(), e.what());
    }
}

//------------------------------------------------------
ShaderVariableDx11_Vector::ShaderVariableDx11_Vector(const std::string& name, const std::string& semantic,
    const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements)
    : ShaderVariableDx11_Base(name, semantic, owner)
{
    m_offset = offset;
    if (elements == 0) elements = 1;
    m_numElements = elements;
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::vectorValue));
}

ShaderVariableDx11_Vector::~ShaderVariableDx11_Vector()
{
}

void ShaderVariableDx11_Vector::SetValue(std::any data)
{
    if (m_owner.expired()) return;
    try
    {
        float* values;
        MathLib::Vector4 vec;
        if (data.type() == typeid(MathLib::Vector4))
        {
            vec = std::any_cast<MathLib::Vector4>(data);
            values = static_cast<float*>(vec);
        }
        else
        {
            values = std::any_cast<float*>(data);
        }
        assert(values);
        MathLib::Vector4 value = std::any_cast<MathLib::Vector4>(data);
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, sizeof(MathLib::Vector4), values, sizeof(MathLib::Vector4));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to vector4 : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}
void ShaderVariableDx11_Vector::SetValues(std::any data_array, unsigned int count)
{
    if (m_owner.expired()) return;
    if (count == 0) count = 1;
    if (count > m_numElements) count = m_numElements;
    try
    {
        MathLib::Vector4* values;
        std::vector<MathLib::Vector4> vecs;
        if (data_array.type() == typeid(std::vector<MathLib::Vector4>))
        {
            vecs = std::any_cast<std::vector<MathLib::Vector4>>(data_array);
            values = &(vecs[0]);
        }
        else
        {
            values = std::any_cast<MathLib::Vector4*>(data_array);
        }
        assert(values);
        unsigned char* dest = m_owner.lock()->GetDataBuff() + m_offset;
        memcpy_s(dest, count * sizeof(MathLib::Vector4), values, count * sizeof(MathLib::Vector4));
        m_owner.lock()->SetDirty();
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to vector4 array : %s", m_name.c_str(), m_semantic.c_str(), data_array.type().name(), e.what());
    }
}
//----------------------------------------------------------------------------
ShaderVariableDx11_Texture::ShaderVariableDx11_Texture(VarOwner var_of,
    const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount)
    : ShaderVariableDx11_Resource(var_of, name, semantic, bindPoint, bindCount)
{
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::textureObject));
}

ShaderVariableDx11_Texture::~ShaderVariableDx11_Texture()
{
}

void ShaderVariableDx11_Texture::SetValue(std::any data)
{
    try
    {
        auto value = std::any_cast<TextureVarTuple>(data);
        m_texture = std::get<Graphics::ITexturePtr>(value);
        m_indexMultiTexture = std::get<std::optional<unsigned int>>(value);
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to texture : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}

void ShaderVariableDx11_Texture::SetValues(std::any data_array, unsigned int count)
{
    Platforms::Debug::Printf("shader variable of texture array not support!!");
}

error ShaderVariableDx11_Texture::Apply()
{
    return ApplyTexture(m_texture, m_indexMultiTexture);
}

future_error ShaderVariableDx11_Texture::AsyncApply()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), tex = m_texture, idx = m_indexMultiTexture]() -> error { return std::dynamic_pointer_cast<ShaderVariableDx11_Texture, IShaderVariable>(lifetime)->ApplyTexture(tex, idx); });
}

error ShaderVariableDx11_Texture::ApplyTexture(const Graphics::ITexturePtr& tex, std::optional<unsigned> indexMultiTexture)
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    ID3D11ShaderResourceView* resource[1] = { nullptr /*m_texture->GetD3DResourceView() */ };
    if ((tex) && (!tex->IsMultiTexture()))
    {
        TextureDx11* texDx11 = dynamic_cast<TextureDx11*>(tex.get());
        if (texDx11)
        {
            resource[0] = texDx11->GetD3DResourceView();
        }
        //Platforms::Debug::Printf("apply texture varibale %lx, with texture %lx\n", reinterpret_cast<std::uint64_t>(this), reinterpret_cast<std::uint64_t>(texDx11));
        /*unsigned int w, h;
        m_texture->GetDimension(&w, &h);
        DebugPrintf("Texture %s, %d, %d\n", m_texture->getName().String().c_str(), w, h);*/
    }
    else if ((tex) && (tex->IsMultiTexture()) && (indexMultiTexture))
    {
        MultiTextureDx11* texDx11 = dynamic_cast<MultiTextureDx11*>(tex.get());
        if (texDx11)
        {
            resource[0] = texDx11->GetD3DResourceView(indexMultiTexture.value());
        }
        /*unsigned int w, h;
        m_multiTexture->GetDimension(&w, &h);
        DebugPrintf("Multi Texture %s, %d, %d\n", m_multiTexture->getName().String().c_str(), w, h);*/
    }
    else
    {
        //Platforms::Debug::ErrorPrintf("apply texture variable %s(%s) fail\n", m_name.c_str(), m_semantic.c_str());
        return ErrorCode::applyTextureVariable;
    }
    /*ID3D11Resource* d3dResource;
    resource[0]->GetResource(&d3dResource);
    ID3D11Texture2D* d3dTex;
    d3dResource->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&d3dTex);
    D3D11_TEXTURE2D_DESC desc;
    d3dTex->GetDesc(&desc);
    DebugPrintf("D3DTexture %d, %d, %d\n", desc.Width, desc.Height, desc.ArraySize);*/
    if (m_varOf == VarOwner::VarOfVertexShader)
    {
        deviceContext->VSSetShaderResources(m_bindPoint, m_bindCount, resource);
    }
    else if (m_varOf == VarOwner::VarOfPixelShader)
    {
        deviceContext->PSSetShaderResources(m_bindPoint, m_bindCount, resource);
    }
    return ErrorCode::ok;
}

//----------------------------------------------------------------------------
ShaderVariableDx11_Sampler::ShaderVariableDx11_Sampler(VarOwner var_of,
    const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount)
    : ShaderVariableDx11_Resource(var_of, name, semantic, bindPoint, bindCount)
{
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::ShaderVariableCreated>(
        m_name, m_semantic, Graphics::ShaderVariableCreated::VarType::samplerState));
}

ShaderVariableDx11_Sampler::~ShaderVariableDx11_Sampler()
{
}

void ShaderVariableDx11_Sampler::SetValue(std::any data)
{
    try
    {
        m_sampler = std::any_cast<Graphics::IDeviceSamplerStatePtr>(data);
    }
    catch (const std::bad_any_cast& e)
    {
        Platforms::Debug::ErrorPrintf("variable %s(%s) bad any cast (%s) to sampler : %s", m_name.c_str(), m_semantic.c_str(), data.type().name(), e.what());
    }
}
void ShaderVariableDx11_Sampler::SetValues(std::any data_array, unsigned int count)
{
    Platforms::Debug::Printf("shader variable of sampler array not support!!");
}

error ShaderVariableDx11_Sampler::Apply()
{
    return ApplySampler(m_sampler);
}

future_error ShaderVariableDx11_Sampler::AsyncApply()
{
    return Graphics::IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), samp = m_sampler]() -> error { return std::dynamic_pointer_cast<ShaderVariableDx11_Sampler, IShaderVariable>(lifetime)->ApplySampler(samp); });
}

error ShaderVariableDx11_Sampler::ApplySampler(const Graphics::IDeviceSamplerStatePtr& sampler)
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    assert(deviceContext);

    if (!sampler) return ErrorCode::nullSamplerState;
    DeviceSamplerStateDx11* samplerDx11 = dynamic_cast<DeviceSamplerStateDx11*>(sampler.get());
    if (!samplerDx11) return ErrorCode::dynamicCastState;
    return samplerDx11->BindToShader(m_varOf, m_bindPoint);
}
