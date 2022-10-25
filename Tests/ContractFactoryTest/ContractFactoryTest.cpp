#include "pch.h"
#include "CppUnitTest.h"
#include "MathLib/MathGlobal.h"
#include "MathLib/Vector3.h"
#include "MathLib/Box3.h"
#include "GameEngine/BoundingVolume.h"
#include "GameEngine/BoundingVolumeContract.h"
#include "Gateways/ContractJsonGateway.h"
#include <cmath>
#include <random>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;

namespace ContractFactoryTest
{
    TEST_CLASS(ContractFactoryTest)
    {
    public:

        TEST_METHOD(TestBoundingVulome)
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
            Box3 box0 = Box3(center, v1, v2, v3, ex, ey, ez);
            BoundingVolume bv0(box0);
            BoundingVolumeContract bv_contract0 = bv0.SerializeContract();
            BoundingVolume bv0_1(bv_contract0);
            Assert::IsTrue(bv0.BoundingBox3().value() == bv0_1.BoundingBox3().value());
            std::string json = ContractJsonGateway::Serialize({ bv_contract0.ToContract() });
            auto contracts = ContractJsonGateway::Deserialize(json);
            Assert::IsTrue(contracts.size() == 1);
            BoundingVolume bv0_2(BoundingVolumeContract::FromContract(contracts[0]));
            Assert::IsTrue(bv0.BoundingBox3().value() == bv0_2.BoundingBox3().value());
        }
    };
}
