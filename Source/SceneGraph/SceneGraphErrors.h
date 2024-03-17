/*********************************************************************
 * \file   SceneGraphErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_ERRORS_H
#define SCENE_GRAPH_ERRORS_H

#include <system_error>

namespace Enigma::SceneGraph
{
    enum class ErrorCode
    {
        ok = 0,

        cameraNotReady = 101,
        nullSceneGraph,
        dataNotReady,
        parentNode,
        nodeNotFound,
        nullPrimitive,
        nullCullerCamera,
        unsupportPawnPrimitive,

        invalidChangingCamera = 201,
        emptyLightQueryResult,
        cameraNotFound,
        frustumNotFound,
        sceneRepositoryFailed,
        entityAlreadyExists,
        sceneFactoryFailed,
        laziedContentNotFound,
        childDtoNotFound,

        spatialFactoryNotFound = 301,
        factoryRttiMismatch,
    };
    class ErrorCategory : public std::error_category
    {
    public:
        std::string message(int err) const override;
        const char* name() const noexcept override;
        static const std::error_category& get()
        {
            return ms_category;
        }
    private:
        const static ErrorCategory ms_category;
    };

    extern std::error_code make_error_code(ErrorCode ec);
}

namespace std
{
    // let compiler know that ErrorCode is compatible with std::error_code
    template <>
    struct is_error_code_enum<Enigma::SceneGraph::ErrorCode> : true_type {};
}


#endif // SCENE_GRAPH_ERRORS_H
