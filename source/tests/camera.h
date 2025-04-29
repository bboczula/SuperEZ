#include <gtest/gtest.h>

#include "../engine/camera/Camera.h"
#include "../engine/camera/Arcball.h"

namespace CameraUnitTest
{
	TEST(PerspectiveCameraSuite, ConstructorPosition_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 0.0f);
		ASSERT_EQ(positionTest.y, 0.0f);
		ASSERT_EQ(positionTest.z, 0.0f);
	}

	TEST(PerspectiveCameraSuite, ConstructorPosition_2)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 1.0f);
		ASSERT_EQ(positionTest.y, 2.0f);
		ASSERT_EQ(positionTest.z, 3.0f);
	}

	TEST(PerspectiveCameraSuite, ConstructorRotation_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto rotationTest = camera->GetRotation();
		ASSERT_EQ(rotationTest.x, 0.0f);
		ASSERT_EQ(rotationTest.y, 0.0f);
		ASSERT_EQ(rotationTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorPosition_1)
	{
		Camera* camera = new Camera(0.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 0.0f);
		ASSERT_EQ(positionTest.y, 0.0f);
		ASSERT_EQ(positionTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorPosition_2)
	{
		Camera* camera = new Camera(0.0f, DirectX::SimpleMath::Vector3(3.0f, 2.0f, 1.0f));
		auto positionTest = camera->GetPosition();
		ASSERT_EQ(positionTest.x, 3.0f);
		ASSERT_EQ(positionTest.y, 2.0f);
		ASSERT_EQ(positionTest.z, 1.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorRotation_1)
	{
		Camera* camera = new Camera(0.0f, DirectX::SimpleMath::Vector3(3.0f, 2.0f, 1.0f));
		auto rotationTest = camera->GetRotation();
		ASSERT_EQ(rotationTest.x, 0.0f);
		ASSERT_EQ(rotationTest.y, 0.0f);
		ASSERT_EQ(rotationTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, DefaultWidthAndHeight)
	{
		// Default width and height are 1.0f, if they are 0.0f, there will be a crash
		Camera* camera = new Camera(0.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto width = camera->GetWidth();
		auto height = camera->GetHeight();
		ASSERT_EQ(width, 1.0f);
		ASSERT_EQ(height, 1.0f);
	}

	TEST(OrthographicCameraSuite, SetWidthAndHeight)
	{
		Camera* camera = new Camera(0.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		camera->SetWidth(10.0f);
		camera->SetHeight(5.0f);
		auto width = camera->GetWidth();
		auto height = camera->GetHeight();
		ASSERT_EQ(width, 10.0f);
		ASSERT_EQ(height, 5.0f);
	}
}