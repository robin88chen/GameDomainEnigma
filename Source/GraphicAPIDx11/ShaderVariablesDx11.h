/*********************************************************************
 * \file   ShaderVariablesDx11.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_VARIABLES_DX11_H
#define SHADER_VARIABLES_DX11_H

#include "GraphicKernel/IShaderVariable.h"
#include "DeviceConstBufferDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/IDeviceSamplerState.h"
#include "GraphicKernel/ITexture.h"
#include "MathLib/Matrix4.h"
#include <D3D11Shader.h>

namespace Enigma::Devices
{
    using error = std::error_code;

    class DeviceConstBufferDx11;
    class ShaderVariableDx11_Base;

    class ShaderVariableDx11_Resource : public Graphics::IShaderVariable
    {
    public:
        ShaderVariableDx11_Resource(IShaderVariable::VarOwner var_of,
            const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount);
        ShaderVariableDx11_Resource(const ShaderVariableDx11_Resource&) = delete;
        ShaderVariableDx11_Resource(ShaderVariableDx11_Resource&&) = delete;
        virtual ~ShaderVariableDx11_Resource() override;
        ShaderVariableDx11_Resource& operator=(const ShaderVariableDx11_Resource&) = delete;
        ShaderVariableDx11_Resource& operator=(ShaderVariableDx11_Resource&&) = delete;

        //virtual void BindCustomFloatData(float* data, unsigned int dataFloatCount, unsigned int varCount = 0) override {};
        virtual void SetValue(std::any) override { }
        virtual void SetValues(std::any, unsigned int) override { }

        virtual Graphics::IShaderVariablePtr GetChildVariableByName(const std::string&) { return nullptr; }
        virtual Graphics::IShaderVariablePtr GetChildVariableBySemantic(const std::string&) { return nullptr; }
        virtual unsigned int GetChildVariableCount() { return 0; }
        virtual Graphics::IShaderVariablePtr GetChildVariableByIndex(unsigned int) { return nullptr; }

    protected:
        VarOwner m_varOf;
        unsigned int m_bindPoint;
        unsigned int m_bindCount;
    };

    class ShaderVariableDx11_ConstBuffer : public ShaderVariableDx11_Resource
    {
    public:
        ShaderVariableDx11_ConstBuffer(IShaderVariable::VarOwner var_of,
            const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount);
        ShaderVariableDx11_ConstBuffer(const ShaderVariableDx11_ConstBuffer&) = delete;
        ShaderVariableDx11_ConstBuffer(ShaderVariableDx11_ConstBuffer&&) = delete;
        virtual ~ShaderVariableDx11_ConstBuffer() override;
        ShaderVariableDx11_ConstBuffer& operator=(const ShaderVariableDx11_ConstBuffer&) = delete;
        ShaderVariableDx11_ConstBuffer& operator=(ShaderVariableDx11_ConstBuffer&&) = delete;

        //virtual void Commit() override;
        error create(const D3D11_SHADER_BUFFER_DESC& cb_desc, const std::string& shader_name);
        error CreateChildVariables(ID3D11ShaderReflectionConstantBuffer* cb_reflect, unsigned int var_count,
            const SemanticNameTable& semantic_table);

        virtual Graphics::IShaderVariablePtr GetChildVariableByName(const std::string& name) override;
        virtual Graphics::IShaderVariablePtr GetChildVariableBySemantic(const std::string& semantic) override;
        virtual unsigned int GetChildVariableCount() override;
        virtual Graphics::IShaderVariablePtr GetChildVariableByIndex(unsigned int index) override;

        void SetDirty() { m_isDirty = true; }

        unsigned char* GetDataBuff() { if (m_constBuffer) return m_constBuffer->GetDataBuffer(); return nullptr; }

        unsigned int GetBindPoint() { return m_bindPoint; }
        virtual error Apply() override;
        /** device const buffer dx11 會用保存的 data buffer 給 async functor, value 不會跑掉 */
        virtual future_error AsyncApply() override;

    protected:
        std::shared_ptr<ShaderVariableDx11_ConstBuffer> ThisSharedPtr()
        {
            return std::dynamic_pointer_cast<ShaderVariableDx11_ConstBuffer, IShaderVariable>(shared_from_this());
        }
    protected:
        bool m_isDirty;
        std::shared_ptr<DeviceConstBufferDx11> m_constBuffer;
        std::vector<std::shared_ptr<ShaderVariableDx11_Base>> m_childVariables;
        unsigned int m_childVariableCount;
    };
    class ShaderVariableDx11_Base : public Graphics::IShaderVariable
    {
    public:
        ShaderVariableDx11_Base(const std::string& name, const std::string& semantic,
            const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner);
        ShaderVariableDx11_Base(const ShaderVariableDx11_Base&) = delete;
        ShaderVariableDx11_Base(ShaderVariableDx11_Base&&) = delete;
        virtual ~ShaderVariableDx11_Base() override;
        ShaderVariableDx11_Base& operator=(const ShaderVariableDx11_Base&) = delete;
        ShaderVariableDx11_Base& operator=(ShaderVariableDx11_Base&&) = delete;

        //virtual void BindCustomFloatData(float* data, unsigned int dataFloatCount, unsigned int varCount = 0) override {};
        virtual void SetValue(std::any) override { }
        virtual void SetValues(std::any, unsigned int) override { }

        virtual error Apply() override { return Graphics::ErrorCode::ok; }

    protected:
        std::weak_ptr<ShaderVariableDx11_ConstBuffer> m_owner;
    };

    class ShaderVariableDx11_Float : public ShaderVariableDx11_Base
    {
    public:
        ShaderVariableDx11_Float(const std::string& name, const std::string& semantic,
            const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements);
        ShaderVariableDx11_Float(const ShaderVariableDx11_Float&) = delete;
        ShaderVariableDx11_Float(ShaderVariableDx11_Float&&) = delete;
        virtual ~ShaderVariableDx11_Float() override;
        ShaderVariableDx11_Float& operator=(const ShaderVariableDx11_Float&) = delete;
        ShaderVariableDx11_Float& operator=(ShaderVariableDx11_Float&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

    protected:
        unsigned int m_offset;
        //float* m_value;
        unsigned int m_numElements;
    };

    class ShaderVariableDx11_Int : public ShaderVariableDx11_Base
    {
    public:
        ShaderVariableDx11_Int(const std::string& name, const std::string& semantic,
            const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements);
        ShaderVariableDx11_Int(const ShaderVariableDx11_Int&) = delete;
        ShaderVariableDx11_Int(ShaderVariableDx11_Int&&) = delete;
        virtual ~ShaderVariableDx11_Int() override;
        ShaderVariableDx11_Int& operator=(const ShaderVariableDx11_Int&) = delete;
        ShaderVariableDx11_Int& operator=(ShaderVariableDx11_Int&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

    protected:
        unsigned int m_offset;
        //int* m_value;
        unsigned int m_numElements;
    };

    class ShaderVariableDx11_Boolean : public ShaderVariableDx11_Base
    {
    public:
        ShaderVariableDx11_Boolean(const std::string& name, const std::string& semantic,
            const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements);
        ShaderVariableDx11_Boolean(const ShaderVariableDx11_Boolean&) = delete;
        ShaderVariableDx11_Boolean(ShaderVariableDx11_Boolean&&) = delete;
        virtual ~ShaderVariableDx11_Boolean() override;
        ShaderVariableDx11_Boolean& operator=(const ShaderVariableDx11_Boolean&) = delete;
        ShaderVariableDx11_Boolean& operator=(ShaderVariableDx11_Boolean&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

    protected:
        unsigned int m_offset;
        BOOL* m_value;  // 傳入的 value data 是 1 byte boolean, 但 shader需要 4 byte BOOL
        unsigned int m_numElements;
    };

    class ShaderVariableDx11_Matrix : public ShaderVariableDx11_Base
    {
    public:
        ShaderVariableDx11_Matrix(const std::string& name, const std::string& semantic,
            const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements, bool isColumnMajor);
        ShaderVariableDx11_Matrix(const ShaderVariableDx11_Matrix&) = delete;
        ShaderVariableDx11_Matrix(ShaderVariableDx11_Matrix&&) = delete;
        virtual ~ShaderVariableDx11_Matrix() override;
        ShaderVariableDx11_Matrix& operator=(const ShaderVariableDx11_Matrix&) = delete;
        ShaderVariableDx11_Matrix& operator=(ShaderVariableDx11_Matrix&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

    protected:
        bool m_isColumnMajor;
        unsigned int m_offset;
        MathLib::Matrix4* m_transposMatrixValue;
        unsigned int m_numElements;
    };

    class ShaderVariableDx11_Vector : public ShaderVariableDx11_Base
    {
    public:
        ShaderVariableDx11_Vector(const std::string& name, const std::string& semantic,
            const std::shared_ptr<ShaderVariableDx11_ConstBuffer>& owner, unsigned int offset, unsigned int elements);
        ShaderVariableDx11_Vector(const ShaderVariableDx11_Vector&) = delete;
        ShaderVariableDx11_Vector(ShaderVariableDx11_Vector&&) = delete;
        virtual ~ShaderVariableDx11_Vector() override;
        ShaderVariableDx11_Vector& operator=(const ShaderVariableDx11_Vector&) = delete;
        ShaderVariableDx11_Vector& operator=(ShaderVariableDx11_Vector&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

    protected:
        unsigned int m_offset;
        unsigned int m_numElements;
    };

    class ShaderVariableDx11_Texture : public ShaderVariableDx11_Resource
    {
    public:
        ShaderVariableDx11_Texture(IShaderVariable::VarOwner var_of,
            const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount);
        ShaderVariableDx11_Texture(const ShaderVariableDx11_Texture&) = delete;
        ShaderVariableDx11_Texture(ShaderVariableDx11_Texture&&) = delete;
        virtual ~ShaderVariableDx11_Texture() override;
        ShaderVariableDx11_Texture& operator=(const ShaderVariableDx11_Texture&) = delete;
        ShaderVariableDx11_Texture& operator=(ShaderVariableDx11_Texture&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

        virtual error Apply() override;
        virtual future_error AsyncApply() override;

        error ApplyTexture(const Graphics::ITexturePtr& tex, std::optional<unsigned> indexMultiTexture);

    protected:
        Graphics::ITexturePtr m_texture; //todo: 為什麼原本用 weak ptr?? (2022.12.28)
        std::optional<unsigned int> m_indexMultiTexture;
    };
    class ShaderVariableDx11_Sampler : public ShaderVariableDx11_Resource
    {
    public:
        ShaderVariableDx11_Sampler(IShaderVariable::VarOwner var_of,
            const std::string& name, const std::string& semantic, unsigned int bindPoint, unsigned int bindCount);
        ShaderVariableDx11_Sampler(const ShaderVariableDx11_Sampler&) = delete;
        ShaderVariableDx11_Sampler(ShaderVariableDx11_Sampler&&) = delete;
        virtual ~ShaderVariableDx11_Sampler() override;
        ShaderVariableDx11_Sampler& operator=(const ShaderVariableDx11_Sampler&) = delete;
        ShaderVariableDx11_Sampler& operator=(ShaderVariableDx11_Sampler&&) = delete;

        virtual void SetValue(std::any data) override;
        virtual void SetValues(std::any data_array, unsigned int count) override;

        virtual error Apply() override;
        virtual future_error AsyncApply() override;

        error ApplySampler(const Graphics::IDeviceSamplerStatePtr& sampler);
    protected:
        Graphics::IDeviceSamplerStatePtr m_sampler; //todo: 為什麼原本用 weak ptr?? (2022.12.28)
    };
}


#endif // SHADER_VARIABLES_DX11_H
