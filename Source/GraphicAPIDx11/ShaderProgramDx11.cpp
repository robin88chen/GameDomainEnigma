#include "ShaderProgramDx11.h"
#include "GraphicAPIDx11.h"
#include "VertexShaderDx11.h"
#include "PixelShaderDx11.h"
#include "VertexDeclarationDx11.h"
#include "ShaderVariablesDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/IShaderVariable.h"
#include "Frameworks/StringFormat.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <d3dcompiler.h>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;


ShaderProgramDx11::ShaderProgramDx11(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader,
    const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl)
    : IShaderProgram(name, vtx_shader, pix_shader, vtx_decl)
{
    VertexShaderDx11* vtx_dx11 = dynamic_cast<VertexShaderDx11*>(m_vtxShader.get());
    PixelShaderDx11* pix_dx11 = dynamic_cast<PixelShaderDx11*>(m_pixShader.get());
    assert(vtx_dx11);
    assert(pix_dx11);
    // 重新 create vars, 就不用 clone vars 了
    RetrieveShaderVariables(vtx_dx11->GetD3DShaderReflection(), vtx_dx11->GetSemanticTable(),
        Graphics::IShaderVariable::VarOwner::VarOfVertexShader);
    RetrieveShaderVariables(pix_dx11->GetD3DShaderReflection(), pix_dx11->GetSemanticTable(),
        Graphics::IShaderVariable::VarOwner::VarOfPixelShader);
}

ShaderProgramDx11::~ShaderProgramDx11()
{
    m_variables.clear();
}

Enigma::Graphics::IShaderVariablePtr ShaderProgramDx11::GetVariableByName(const std::string& name)
{
    if (m_variables.empty()) return nullptr;
    for (auto var : m_variables)
    {
        if (!var) continue;
        if (var->GetVariableName() == name) return var;
        auto child = var->GetChildVariableByName(name);
        if (child) return child;
    }
    return nullptr;
}

Enigma::Graphics::IShaderVariablePtr ShaderProgramDx11::GetVariableBySemantic(const std::string& semantic)
{
    if (m_variables.empty()) return nullptr;
    for (auto var : m_variables)
    {
        if (!var) continue;
        if (var->GetVariableSemantic() == semantic) return var;
        auto child = var->GetChildVariableBySemantic(semantic);
        if (child) return child;
    }
    return nullptr;
}

unsigned int ShaderProgramDx11::GetVariableCount()
{
    if (m_variables.empty()) return 0;
    unsigned int total = (unsigned int)m_variables.size();
    for (auto var : m_variables)
    {
        if (var)
        {
            total += var->GetChildVariableCount();
        }
    }
    return total;
}

Enigma::Graphics::IShaderVariablePtr ShaderProgramDx11::GetVariableByIndex(unsigned int index)
{
    if (m_variables.empty()) return nullptr;
    if (index < m_variables.size()) return m_variables[index];
    unsigned int iter_idx = (unsigned int)m_variables.size();
    unsigned int parent_idx = 0;
    while (parent_idx < m_variables.size())
    {
        unsigned int child_idx = index - iter_idx;
        if (child_idx < m_variables[parent_idx]->GetChildVariableCount())
        {
            return m_variables[parent_idx]->GetChildVariableByIndex(child_idx);
        }
        iter_idx += m_variables[parent_idx]->GetChildVariableCount();
        parent_idx++;
    }
    return nullptr;
}

/*error ShaderProgramDx11::CommitVariables()
{
    if (m_variables.size() > 0)
    {
        for (auto var : m_variables)
        {
            if (var)
            {
                var->Commit();
            }
        }
    }
    return ErrorCode::ok;
}*/

error ShaderProgramDx11::ApplyVariables()
{
    if (m_variables.size() > 0)
    {
        for (auto var : m_variables)
        {
            if (var)
            {
                error er = var->Apply();
                if (er) return er;
            }
        }
    }
    return ErrorCode::ok;
}

future_error ShaderProgramDx11::AsyncApplyVariables()
{
    future_error er = make_future_err(ErrorCode::ok);
    if (m_variables.size() > 0)
    {
        for (auto var : m_variables)
        {
            if (var)
            {
                er = var->AsyncApply();
            }
        }
    }
    return er;
}

void ShaderProgramDx11::RetrieveShaderVariables(ID3D11ShaderReflection* shaderReflect,
    const Graphics::IShaderVariable::SemanticNameTable& semantic_table, Graphics::IShaderVariable::VarOwner var_of)
{
    GraphicAPIDx11* graphic = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(graphic);
    ID3D11Device* device = graphic->GetD3DDevice();
    assert(device);
    if (FATAL_LOG_EXPR(!shaderReflect)) return;

    D3D11_SHADER_DESC shader_desc;
    shaderReflect->GetDesc(&shader_desc);

    std::string shader_name;
    if (var_of == Graphics::IShaderVariable::VarOwner::VarOfVertexShader)
    {
        shader_name = m_vtxShader->GetName();
    }
    else
    {
        shader_name = m_pixShader->GetName();
    }
    // shader resource var
    for (unsigned int i = 0; i < shader_desc.BoundResources; i++)
    {
        D3D11_SHADER_INPUT_BIND_DESC res_desc;
        shaderReflect->GetResourceBindingDesc(i, &res_desc);

        std::string res_semantic = "";
        if (!semantic_table.empty())
        {
            auto iter = semantic_table.find(res_desc.Name);
            //stdext::hash_map <std::string, std::string>::iterator iter = semantic_table->find(res_desc.Name);
            if (iter != semantic_table.end())
            {
                res_semantic = iter->second;
            }
        }
        if (res_desc.Type == D3D_SIT_CBUFFER)
        {
            ID3D11ShaderReflectionConstantBuffer* reflect_cb =
                shaderReflect->GetConstantBufferByName(res_desc.Name);
            if (!reflect_cb) continue;
            D3D11_SHADER_BUFFER_DESC cb_desc;
            reflect_cb->GetDesc(&cb_desc);  // for global variables, cb name = "$Globals"
            auto var_cb = std::shared_ptr<ShaderVariableDx11_ConstBuffer>{
                menew ShaderVariableDx11_ConstBuffer(var_of, res_desc.Name, res_semantic.c_str(),
                res_desc.BindPoint, res_desc.BindPoint) };
            var_cb->Create(cb_desc, shader_name);
            if (cb_desc.Variables == 0) continue;
            var_cb->CreateChildVariables(reflect_cb, cb_desc.Variables, semantic_table);
            m_variables.push_back(std::dynamic_pointer_cast<ShaderVariableDx11_Resource, ShaderVariableDx11_ConstBuffer>(var_cb));
        }
        else if (res_desc.Type == D3D_SIT_TEXTURE)
        {
            auto var_tex = std::shared_ptr<ShaderVariableDx11_Texture>{ menew ShaderVariableDx11_Texture(var_of, res_desc.Name,
                res_semantic.c_str(), res_desc.BindPoint, res_desc.BindCount) };
            m_variables.push_back(std::dynamic_pointer_cast<ShaderVariableDx11_Resource, ShaderVariableDx11_Texture>(var_tex));
        }
        else if (res_desc.Type == D3D_SIT_SAMPLER)
        {
            auto var_samp = std::shared_ptr<ShaderVariableDx11_Sampler>{ menew ShaderVariableDx11_Sampler(var_of, res_desc.Name,
                res_semantic.c_str(), res_desc.BindPoint, res_desc.BindCount) };
            m_variables.push_back(std::dynamic_pointer_cast<ShaderVariableDx11_Resource, ShaderVariableDx11_Sampler>(var_samp));
        }
    }
}
