#include "SkinMeshPrimitive.h"
#include "SkinMeshPrimitiveAssembler.h"
#include "GameEngine/EffectMaterial.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;
using namespace Enigma::Primitives;

const std::string SEMANTIC_BONE_MATRIX = "BoneMatrix";

DEFINE_RTTI(Renderables, SkinMeshPrimitive, MeshPrimitive);

SkinMeshPrimitive::SkinMeshPrimitive(const PrimitiveId& id) : MeshPrimitive(id)
{
    m_factoryDesc = FactoryDesc(SkinMeshPrimitive::TYPE_RTTI.getName());
    m_ownerNodeRootRefTransform = Matrix4::IDENTITY;
}

SkinMeshPrimitive::~SkinMeshPrimitive()
{
    loosePrimitiveBoneMatrix();
}

std::shared_ptr<SkinMeshPrimitive> SkinMeshPrimitive::create(const Primitives::PrimitiveId& id)
{
    return std::make_shared<SkinMeshPrimitive>(id);
}

std::shared_ptr<PrimitiveAssembler> SkinMeshPrimitive::assembler() const
{
    return std::make_shared<SkinMeshPrimitiveAssembler>(m_id.origin());
}

std::shared_ptr<PrimitiveDisassembler> SkinMeshPrimitive::disassembler() const
{
    return std::make_shared<SkinMeshPrimitiveDisassembler>();
}

void SkinMeshPrimitive::changeMaterials(const std::vector<std::shared_ptr<PrimitiveMaterial>>& materials)
{
    loosePrimitiveBoneMatrix();
    MeshPrimitive::changeMaterials(materials);
    bindPrimitiveBoneMatrix();
}

void SkinMeshPrimitive::rebindMaterials()
{
    loosePrimitiveBoneMatrix();
    MeshPrimitive::rebindMaterials();
    bindPrimitiveBoneMatrix();
}

void SkinMeshPrimitive::bindOwnerRootRefTransform(const MathLib::Matrix4& mx)
{
    m_ownerNodeRootRefTransform = mx;
}

void SkinMeshPrimitive::changeEffects(const EffectMaterialList& effects)
{
    loosePrimitiveBoneMatrix();
    MeshPrimitive::changeEffects(effects);
    bindPrimitiveBoneMatrix();
}

void SkinMeshPrimitive::changeEffectInSegment(unsigned index, const std::shared_ptr<Engine::EffectMaterial>& effect)
{
    if (index >= m_materials.size()) return;
    looseSegmentBoneMatrix(index);
    MeshPrimitive::changeEffectInSegment(index, effect);
    bindSegmentBoneMatrix(index);
}

void SkinMeshPrimitive::createBoneMatrixArray(unsigned size)
{
    m_boneEffectMatrix.resize(size, Matrix4::IDENTITY);
}

void SkinMeshPrimitive::updateBoneEffectMatrix(unsigned idx, const MathLib::Matrix4& ref_mx)
{
    if (idx >= m_boneEffectMatrix.size()) return;
    m_boneEffectMatrix[idx] = ref_mx;
}

void SkinMeshPrimitive::clearBoneMatrixArray()
{
    loosePrimitiveBoneMatrix();
    m_boneEffectMatrix.clear();
}

void SkinMeshPrimitive::bindPrimitiveBoneMatrix()
{
    if (m_materials.empty()) return;
    for (auto& mat : m_materials)
    {
        if (!mat) continue;
        auto er = mat->assignVariableFunc(SEMANTIC_BONE_MATRIX,
            [lifetime = weak_from_this()](EffectVariable& v)
            { if (!lifetime.expired()) std::dynamic_pointer_cast<SkinMeshPrimitive, Primitive>(lifetime.lock())->assignBoneMatrix(v); });
        if (er) return;
    }
}

void SkinMeshPrimitive::bindSegmentBoneMatrix(unsigned index)
{
    if (index >= m_materials.size()) return;
    if (!m_materials[index]) return;
    m_materials[index]->assignVariableFunc(SEMANTIC_BONE_MATRIX,
        [lifetime = weak_from_this()](EffectVariable& v)
        { if (!lifetime.expired()) std::dynamic_pointer_cast<SkinMeshPrimitive, Primitive>(lifetime.lock())->assignBoneMatrix(v); });
}

void SkinMeshPrimitive::loosePrimitiveBoneMatrix()
{
    if (m_materials.empty()) return;
    for (auto& mat : m_materials)
    {
        if (!mat) continue;
        auto er = mat->assignVariableFunc(SEMANTIC_BONE_MATRIX, nullptr);
        if (er) return;
    }
}

void SkinMeshPrimitive::looseSegmentBoneMatrix(unsigned index)
{
    if (index >= m_materials.size()) return;
    if (!m_materials[index]) return;
    [[maybe_unused]] auto er = m_materials[index]->assignVariableFunc(SEMANTIC_BONE_MATRIX, nullptr);
}

void SkinMeshPrimitive::assignBoneMatrix(Engine::EffectVariable& var)
{
    if (m_boneEffectMatrix.empty()) return;
    var.assignValues(m_boneEffectMatrix, static_cast<unsigned>(m_boneEffectMatrix.size()));
}

