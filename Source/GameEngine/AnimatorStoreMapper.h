/*********************************************************************
 * \file   AnimatorStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_STORE_MAPPER_H
#define ANIMATOR_STORE_MAPPER_H

#include "GenericDto.h"
#include <system_error>

namespace Enigma::Engine
{
    class AnimatorId;

    class AnimatorStoreMapper
    {
    public:
        virtual ~AnimatorStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasAnimator(const AnimatorId& id) = 0;
        virtual std::optional<GenericDto> queryAnimator(const AnimatorId& id) = 0;
        virtual std::error_code removeAnimator(const AnimatorId& id) = 0;
        virtual std::error_code putAnimator(const AnimatorId& id, const GenericDto& dto) = 0;

        virtual std::uint64_t nextSequenceNumber() = 0;
    };
}

#endif // ANIMATOR_STORE_MAPPER_H
