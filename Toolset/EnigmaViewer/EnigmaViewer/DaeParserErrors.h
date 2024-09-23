/*********************************************************************
 * \file   DaeParserErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_PARSER_ERRORS_H
#define DAE_PARSER_ERRORS_H

#include <system_error>

namespace EnigmaViewer
{
    enum class ParserError
    {
        ok = 0,
        emptyRoot,

        noAnimationsLib = 101,
        noAnimationNode,
        noAnimationNameNode,
        noAnimationKeyNode,
        noAnimationChannel,
        noAnimationSamplerSource,
        noAnimationSampler,
        noTimeValueSource,
        noTimeValueArray,
        noAnimMatrixSource,
        noAnimMatrixArray,
        noTimeValues,
        animMatrixCountMismatch,
        noAnimationTarget,
        noAnimationTargetMesh,
    };
    class ParserErrorCategory : public std::error_category
    {
    public:
        std::string message(int err) const override;
        const char* name() const noexcept override;
        static const std::error_category& get()
        {
            return ms_category;
        }
    private:
        const static ParserErrorCategory ms_category;
    };

    extern std::error_code make_error_code(ParserError er);
}

namespace std
{
    // let compiler know that ErrorCode is compatible with std::error_code
    template <>
    struct is_error_code_enum<EnigmaViewer::ParserError> : true_type {};
}

#endif // DAE_PARSER_ERRORS_H
