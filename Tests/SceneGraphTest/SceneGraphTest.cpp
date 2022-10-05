#include "pch.h"
#include "CppUnitTest.h"
#include "MathLib/MathGlobal.h"
#include "MathLib/Vector3.h"
#include "MathLib/Box3.h"
#include "MathLib/ContainmentBox3.h"
#include "GameEngine/BoundingVolume.h"
#include <random>
#include <cmath>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

namespace SceneGraphTest
{
    TEST_CLASS(SceneGraphTest)
    {
    public:

        TEST_METHOD(TestBounding)
        {
            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(-1.0f, std::nextafter(1.0f, 1.01f));
            std::uniform_real_distribution<float> pi_rand(-Math::PI, Math::PI);
            std::uniform_real_distribution<float> unif_ten_rand(-10.0f, std::nextafter(10.0f, 10.01f));
            std::uniform_real_distribution<float> half_pi_rand(-Math::HALF_PI, Math::HALF_PI);
            float x = unif_rand(generator);
            float y = unif_rand(generator);
            float z = unif_rand(generator);
            float ex = unif_rand(generator) + 1.01f;
            float ey = unif_rand(generator) + 1.01f;
            float ez = unif_rand(generator) + 1.01f;
            Vector3 v1 = Vector3(x, y, z);
            v1.NormalizeSelf();
            Vector3 v2 = v1.Cross(Vector3(0.0, 0.0, 1.0));
            Vector3 v3 = v2.Cross(v1);
            Vector3 center = Vector3(unif_ten_rand(generator), unif_ten_rand(generator), unif_ten_rand(generator));
            Box3 b0 = Box3(center, v1, v2, v3, ex, ey, ez);
            auto bound = b0.ComputeVertices();
            Assert::IsTrue(Math::IsEqual(bound[0].X(), center.X() - ex * v1.X() - ey * v2.X() - ez * v3.X()));
            Assert::IsTrue(Math::IsEqual(bound[2].Y(), center.Y() + ex * v1.Y() + ey * v2.Y() - ez * v3.Y()));
            Assert::IsTrue(Math::IsEqual(bound[4].Z(), center.Z() - ex * v1.Z() - ey * v2.Z() + ez * v3.Z()));
            Assert::IsTrue(Math::IsEqual(bound[6].Y(), center.Y() + ex * v1.Y() + ey * v2.Y() + ez * v3.Y()));
            Assert::IsTrue((bound[3] + bound[5]) == 2.0f * center);

            Box3 b1 = ContainmentBox3::ComputeOrientedBox(&bound[0], 8);
            Assert::IsTrue(b0.Center() == b1.Center());  // axis & extent usually won't match, only center pos.
            auto bound1 = b1.ComputeVertices();
            Math::m_epsilonUlp = 10.0f;
            int found = 0;
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (bound[i] == bound1[j])
                    {
                        found++;
                        break;
                    }
                }
            }
            Assert::IsTrue(TestCornerVertices(bound, bound1));
            BoundingVolume bv{ b0 };
            Assert::IsTrue(bv.BoundingBox3().value() == b0);
            BoundingVolume bv1 = BoundingVolume::CreateFromTransform(bv, Matrix4::IDENTITY);
            auto bound11 = bv1.BoundingBox3().value().ComputeVertices();
            Assert::IsTrue(TestCornerVertices(bound, bound11));
            float yaw = half_pi_rand(generator);
            float pitch = half_pi_rand(generator);
            float roll = half_pi_rand(generator);
            Matrix4 mx4rpy;
            mx4rpy = Matrix4::MakeRotationYawPitchRoll(yaw, pitch, roll);
            Vector3 vpos = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Vector3 vscale = Vector3(unif_rand(generator), unif_rand(generator), unif_rand(generator));
            Matrix4 mxpos;
            mxpos = Matrix4::MakeTranslateTransform(vpos);
            Matrix4 mxscale;
            mxscale = Matrix4::MakeScaleTransform(vscale);
            Matrix4 mxsum = mxpos * mx4rpy * mxscale;
            BoundingVolume bv2 = BoundingVolume::CreateFromTransform(bv, mxsum);
            auto bound2 = bound1;
            for (int i = 0; i < 8; i++)
            {
                bound2[i] = mxsum.TransformCoord(bound2[i]);
            }
            auto bound22 = bv2.BoundingBox3().value().ComputeVertices();
            Assert::IsTrue(TestCornerVertices(bound22, bound2));

            //! Merge 的結果並不是精確的Box, 只好測試是不是包含在內
            std::array<Vector3, 16> bound1_2;
            memcpy(&bound1_2[0], &bound1[0], sizeof(Vector3) * 8);
            memcpy(&bound1_2[8], &bound2[0], sizeof(Vector3) * 8);
            Box3 b23 = ContainmentBox3::ComputeOrientedBox(&bound1_2[0], 16);
            BoundingVolume bv23{ b23 };
            for (int i = 0; i < 16; i++)
            {
                Assert::IsTrue(bv23.PointInside(bound1_2[i]));
            }
            BoundingVolume bv3 = bv;
            bv3.Merge(mxsum, bv);
            for (int i = 0; i < 16; i++)
            {
                Assert::IsTrue(bv3.PointInside(bound1_2[i]));
            }
        }

        bool TestCornerVertices(const std::array<Vector3, 8>& b0, const std::array<Vector3, 8>& b1)
        {
            int found = 0;
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (b0[i] == b1[j])
                    {
                        found++;
                        break;
                    }
                }
            }
            return (found == 8);
        }
    };
}
