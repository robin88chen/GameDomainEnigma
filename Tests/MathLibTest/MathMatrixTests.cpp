#include "pch.h"
#include "CppUnitTest.h"
#include "MathGlobal.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Enigma::MathLib;

namespace MathLibTests
{
    TEST_CLASS(MathMatrixTests)
    {
    public:

        TEST_METHOD(Matrix2Test)
        {
            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(-10.0f, std::nextafter(10.0f, 10.1f));
            std::uniform_real_distribution<float> pi_rand(-Math::PI, Math::PI);
            Matrix2 mx1 = Matrix2(false);
            Assert::IsTrue(mx1 == Matrix2::IDENTITY);
            Matrix2 mx2 = Matrix2();
            Assert::IsTrue(mx2 == Matrix2::ZERO);
            float angle = pi_rand(generator);
            Matrix2 mxa(angle);
            mx1 = Matrix2::FromAngle(angle);
            Assert::IsTrue(mx1 == mxa);
            float angle2 = mx1.ToAngle();
            Assert::IsTrue(Math::IsEqual(angle, angle2));
            Matrix2 mx3 = Matrix2::MakeIdentity();
            Assert::IsTrue(mx3 == Matrix2::IDENTITY);
            mx3 = Matrix2::MakeZero();
            Assert::IsTrue(mx3 == Matrix2::ZERO);
            float m00 = unif_rand(generator);
            float m11 = unif_rand(generator);
            mx3 = Matrix2::MakeDiagonal(m00, m11);
            Matrix2 mx4 = Matrix2(m00, m11);
            Assert::IsTrue(mx3 == mx4);
            float mrc[4];
            mrc[0] = unif_rand(generator); mrc[1] = unif_rand(generator);
            mrc[2] = unif_rand(generator); mrc[3] = unif_rand(generator);
            Matrix2 mx5 = Matrix2(mrc[0], mrc[1], mrc[2], mrc[3]);
            Matrix2 mx6 = Matrix2(mrc);
            Assert::IsTrue(mx5 == mx6);
            Assert::IsTrue(Math::IsEqual(mx5[0][0], mrc[0]));
            //Assert::IsTrue(Math::IsEqual(mx5.m_21,mrc[2]));
            Assert::IsTrue(Math::IsEqual(mx5[1][0], mrc[2]));
            Assert::IsTrue(Math::IsEqual(mx5(1, 0), mrc[2]));
            Vector2 v1 = Vector2(mrc[0], mrc[1]);
            Vector2 v2 = Vector2(mrc[2], mrc[3]);
            Matrix2 mx7;
            mx7.SetRow(0, v1); mx7.SetRow(1, v2);
            Assert::IsTrue(mx5 == mx7);
            Vector2 v3 = Vector2(mrc[0], mrc[2]);
            Vector2 v4 = Vector2(mrc[1], mrc[3]);
            Matrix2 mx8;
            mx8.SetColumn(0, v3); mx8.SetColumn(1, v4);
            Assert::IsTrue(mx5 == mx8);
            float* mrc1 = (float*)mx8;
            Assert::IsTrue(Math::IsEqual(mrc1[0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mrc1[3], mrc[3]));
            Vector2 v5 = mx8.GetRow(0);
            Assert::IsTrue(v5 == v1);
            Vector2 v6 = mx7.GetColumn(1);
            Assert::IsTrue(v6 == v4);
            float mrc2[4];
            mx7.GetColumnMajor(mrc2);
            Assert::IsTrue(Math::IsEqual(mrc2[0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mrc2[1], mrc[2]));
            mx7 = mx1;
            Assert::IsTrue(mx7 == mx1);
            Assert::IsTrue(mx5 + mx6 == 2.0f * mx5);
            Assert::IsTrue(3.0f * mx8 == mx8 * 3.0f);
            Assert::IsTrue(mx5 - mx6 == Matrix2::ZERO);
            Assert::IsTrue(Matrix2::ZERO - mx5 == -mx6);
            Assert::IsTrue(mx5 / 2.0f == 0.5f * mx6);
            mx7 = mx5;
            mx7 += mx6;
            Assert::IsTrue(mx7 == 2.0f * mx5);
            mx7 -= mx5;
            Assert::IsTrue(mx7 == mx6);
            mx7 *= 2.0f;
            Assert::IsTrue(mx7 == 2.0f * mx6);
            mx7 /= 4.0f;
            Assert::IsTrue(mx7 == mx6 / 2.0f);
            mx8 = mx5.Inverse();
            Assert::IsTrue(mx8 * mx5 == Matrix2::IDENTITY);
            mx7 = mx5.Transpose();
            mx7 = mx7.Transpose();
            Assert::IsTrue(mx7 == mx5);
        }

        TEST_METHOD(Matrix3Test)
        {
            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(-10.0f, std::nextafter(10.0f, 10.1f));
            std::uniform_real_distribution<float> pi_rand(-Math::PI, Math::PI);
            std::uniform_real_distribution<float> half_pi_rand(-Math::HALF_PI, Math::HALF_PI);
            Matrix3 mx1 = Matrix3(false);
            Assert::IsTrue(mx1 == Matrix3::IDENTITY);
            Matrix3 mx2 = Matrix3();
            Assert::IsTrue(mx2 == Matrix3::ZERO);
            float angle = pi_rand(generator);
            if (angle < 0.0f) angle = -angle;
            Vector3 axis0 = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            axis0 = axis0.Normalize();
            Matrix3 mxa(axis0, angle);
            mx1 = Matrix3::FromAxisAngle(axis0, angle);
            Assert::IsTrue(mx1 == mxa);
            float angle2;
            Vector3 axis1;
            std::tie(axis1, angle2) = mx1.ToAxisAngle();
            Math::m_epsilonUlp = 10.0f;
            Assert::IsTrue(Math::IsEqual(angle, angle2));
            Assert::IsTrue(axis1 == axis0);
            Matrix3 mx3 = Matrix3(unif_rand(generator), unif_rand(generator), unif_rand(generator),
                unif_rand(generator), unif_rand(generator), unif_rand(generator),
                unif_rand(generator), unif_rand(generator), unif_rand(generator));
            mx3 = Matrix3::MakeIdentity();
            Assert::IsTrue(mx3 == Matrix3::IDENTITY);
            mx3 = Matrix3::MakeZero();
            Assert::IsTrue(mx3 == Matrix3::ZERO);
            float m00 = unif_rand(generator);
            float m11 = unif_rand(generator);
            float m22 = unif_rand(generator);
            mx3 = Matrix3::MakeDiagonal(m00, m11, m22);
            Matrix3 mx4 = Matrix3(m00, m11, m22);
            Assert::IsTrue(mx3 == mx4);
            float mrc[9];
            mrc[0] = unif_rand(generator); mrc[1] = unif_rand(generator);   mrc[2] = unif_rand(generator);
            mrc[3] = unif_rand(generator); mrc[4] = unif_rand(generator); mrc[5] = unif_rand(generator);
            mrc[6] = unif_rand(generator); mrc[7] = unif_rand(generator); mrc[8] = unif_rand(generator);
            Matrix3 mx5 = Matrix3(mrc[0], mrc[1], mrc[2], mrc[3], mrc[4], mrc[5], mrc[6], mrc[7], mrc[8]);
            Matrix3 mx6 = Matrix3(mrc);
            Assert::IsTrue(mx5 == mx6);
            Assert::IsTrue(Math::IsEqual(mx5[0][0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mx5[1][0], mrc[3]));
            Assert::IsTrue(Math::IsEqual(mx5[1][1], mrc[4]));
            Assert::IsTrue(Math::IsEqual(mx5(2, 0), mrc[6]));
            Vector3 v1 = Vector3(mrc[0], mrc[1], mrc[2]);
            Vector3 v2 = Vector3(mrc[3], mrc[4], mrc[5]);
            Vector3 v3 = Vector3(mrc[6], mrc[7], mrc[8]);

            Matrix3 mx7;
            mx7.SetRow(0, v1); mx7.SetRow(1, v2); mx7.SetRow(2, v3);
            Assert::IsTrue(mx5 == mx7);
            Matrix3 mx10 = Matrix3(v1, v2, v3, false);
            Assert::IsTrue(mx10 == mx7);
            Vector3 v4 = Vector3(mrc[0], mrc[3], mrc[6]);
            Vector3 v5 = Vector3(mrc[1], mrc[4], mrc[7]);
            Vector3 v6 = Vector3(mrc[2], mrc[5], mrc[8]);
            Matrix3 mx8;
            mx8.SetColumn(0, v4); mx8.SetColumn(1, v5); mx8.SetColumn(2, v6);
            Assert::IsTrue(mx5 == mx8);
            Matrix3 mx11 = Matrix3(v4, v5, v6, true);
            Assert::IsTrue(mx8 == mx11);
            Vector3 av[6] = { v1,v2,v3,v4,v5,v6 };
            Matrix3 mx12 = Matrix3(&av[0], false);
            Assert::IsTrue(mx12 == mx10);
            Matrix3 mx13 = Matrix3(&av[3], true);
            Assert::IsTrue(mx13 == mx11);
            float* mrc1 = (float*)mx8;
            Assert::IsTrue(Math::IsEqual(mrc1[0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mrc1[7], mrc[7]));
            Vector3 v7 = mx8.GetRow(0);
            Assert::IsTrue(v7 == v1);
            Vector3 v8 = mx7.GetColumn(1);
            Assert::IsTrue(v8 == v5);
            float mrc2[9];
            mx7.GetColumnMajor(mrc2);
            Assert::IsTrue(Math::IsEqual(mrc2[0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mrc2[7], mrc[5]));
            Matrix3 mx9 = Matrix3(mrc2);
            mx9 = mx9.Transpose();
            Assert::IsTrue(mx9 == mx7);
            mx7 = mx1;
            Assert::IsTrue(mx7 == mx1);
            Assert::IsTrue(mx5 + mx6 == 2.0f * mx5);
            Assert::IsTrue(3.0f * mx8 == mx8 * 3.0f);
            Assert::IsTrue(mx5 - mx6 == Matrix3::ZERO);
            Assert::IsTrue(Matrix3::ZERO - mx5 == -mx6);
            Assert::IsTrue(mx5 / 2.0f == 0.5f * mx6);
            mx7 = mx5;
            mx7 += mx6;
            Assert::IsTrue(mx7 == 2.0f * mx5);
            mx7 -= mx5;
            Assert::IsTrue(mx7 == mx6);
            mx7 *= 2.0f;
            Assert::IsTrue(mx7 == 2.0f * mx6);
            mx7 /= 4.0f;
            Assert::IsTrue(mx7 == mx6 / 2.0f);
            mx8 = mx5.Inverse();
            Math::m_epsilonUlp = 10.0f;
            Assert::IsTrue(mx8 * mx5 == Matrix3::IDENTITY);
            mx7 = mx5.Transpose();
            mx7 = mx7.Transpose();
            //Math::m_epsilonUlp = 1.0f;
            Assert::IsTrue(mx7 == mx5);

            float yaw = half_pi_rand(generator);
            float pitch = half_pi_rand(generator);
            float roll = half_pi_rand(generator);
            Matrix3 mx20;
            mx20 = Matrix3::FromEulerAnglesXYZ(EulerAngles{ yaw, pitch, roll });
            EulerAngles euler1;
            UniqueEulerAngle unique;
            std::tie(euler1, unique) = mx20.ToEulerAnglesXYZ();
            if (unique == UniqueEulerAngle::True)
            {
                Assert::IsTrue(Math::IsEqual(yaw, euler1.m_x));
                Assert::IsTrue(Math::IsEqual(pitch, euler1.m_y));
                Assert::IsTrue(Math::IsEqual(roll, euler1.m_z));
            }
            mx20 = Matrix3::FromEulerAnglesXZY(EulerAngles{ yaw, pitch, roll });
            std::tie(euler1, unique) = mx20.ToEulerAnglesXZY();
            if (unique == UniqueEulerAngle::True)
            {
                Assert::IsTrue(Math::IsEqual(yaw, euler1.m_x));
                Assert::IsTrue(Math::IsEqual(pitch, euler1.m_y));
                Assert::IsTrue(Math::IsEqual(roll, euler1.m_z));
            }
            mx20 = Matrix3::FromEulerAnglesYXZ(EulerAngles{ yaw, pitch, roll });
            std::tie(euler1, unique) = mx20.ToEulerAnglesYXZ();
            if (unique == UniqueEulerAngle::True)
            {
                Assert::IsTrue(Math::IsEqual(yaw, euler1.m_x));
                Assert::IsTrue(Math::IsEqual(pitch, euler1.m_y));
                Assert::IsTrue(Math::IsEqual(roll, euler1.m_z));
            }
            mx20 = Matrix3::FromEulerAnglesYZX(EulerAngles{ yaw, pitch, roll });
            std::tie(euler1, unique) = mx20.ToEulerAnglesYZX();
            if (unique == UniqueEulerAngle::True)
            {
                Assert::IsTrue(Math::IsEqual(yaw, euler1.m_x));
                Assert::IsTrue(Math::IsEqual(pitch, euler1.m_y));
                Assert::IsTrue(Math::IsEqual(roll, euler1.m_z));
            }
            mx20 = Matrix3::FromEulerAnglesZXY(EulerAngles{ yaw, pitch, roll });
            std::tie(euler1, unique) = mx20.ToEulerAnglesZXY();
            if (unique == UniqueEulerAngle::True)
            {
                Assert::IsTrue(Math::IsEqual(yaw, euler1.m_x));
                Assert::IsTrue(Math::IsEqual(pitch, euler1.m_y));
                Assert::IsTrue(Math::IsEqual(roll, euler1.m_z));
            }
            mx20 = Matrix3::FromEulerAnglesZYX(EulerAngles{ yaw, pitch, roll });
            std::tie(euler1, unique) = mx20.ToEulerAnglesZYX();
            if (unique == UniqueEulerAngle::True)
            {
                Assert::IsTrue(Math::IsEqual(yaw, euler1.m_x));
                Assert::IsTrue(Math::IsEqual(pitch, euler1.m_y));
                Assert::IsTrue(Math::IsEqual(roll, euler1.m_z));
            }

            Matrix3 mxp;
            mxp = Matrix3::RotationX(pitch);
            Matrix3 mx21;
            mx21 = Matrix3::FromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), pitch);
            Assert::IsTrue(mxp == mx21);
            Matrix3 mxy;
            mxy = Matrix3::RotationY(yaw);
            mx21 = Matrix3::FromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), yaw);
            Assert::IsTrue(mxy == mx21);
            Matrix3 mxr;
            mxr = Matrix3::RotationZ(roll);
            mx21 = Matrix3::FromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), roll);
            Assert::IsTrue(mx21 == mxr);
            Matrix3 mxrpy;
            mxrpy = Matrix3::RotationYawPitchRoll(yaw, pitch, roll);
            mx21 = Matrix3::FromEulerAnglesYXZ(EulerAngles{ pitch, yaw, roll });
            Assert::IsTrue(mx21 == mxrpy);
            mx21 = mxy * mxp * mxr;
            Assert::IsTrue(mx21 == mxrpy);
        }
        TEST_METHOD(Matrix4Test)
        {
            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(-10.0f, std::nextafter(10.0f, 10.1f));
            Matrix4 mx1 = Matrix4(false);
            Assert::IsTrue(mx1 == Matrix4::IDENTITY);
            Matrix4 mx2 = Matrix4();
            Assert::IsTrue(mx2 == Matrix4::ZERO);
            Matrix4 mx3 = Matrix4(unif_rand(generator), unif_rand(generator), unif_rand(generator), unif_rand(generator),
                unif_rand(generator), unif_rand(generator), unif_rand(generator), unif_rand(generator),
                unif_rand(generator), unif_rand(generator), unif_rand(generator), unif_rand(generator),
                unif_rand(generator), unif_rand(generator), unif_rand(generator), unif_rand(generator));
            mx3 = Matrix4::MakeIdentity();
            Assert::IsTrue(mx3 == Matrix4::IDENTITY);
            mx3 = Matrix4::MakeZero();
            Assert::IsTrue(mx3 == Matrix4::ZERO);
            float mrc[16];
            mrc[0] = unif_rand(generator); mrc[1] = unif_rand(generator);   mrc[2] = unif_rand(generator);  mrc[3] = unif_rand(generator);
            mrc[4] = unif_rand(generator); mrc[5] = unif_rand(generator);       mrc[6] = unif_rand(generator); mrc[7] = unif_rand(generator);
            mrc[8] = unif_rand(generator); mrc[9] = unif_rand(generator); mrc[10] = unif_rand(generator); mrc[11] = unif_rand(generator);
            mrc[12] = unif_rand(generator); mrc[13] = unif_rand(generator); mrc[14] = unif_rand(generator); mrc[15] = unif_rand(generator);

            Matrix4 mx5 = Matrix4(mrc[0], mrc[1], mrc[2], mrc[3], mrc[4], mrc[5], mrc[6], mrc[7], mrc[8],
                mrc[9], mrc[10], mrc[11], mrc[12], mrc[13], mrc[14], mrc[15]);
            Matrix4 mx6 = Matrix4(mrc);
            Assert::IsTrue(mx5 == mx6);
            Assert::IsTrue(Math::IsEqual(mx5[0][0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mx5[1][0], mrc[4]));
            Assert::IsTrue(Math::IsEqual(mx5[1][1], mrc[5]));
            Assert::IsTrue(Math::IsEqual(mx5(2, 0), mrc[8]));
            Assert::IsTrue(Math::IsEqual(mx5(3, 2), mrc[14]));
            Vector4 v1 = Vector4(mrc[0], mrc[1], mrc[2], mrc[3]);
            Vector4 v2 = Vector4(mrc[4], mrc[5], mrc[6], mrc[7]);
            Vector4 v3 = Vector4(mrc[8], mrc[9], mrc[10], mrc[11]);
            Vector4 va = Vector4(mrc[12], mrc[13], mrc[14], mrc[15]);

            Matrix4 mx7;
            mx7.SetRow(0, v1); mx7.SetRow(1, v2); mx7.SetRow(2, v3); mx7.SetRow(3, va);
            Assert::IsTrue(mx5 == mx7);
            Vector4 v4 = Vector4(mrc[0], mrc[4], mrc[8], mrc[12]);
            Vector4 v5 = Vector4(mrc[1], mrc[5], mrc[9], mrc[13]);
            Vector4 v6 = Vector4(mrc[2], mrc[6], mrc[10], mrc[14]);
            Vector4 vb = Vector4(mrc[3], mrc[7], mrc[11], mrc[15]);
            Matrix4 mx8;
            mx8.SetColumn(0, v4); mx8.SetColumn(1, v5); mx8.SetColumn(2, v6); mx8.SetColumn(3, vb);
            Assert::IsTrue(mx5 == mx8);

            float* mrc1 = (float*)mx8;
            Assert::IsTrue(Math::IsEqual(mrc1[0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mrc1[7], mrc[7]));
            Vector4 v7 = mx8.GetRow(0);
            Assert::IsTrue(v7 == v1);
            Vector4 v8 = mx7.GetColumn(1);
            Assert::IsTrue(v8 == v5);
            float mrc2[16];
            mx7.GetColumnMajor(mrc2);
            Assert::IsTrue(Math::IsEqual(mrc2[0], mrc[0]));
            Assert::IsTrue(Math::IsEqual(mrc2[7], mrc[13]));
            Matrix4 mx9 = Matrix4(mrc2);
            mx9 = mx9.Transpose();
            Assert::IsTrue(mx9 == mx7);

            mx7 = mx1;
            Assert::IsTrue(mx7 == mx1);
            Assert::IsTrue(mx5 + mx6 == 2.0f * mx5);
            Assert::IsTrue(3.0f * mx8 == mx8 * 3.0f);
            Assert::IsTrue(mx5 - mx6 == Matrix4::ZERO);
            Assert::IsTrue(Matrix4::ZERO - mx5 == -mx6);
            Assert::IsTrue(mx5 / 2.0f == 0.5f * mx6);
            mx7 = mx5;
            mx7 += mx6;
            Assert::IsTrue(mx7 == 2.0f * mx5);
            mx7 -= mx5;
            Assert::IsTrue(mx7 == mx6);
            mx7 *= 2.0f;
            Assert::IsTrue(mx7 == 2.0f * mx6);
            mx7 /= 4.0f;
            Assert::IsTrue(mx7 == mx6 / 2.0f);
            mx8 = mx5.Inverse();
            Math::m_epsilonUlp = 10.0f;
            Assert::IsTrue(mx8 * mx5 == Matrix4::IDENTITY);
            mx7 = mx5.Transpose();
            mx7 = mx7.Transpose();
            //Math::m_epsilonUlp = 1.0f;
            Assert::IsTrue(mx7 == mx5);

            Vector3 v31 = Vector3(mrc[0], mrc[1], mrc[2]);
            Vector3 v32 = Vector3(mrc[4], mrc[5], mrc[6]);
            Vector3 v33 = Vector3(mrc[8], mrc[9], mrc[10]);
            Vector3 v34 = Vector3(0.0f, 0.0f, 0.0f);
            Vector3 av3[4] = { v31,v32,v33,v34 };

            Matrix4 mx10_4 = Matrix4(v31, v32, v33, v34, false);
            Matrix3 mx10_3 = Matrix3(v31, v32, v33, false);
            Matrix4 mx11_4 = mx10_3;
            Assert::IsTrue(mx11_4 == mx10_4);
            Matrix3 mx11_3 = mx10_4;
            Assert::IsTrue(mx11_3 == mx10_3);
            Matrix4 mx12_4 = Matrix4(av3, false);
            Assert::IsTrue(mx12_4 == mx10_4);

            mx10_4 = Matrix4(v31, v32, v33, v34, true);
            mx10_3 = Matrix3(v31, v32, v33, true);
            mx11_4 = mx10_3;
            Assert::IsTrue(mx11_4 == mx10_4);
            mx11_3 = mx10_4;
            Assert::IsTrue(mx11_3 == mx10_3);
            mx12_4 = Matrix4(av3, true);
            Assert::IsTrue(mx12_4 == mx10_4);

            Vector4 v10 = Vector4(unif_rand(generator), unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Vector4 v11 = mx5 * v10;
            Vector4 v12;
            for (int i = 0; i < 4; i++)
            {
                v12.X() += mx5[0][i] * v10[i];
                v12.Y() += mx5[1][i] * v10[i];
                v12.Z() += mx5[2][i] * v10[i];
                v12.W() += mx5[3][i] * v10[i];
            }
            Assert::IsTrue(v11 == v12);

            Vector3 v301 = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Vector3 v302 = mx5 * v301;
            Vector4 v401 = Vector4(v301.X(), v301.Y(), v301.Z(), 1.0f);
            Vector4 v402 = mx5 * v401;
            Vector3 v303 = Vector3(v402.X() / v402.W(), v402.Y() / v402.W(), v402.Z() / v402.W());
            Assert::IsTrue(v302 == v303);

            Vector3 v304 = mx5.TransformCoord(v301);
            Assert::IsTrue(v304 == v302);
            Vector3 v305 = mx5.Transform(v301);
            Assert::IsTrue(Math::IsEqual(v305.X(), v402.X()));
            Assert::IsTrue(Math::IsEqual(v305.Y(), v402.Y()));
            Assert::IsTrue(Math::IsEqual(v305.Z(), v402.Z()));
            Vector4 v403 = Vector4(v301.X(), v301.Y(), v301.Z(), 0.0f);
            Vector4 v404 = mx5 * v403;
            Vector3 v306 = mx5.TransformVector(v301);
            Assert::IsTrue(Math::IsEqual(v306.X(), v404.X()));
            Assert::IsTrue(Math::IsEqual(v306.Y(), v404.Y()));
            Assert::IsTrue(Math::IsEqual(v306.Z(), v404.Z()));

            std::uniform_real_distribution<float> pi_rand(-Math::PI, Math::PI);
            std::uniform_real_distribution<float> half_pi_rand(-Math::HALF_PI, Math::HALF_PI);
            float yaw = half_pi_rand(generator);
            float pitch = half_pi_rand(generator);
            float roll = half_pi_rand(generator);

            Matrix3 mx20;
            mx20 = Matrix3::RotationYawPitchRoll(yaw, pitch, roll);
            Matrix4 mx21;
            mx21 = Matrix4::MakeRotationYawPitchRoll(yaw, pitch, roll);
            Matrix4 mx420 = mx20;
            Assert::IsTrue(mx420 == mx21);

            Matrix3 mx3p;
            mx3p = Matrix3::RotationX(pitch);
            Matrix4 mx4p;
            mx4p = Matrix4::MakeRotationXTransform(pitch);
            Matrix4 mx34p = mx3p;
            Assert::IsTrue(mx34p == mx4p);

            Matrix3 mx3y;
            mx3y = Matrix3::RotationY(yaw);
            Matrix4 mx4y;
            mx4y = Matrix4::MakeRotationYTransform(yaw);
            Matrix4 mx34y = mx3y;
            Assert::IsTrue(mx34y == mx4y);

            Matrix3 mx3r;
            mx3r = Matrix3::RotationZ(roll);
            Matrix4 mx4r;
            mx4r = Matrix4::MakeRotationZTransform(roll);
            Matrix4 mx34r = mx3r;
            Assert::IsTrue(mx34r = mx4r);

            Matrix3 mx3rpy;
            mx3rpy = Matrix3::RotationYawPitchRoll(yaw, pitch, roll);
            Matrix4 mx34rpy = mx3rpy;
            Matrix4 mx4rpy;
            mx4rpy = Matrix4::MakeRotationYawPitchRoll(yaw, pitch, roll);
            Assert::IsTrue(mx34rpy = mx4rpy);
            mx4rpy = mx4y * mx4p * mx4r;
            Assert::IsTrue(mx34rpy = mx4rpy);

            Vector3 vpos = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Vector3 vscale = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Matrix4 mxpos;
            mxpos = Matrix4::MakeTranslateTransform(vpos);
            Matrix4 mxscale;
            mxscale = Matrix4::MakeScaleTransform(vscale);
            Matrix4 mxsum = mxpos * mx4rpy * mxscale;

            Vector3 vpos1 = mxsum.UnMatrixTranslate();
            Vector3 vscale1 = mxsum.UnMatrixScale();
            Matrix4 mx4rpy1 = mxsum.UnMatrixRotation();
            Matrix4 mxpos1;
            mxpos1 = Matrix4::MakeTranslateTransform(vpos1);
            Matrix4 mxscale1;
            mxscale1 = Matrix4::MakeScaleTransform(vscale1);
            Matrix4 mxsum1 = mxpos1 * mx4rpy1 * mxscale1;
            Assert::IsTrue(mxsum == mxsum1);
        }
    };
}
