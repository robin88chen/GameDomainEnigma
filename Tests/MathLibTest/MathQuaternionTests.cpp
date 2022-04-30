#include "pch.h"
#include "CppUnitTest.h"
#include "MathGlobal.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Enigma::MathLib;

namespace MathLibTests
{
    TEST_CLASS(MathQuaternionTests)
    {
    public:

        TEST_METHOD(QuaternionTest)
        {
            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(-10.0f, std::nextafter(10.0f, 10.1f));
            std::uniform_real_distribution<float> pi_rand(-Math::PI, Math::PI);
            std::uniform_real_distribution<float> half_pi_rand(-Math::HALF_PI, Math::HALF_PI);
            Math::m_epsilonUlp = 10.0f;
            Quaternion q0 = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
            Assert::IsTrue(q0 == Quaternion::ZERO);
            Quaternion q1 = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
            Assert::IsTrue(q1 == Quaternion::IDENTITY);
            Quaternion q2(q1);
            Assert::IsTrue(q2 == Quaternion::IDENTITY);
            float x = unif_rand(generator);
            float y = unif_rand(generator);
            float z = unif_rand(generator);
            float w = unif_rand(generator);
            Quaternion q3 = Quaternion(w, x, y, z);
            Assert::IsTrue(Math::IsEqual(x, q3.X()));
            Assert::IsTrue(Math::IsEqual(y, q3[2]));
            float* xyzw = (float*)q3;
            Assert::IsTrue(Math::IsEqual(xyzw[0], q3.W()));

            Assert::IsTrue(Math::IsEqual(q3.Length(), sqrt(x * x + y * y + z * z + w * w)));
            Assert::IsTrue(Math::IsEqual(q3.SquaredLength(), (x * x + y * y + z * z + w * w)));
            Assert::IsTrue(Math::IsEqual(q3.Dot(q3), q3.SquaredLength()));
            float q3len = q3.Length();
            Quaternion nor_q = q3.Normalize();
            Quaternion q3_l = q3 / q3.Length();
            Assert::IsTrue(nor_q == q3_l);

            //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
            Vector3 axis = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            axis = axis.Normalize();
            float angle = pi_rand(generator);
            Quaternion q4 = Quaternion(axis, angle);
            Assert::IsTrue(Math::IsEqual(q4.X(), sin(0.5f * angle) * axis.X()));
            Assert::IsTrue(Math::IsEqual(q4.W(), cos(0.5f * angle)));
            Quaternion q5;
            q5 = Quaternion::FromAxisAngle(axis, angle);
            Assert::IsTrue(q4 == q5);
            Quaternion q6 = q5;
            Vector3 axis2;
            float angle2;
            std::tie(axis2, angle2) = q6.ToAxisAngle();
            Assert::IsTrue(((axis == axis2) && (Math::IsEqual(angle, angle2))) || ((axis == -axis2) && (Math::IsEqual(angle, -angle2))));

            float yaw = half_pi_rand(generator);
            float pitch = half_pi_rand(generator);
            float roll = half_pi_rand(generator);

            Matrix3 mx_rot;
            mx_rot = Matrix3::RotationYawPitchRoll(yaw, pitch, roll);
            Quaternion q_rot;
            q_rot = Quaternion::FromRotationMatrix(mx_rot);
            Vector3 pos = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Vector3 pos1 = mx_rot * pos;
            Vector3 pos2 = q_rot * pos;
            Assert::IsTrue(pos1 == pos2);
            Vector3 pos3 = q_rot.Rotate(pos);
            Assert::IsTrue(pos3 == pos1);
            Matrix3 mx_rot2;
            mx_rot2 = q_rot.ToRotationMatrix();
            Assert::IsTrue(mx_rot == mx_rot2);

            Vector3 rot_column[3];
            rot_column[0] = mx_rot.GetColumn(0); rot_column[1] = mx_rot.GetColumn(1); rot_column[2] = mx_rot.GetColumn(2);
            Quaternion q_rot2;
            q_rot2 = Quaternion::FromRotationMatrix(Matrix3::FromColumnVectors(rot_column));
            Assert::IsTrue(q_rot == q_rot2);
            Vector3 rot_column2[3];
            Matrix3 mx_rot3 = q_rot2.ToRotationMatrix();
            Assert::IsTrue(rot_column[1] == mx_rot3.GetColumn(1));

            Quaternion q7 = q5 + q6;
            Assert::IsTrue(q7 == 2.0f * q5);
            Quaternion q8 = q7 - q6;
            Assert::IsTrue(q8 == q5);
            Quaternion q9 = q7 / 2.0f;
            Assert::IsTrue(q9 == q8);
            Assert::IsTrue(q5 * 0.5 == q6 / 2.0f);

            Quaternion q10 = q5.Inverse();
            Assert::IsTrue(q10 * q5 == Quaternion::IDENTITY);

            q7 += q6;
            Assert::IsTrue(q7 == 3.0f * q6);
            q7 -= q5;
            Assert::IsTrue(q7 == 2.0f * q6);
            q9 *= 2.0f;
            Assert::IsTrue(q9 == q7);
            q9 /= 2.0f;
            Assert::IsTrue(q9 == 0.5f * q7);

            Quaternion q11 = q5 + q5.Conjugate();
            q11 = q11.Normalize();
            Assert::IsTrue(q11 == Quaternion::IDENTITY);

            // Fix Me : how to test Slerp? 
        }
    };
}
