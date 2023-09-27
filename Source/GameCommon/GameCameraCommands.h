/*********************************************************************
 * \file   GameCameraCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2023
 *********************************************************************/
#ifndef GAME_CAMERA_COMMANDS_H
#define GAME_CAMERA_COMMANDS_H
#include "Frameworks/Command.h"
#include "MathLib/Vector3.h"

namespace Enigma::GameCommon
{
    class ZoomCamera : public Frameworks::ICommand
    {
    public:
        ZoomCamera(float dist) : m_dist(dist) {}
        float GetDistance() const { return m_dist; }
    private:
        float m_dist;
    };
    class SphereRotateCamera : public Frameworks::ICommand
    {
    public:
        SphereRotateCamera(float horz_angle, float vert_angle, const MathLib::Vector3& center = MathLib::Vector3::ZERO) : m_horz_angle(horz_angle), m_vert_angle(vert_angle), m_center(center) {}
        float GetHorizontalAngle() const { return m_horz_angle; }
        float GetVerticalAngle() const { return m_vert_angle; }
        const MathLib::Vector3& GetCenter() const { return m_center; }

    private:
        float m_horz_angle;
        float m_vert_angle;
        MathLib::Vector3 m_center;
    };
    class MoveCamera : public Frameworks::ICommand
    {
    public:
        MoveCamera(float dir_dist, float slide_dist) : m_dir_dist(dir_dist), m_slide_dist(slide_dist) {}
        float GetDirectionDistance() const { return m_dir_dist; }
        float GetSlideDistance() const { return m_slide_dist; }

    private:
        float m_dir_dist;
        float m_slide_dist;
    };
    class MoveCameraXZ : public Frameworks::ICommand
    {
    public:
        MoveCameraXZ(float move_x, float move_z) : m_move_x(move_x), m_move_z(move_z) {}
        float GetMoveX() const { return m_move_x; }
        float GetMoveZ() const { return m_move_z; }
    private:
        float m_move_x;
        float m_move_z;
    };
}

#endif // GAME_CAMERA_COMMANDS_H
