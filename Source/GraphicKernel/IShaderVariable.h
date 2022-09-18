/*********************************************************************
 * \file   IShaderVariable.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef SHADER_VARIABLE_INTERFACE_H
#define SHADER_VARIABLE_INTERFACE_H

#include "Frameworks/ExtentTypesDefine.h"
#include "ITexture.h"
#include <any>
#include <unordered_map>
#include <functional>
#include <string>
#include <memory>
#include <system_error>
#include <future>
#include <optional>

namespace Enigma::Graphics
{
    using error = std::error_code;

    /** Shader Variable interface */
    class IShaderVariable : public std::enable_shared_from_this<IShaderVariable>
    {
    public:
        enum class VarOwner
        {
            VarOfVertexShader,
            VarOfPixelShader,
        };
    public:
        using SemanticNameTable = std::unordered_map<std::string, std::string>;
        using TextureVarTuple = std::tuple<ITexturePtr, std::optional<unsigned int>>;

    public:
        IShaderVariable(const std::string& name, const std::string& semantic);
        IShaderVariable(const IShaderVariable&) = delete;
        IShaderVariable(IShaderVariable&&) = delete;
        virtual ~IShaderVariable();
        IShaderVariable& operator=(const IShaderVariable&) = delete;
        IShaderVariable& operator=(IShaderVariable&&) = delete;

        virtual const std::string& GetVariableName() { return m_name; };
        virtual const std::string& GetVariableSemantic() { return m_semantic; };
        /**  set value 提供　Variable Map 或是其他外部的 commit 函式將值設定至變數資料中,
        而後再經由不同平台的不同 Apply 函式，設定到 Device中,
        for multi-texture, indices 跟在 data 裡 */
        virtual void SetValue(std::any data) = 0;
        virtual void SetValues(std::any data_array, unsigned int count) = 0;

        /** 實作上要注意 apply 時, value 是不是已經被後來的 commit修改了 */
        virtual error Apply() = 0;
        virtual future_error AsyncApply();

        //todo : maybe useful
        //virtual void BindCustomFloatData(float* pData, unsigned int nDataFloatCount, unsigned int nVarCount = 0) = 0;

    protected:
        std::string m_name;
        std::string m_semantic;
    };
    using IShaderVariablePtr = std::shared_ptr<IShaderVariable>;
    using IShaderVariableWeak = std::weak_ptr<IShaderVariable>;
};


#endif // SHADER_VARIABLE_INTERFACE_H
