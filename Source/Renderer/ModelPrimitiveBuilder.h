/*********************************************************************
 * \file   ModelPrimitiveBuilder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_BUILDER_H
#define _MODEL_PRIMITIVE_BUILDER_H

namespace Enigma::Renderer
{
    class ModelPrimitiveBuilder
    {
    public:
        ModelPrimitiveBuilder();
        ModelPrimitiveBuilder(const ModelPrimitiveBuilder&) = delete;
        ModelPrimitiveBuilder(ModelPrimitiveBuilder&&) = delete;
        ~ModelPrimitiveBuilder();
        ModelPrimitiveBuilder& operator=(const ModelPrimitiveBuilder&) = delete;
        ModelPrimitiveBuilder& operator=(ModelPrimitiveBuilder&&) = delete;
    };
}

#endif // _MODEL_PRIMITIVE_BUILDER_H
