#include "pch.h"
#include "CppUnitTest.h"
#include "ColorRGB.h"
#include "ColorRGBA.h"
#include "MathGlobal.h"
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Enigma::MathLib;

namespace MathLibTests
{
    TEST_CLASS(MathColorTests)
    {
    public:

        TEST_METHOD(ColorRGBTest)
        {
            Assert::IsTrue(ColorRGB(0.0f, 0.0f, 0.0f) == ColorRGB::BLACK);
            Assert::IsTrue(ColorRGB(1.0f, 1.0f, 1.0f) == ColorRGB::WHITE);
            Assert::IsTrue(ColorRGB(-1.0f, -1.0f, -1.0f) == ColorRGB::INVALID);
            Assert::IsTrue(ColorRGB(0.0f, 0.0f, 0.0f) == ColorRGB::ZERO);

            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(0.0f, std::nextafter(2.0f, 2.1f));
            float r = unif_rand(generator);
            float g = unif_rand(generator);
            float b = unif_rand(generator);
            ColorRGB c1(r, g, b);
            Assert::IsTrue(Math::IsEqual(c1.R(), r));
            Assert::IsTrue(Math::IsEqual(c1.G(), g));
            Assert::IsTrue(Math::IsEqual(c1.B(), b));
            Assert::IsTrue(Math::IsEqual(c1[0], r));
            Assert::IsTrue(Math::IsEqual(c1[2], b));
            Assert::IsTrue(Math::IsEqual(((const float*)c1)[1], g));

            ColorRGB c2 = c1;
            Assert::IsTrue(c1.GetRGB() == c2.GetRGB());
            Assert::IsTrue(c2 == c1);

            float rgbs[3];
            rgbs[0] = unif_rand(generator);
            rgbs[1] = unif_rand(generator);
            rgbs[2] = unif_rand(generator);
            ColorRGB c3(rgbs);
            ColorRGB c4 = c2 + c3;
            Assert::IsTrue(Math::IsEqual(c4.R(), r + rgbs[0]));
            ColorRGB c5 = c2 - c3;
            Assert::IsTrue(Math::IsEqual(c5.G(), g - rgbs[1]));
            ColorRGB c6 = c2 * c3;
            Assert::IsTrue(Math::IsEqual(c6.B(), b * rgbs[2]));
            ColorRGB c7(c3);
            c7 += c2;
            Assert::IsTrue(Math::IsEqual(c7.R(), r + rgbs[0]));
            Assert::IsTrue(Math::IsEqual(c7.G(), c4.G()));
            ColorRGB c8(c2);
            c8 -= c3;
            Assert::IsTrue(Math::IsEqual(c8.R(), r - rgbs[0]));
            Assert::IsTrue(Math::IsEqual(c8.B(), c5.B()));
            ColorRGB c9(c3);
            c9 *= c2;
            Assert::IsTrue(Math::IsEqual(c9.R(), r * rgbs[0]));
            Assert::IsTrue(Math::IsEqual(c9.G(), c6.G()));
            ColorRGB c10 = 2.0f * c3;
            Assert::IsTrue(Math::IsEqual(c10.R(), 2.0f * rgbs[0]));
            ColorRGB c11(c3);
            c11 *= 2.0f;
            Assert::IsTrue(Math::IsEqual(c11.G(), 2.0f * rgbs[1]));

            ColorRGB c12 = c11.Clamp();
            Assert::IsTrue((c12.R() <= 1.0f) && (c12.R() >= 0.0f));
            Assert::IsTrue((c12.G() <= 1.0f) && (c12.G() >= 0.0f));
            Assert::IsTrue((c12.B() <= 1.0f) && (c12.B() >= 0.0f));

            ColorRGB c13(r + 1.0f, g, b);
            ColorRGB c14 = c13.ScaleByMax();
            Assert::IsTrue(Math::IsEqual(c14.R(), 1.0f));
            Assert::IsTrue(Math::IsEqual(c14.G(), g / (r + 1.0f)));
        }
        TEST_METHOD(ColorRGBATest)
        {
            Assert::IsTrue(ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f) == ColorRGBA::BLACK);
            Assert::IsTrue(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f) == ColorRGBA::WHITE);
            Assert::IsTrue(ColorRGBA(-1.0f, -1.0f, -1.0f, -1.0f) == ColorRGBA::INVALID);
            Assert::IsTrue(ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f) == ColorRGBA::ZERO);

            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<float> unif_rand(0.0f, std::nextafter(2.0f, 2.1f));
            float r = unif_rand(generator);
            float g = unif_rand(generator);
            float b = unif_rand(generator);
            float a = unif_rand(generator);
            ColorRGBA c1(r, g, b, a);
            Assert::IsTrue(Math::IsEqual(c1.R(), r));
            Assert::IsTrue(Math::IsEqual(c1.G(), g));
            Assert::IsTrue(Math::IsEqual(c1.B(), b));
            Assert::IsTrue(Math::IsEqual(c1.A(), a));
            Assert::IsTrue(Math::IsEqual(c1[0], r));
            Assert::IsTrue(Math::IsEqual(c1[2], b));
            Assert::IsTrue(Math::IsEqual(((const float*)c1)[3], a));

            ColorRGBA c2 = c1;
            Assert::IsTrue(c1.GetRGBA() == c2.GetRGBA());
            Assert::IsTrue(c2 == c1);

            float rgbs[4];
            rgbs[0] = unif_rand(generator);
            rgbs[1] = unif_rand(generator);
            rgbs[2] = unif_rand(generator);
            rgbs[3] = unif_rand(generator);
            ColorRGBA c3(rgbs);
            ColorRGBA c4 = c2 + c3;
            Assert::IsTrue(Math::IsEqual(c4.R(), r + rgbs[0]));
            ColorRGBA c5 = c2 - c3;
            Assert::IsTrue(Math::IsEqual(c5.G(), g - rgbs[1]));
            ColorRGBA c6 = c2 * c3;
            Assert::IsTrue(Math::IsEqual(c6.A(), a * rgbs[3]));
            ColorRGBA c7(c3);
            c7 += c2;
            Assert::IsTrue(Math::IsEqual(c7.R(), r + rgbs[0]));
            Assert::IsTrue(Math::IsEqual(c7.G(), c4.G()));
            ColorRGBA c8(c2);
            c8 -= c3;
            Assert::IsTrue(Math::IsEqual(c8.R(), r - rgbs[0]));
            Assert::IsTrue(Math::IsEqual(c8.A(), c5.A()));
            ColorRGBA c9(c3);
            c9 *= c2;
            Assert::IsTrue(Math::IsEqual(c9.A(), a * rgbs[3]));
            Assert::IsTrue(Math::IsEqual(c9.G(), c6.G()));
            ColorRGBA c10 = 2.0f * c3;
            Assert::IsTrue(Math::IsEqual(c10.R(), 2.0f * rgbs[0]));
            ColorRGBA c11(c3);
            c11 *= 2.0f;
            Assert::IsTrue(Math::IsEqual(c11.B(), 2.0f * rgbs[2]));

            ColorRGBA c12 = c11.Clamp();
            Assert::IsTrue((c12.R() <= 1.0f) && (c12.R() >= 0.0f));
            Assert::IsTrue((c12.G() <= 1.0f) && (c12.G() >= 0.0f));
            Assert::IsTrue((c12.B() <= 1.0f) && (c12.B() >= 0.0f));
            Assert::IsTrue((c12.A() <= 1.0f) && (c12.A() >= 0.0f));

            ColorRGBA c13(r + 1.0f, g, b, a);
            ColorRGBA c14 = c13.ScaleByMax();
            Assert::IsTrue(Math::IsEqual(c14.R(), 1.0f));
            Assert::IsTrue(Math::IsEqual(c14.G(), g / (r + 1.0f)));
        }
    };
}