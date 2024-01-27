#include "MathAlgorithm.h"
#include "MathGlobal.h"
#include "Matrix3.h"
#include "Quaternion.h"
#include <cmath>

using namespace Enigma::MathLib;

Matrix4 MathAlgorithm::MakePerspectiveProjectionFovLH(float fovy, float aspect, float nearPlane, float farPlane)
{
    Matrix4 rm(true);
    float theta = fovy * 0.5f;
    float Q = farPlane / (farPlane - nearPlane);
    float h = (float)(cos(theta) / sin(theta));
    float w = h / aspect;

    rm[0][0] = w;
    rm[1][1] = h;
    rm[2][2] = Q;
    rm[3][2] = 1.0f;
    rm[2][3] = -Q * nearPlane;
    return rm;
}

Matrix4 MathAlgorithm::MakePerspectiveProjectionFovRH(float fovy, float aspect, float nearPlane, float farPlane)
{
    Matrix4 rm(true);
    float theta = fovy * 0.5f;
    float Q = farPlane / (nearPlane - farPlane);
    float h = (float)(cos(theta) / sin(theta));
    float w = h / aspect;

    rm[0][0] = w;
    rm[1][1] = h;
    rm[2][2] = Q;
    rm[3][2] = -1.0f;
    rm[2][3] = Q * nearPlane;
    return rm;
}

Matrix4 MathAlgorithm::MakePerspectiveProjectionLH(float w, float h, float nearPlane, float farPlane)
{
    Matrix4 rm(true);

    float Q = farPlane / (farPlane - nearPlane);

    rm[0][0] = 2.0f * nearPlane / w;
    rm[1][1] = 2.0f * nearPlane / h;
    rm[2][2] = Q;
    rm[3][2] = 1.0f;
    rm[2][3] = -Q * nearPlane;
    return rm;
}

Matrix4 MathAlgorithm::MakePerspectiveProjectionRH(float w, float h, float nearPlane, float farPlane)
{
    Matrix4 rm(true);

    float Q = farPlane / (nearPlane - farPlane);

    rm[0][0] = 2.0f * nearPlane / w;
    rm[1][1] = 2.0f * nearPlane / h;
    rm[2][2] = Q;
    rm[3][2] = -1.0f;
    rm[2][3] = Q * nearPlane;
    return rm;
}

Matrix4 MathAlgorithm::MakeLookAtTransformLH(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    Matrix4 rm(false);

    Vector3 axisX, axisY, axisZ;
    Vector3 tran;
    //      float X,Y,Z;
    Matrix3 rotation;

    axisZ = at - eye;
    axisZ.normalizeSelf();
    axisX = up.cross(axisZ);
    axisX.normalizeSelf();
    axisY = axisZ.cross(axisX);
    rotation = Matrix3(axisX, axisY, axisZ, false);
    tran = rotation * eye;
    return Matrix4(axisX, axisY, axisZ, -tran, false);
}

Matrix4 MathAlgorithm::MakeOrthoProjectionLH(float w, float h, float zn, float zf)
{
    Matrix4 rm(false);
    rm[0][0] = 2.0f / w;
    rm[1][1] = 2.0f / h;
    rm[2][2] = 1.0f / (zf - zn);
    rm[2][3] = zn / (zn - zf);
    return rm;
}

Matrix4 MathAlgorithm::MakeOrthoProjectionRH(float w, float h, float zn, float zf)
{
    Matrix4 rm(false);
    rm[0][0] = 2.0f / w;
    rm[1][1] = 2.0f / h;
    rm[2][2] = 1.0f / (zn - zf);
    rm[2][3] = zn / (zn - zf);
    return rm;
}

MathAlgorithm::TangentBinormalPair MathAlgorithm::CalculateTangentVector(const Vector3 vec[3], const Vector2 uv[3])
{
    Vector3 tan;
    Vector3 binor;

    Vector3 e1 = vec[1] - vec[0];
    Vector3 e2 = vec[2] - vec[0];
    if ((e1.length() <= Math::ZERO_TOLERANCE) || (e2.length() <= Math::ZERO_TOLERANCE))
    {
        return { tan, binor};
    }

    Vector2 st1 = uv[1] - uv[0];
    Vector2 st2 = uv[2] - uv[0];
    float st_perp = st1.dotPerp(st2);
    if (std::fabs(st_perp) <= Math::ZERO_TOLERANCE) return { tan, binor };

    float r = 1.0f / st_perp;
    tan = r * (st2.y() * e1 - st1.y() * e2);
    binor = r * (st1.x() * e2 - st2.x() * e1);
    return { tan,binor };
}
