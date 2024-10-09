/*****************************************************************
 * \file   PrimitiveFactoryDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef PRIMITIVE_FACTORY_DELEGATE_H
#define PRIMITIVE_FACTORY_DELEGATE_H

#include <functional>
#include <memory>

namespace Enigma::Primitives
{
    class Primitive;
    class PrimitiveId;

    using PrimitiveCreator = std::function<std::shared_ptr<Primitive>(const PrimitiveId& id)>;
}

#endif // PRIMITIVE_FACTORY_DELEGATE_H
