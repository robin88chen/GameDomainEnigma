#include "MeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, MeshPrimitive, Primitive);

MeshPrimitive::MeshPrimitive(const std::string& name) : Primitive()
{
    m_name = name;
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
    m_elements.clear();
}

MeshPrimitive::MeshPrimitive(const MeshPrimitive& mesh) : Primitive()
{
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    m_elements = mesh.m_elements;
}

MeshPrimitive::MeshPrimitive(MeshPrimitive&& mesh) : Primitive()
{
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_elements = std::move(mesh.m_elements);
}

MeshPrimitive::~MeshPrimitive()
{
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
    m_elements.clear();
}

MeshPrimitive& MeshPrimitive::operator=(const MeshPrimitive& mesh)
{
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    m_elements = mesh.m_elements;
    return *this;
}

MeshPrimitive& MeshPrimitive::operator=(MeshPrimitive&& mesh)
{
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_elements = std::move(mesh.m_elements);
    return *this;
}

