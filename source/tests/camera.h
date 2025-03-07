#include <gtest/gtest.h>

#include "../engine/camera/PerspectiveCamera.h"
#include "../engine/camera/OrthographicCamera.h"
#include "../engine/camera/Arcball.h"

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
		ASSERT_EQ(positionTest.x, 3.0f);
		ASSERT_EQ(positionTest.y, 2.0f);
		ASSERT_EQ(positionTest.z, 1.0f);
	}

	TEST(OrthographicCameraSuite, ConstructorRotation_1)
	{
		OrthographicCamera* camera = new OrthographicCamera(DirectX::SimpleMath::Vector3(3.0f, 2.0f, 1.0f));
		auto rotationTest = camera->GetRotation();
		ASSERT_EQ(rotationTest.x, 0.0f);
		ASSERT_EQ(rotationTest.y, 0.0f);
		ASSERT_EQ(rotationTest.z, 0.0f);
	}

	TEST(OrthographicCameraSuite, DefaultWidthAndHeight)
	{
		OrthographicCamera* camera = new OrthographicCamera(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
		auto width = camera->GetWidth();
		auto height = camera->GetHeight();
		ASSERT_EQ(width, 0.0f);
		ASSERT_EQ(height, 0.0f);
	}

	TEST(ArballCameraSuite, RotateYRight90Degrees)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 3.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 0.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft90Degrees)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, -3.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 0.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYRight180Degrees)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -180.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft180Degrees)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 180.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYRight360Degrees)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -360.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft360Degrees)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 360.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYRight90Degrees_StartOnXAxis)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft90Degrees_StartOnXAxis)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}
}