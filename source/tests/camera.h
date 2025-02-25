#include <gtest/gtest.h>

#include "../engine/camera/PerspectiveCamera.h"
#include "../engine/camera/OrthographicCamera.h"

namespace CameraUnitTest
{
	TEST(PerspectiveCameraSuite, ConstructorPosition_1)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 0.0f);
		ASSERT_EQ(positionTest.y, 0.0f);
		ASSERT_EQ(positionTest.z, 0.0f);
	}

	TEST(PerspectiveCameraSuite, ConstructorPosition_2)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 1.0f);
		ASSERT_EQ(positionTest.y, 2.0f);
		ASSERT_EQ(positionTest.z, 3.0f);
	}

	TEST(PerspectiveCameraSuite, ConstructorRotation_1)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto rotationTest = camera->GetRotation();
		ASSERT_EQ(rotationTest.x, 0.0f);
		ASSERT_EQ(rotationTest.y, 0.0f);
		ASSERT_EQ(rotationTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorPosition_1)
	{
		OrthographicCamera* camera = new OrthographicCamera(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 0.0f);
		ASSERT_EQ(positionTest.y, 0.0f);
		ASSERT_EQ(positionTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorPosition_2)
	{
		OrthographicCamera* camera = new OrthographicCamera(DirectX::SimpleMath::Vector3(3.0f, 2.0f, 1.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 0.0f);
		ASSERT_EQ(positionTest.y, 0.0f);
		ASSERT_EQ(positionTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorRotation_1)
	{
		OrthographicCamera* camera = new OrthographicCamera(DirectX::SimpleMath::Vector3(3.0f, 2.0f, 1.0f));
		auto rotationTest = camera->GetRotation();
		ASSERT_EQ(rotationTest.x, 0.0f);
		ASSERT_EQ(rotationTest.y, 0.0f);
		ASSERT_EQ(rotationTest.z, 0.0f);
	}
}