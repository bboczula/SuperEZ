#include <gtest/gtest.h>

#include "../engine/camera/Camera.h"
#include "../engine/camera/Camera.h"
#include "../engine/camera/Arcball.h"

namespace CameraUnitTest
{
	TEST(ArballCameraSuite, RotateYRight90Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, -3.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 0.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft90Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 3.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 0.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYRight180Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 180.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft180Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -180.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYRight360Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 360.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft360Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -360.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYRight90Degrees_StartOnXAxis)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotateYLeft90Degrees_StartOnXAxis)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f));
		Arcball* arcball = new Arcball(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RadiusLength_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);

		ASSERT_EQ(arcball->GetRadius(), 3.0f);
	}

	TEST(ArballCameraSuite, RadiusLength_2)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 5.0f));
		Arcball* arcball = new Arcball(camera);

		ASSERT_EQ(arcball->GetRadius(), 5.0f);
	}

	TEST(ArballCameraSuite, RadiusLength_3)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, -5.0f, 0.0f));
		Arcball* arcball = new Arcball(camera);

		ASSERT_EQ(arcball->GetRadius(), 5.0f);
	}

	TEST(ArballCameraSuite, RadiusLength_4)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(4.0f, 0.0f, 0.0f));
		Arcball* arcball = new Arcball(camera);

		ASSERT_EQ(arcball->GetRadius(), 4.0f);
	}

	TEST(ArballCameraSuite, RadiusLength_5)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Arcball* arcball = new Arcball(camera);
		camera->SetPosition(DirectX::SimpleMath::Vector3(0.0f, -6.0f, 0.0f));

		ASSERT_EQ(arcball->GetRadius(), 6.0f);
	}

	TEST(ArballCameraSuite, SetRadiusLength_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Arcball* arcball = new Arcball(camera);
		arcball->SetRadius(10.0f);

		ASSERT_EQ(arcball->GetRadius(), 10.0f);
	}

	TEST(ArballCameraSuite, DefaultRadiusRotation_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Arcball* arcball = new Arcball(camera);
		arcball->Rotate(0.0f, 90.0f, 0.0f);

		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(arcball->GetRadius(), 3.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, SetRadiusRotation_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Arcball* arcball = new Arcball(camera);
		arcball->SetRadius(10.0f);
		arcball->Rotate(0.0f, 90.0f, 0.0f);

		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(arcball->GetRadius(), 10.0f, ABS_ERROR);
	}

	TEST(ArballCameraSuite, RotationSetRadius_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Arcball* arcball = new Arcball(camera);
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		arcball->SetRadius(10.0f);

		ASSERT_EQ(arcball->GetRadius(), 10.0f);
	}

	TEST(ArballCameraSuite, RotationSetRadius_2)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Arcball* arcball = new Arcball(camera);
		arcball->SetRadius(10.0f);
		arcball->Rotate(0.0f, 90.0f, 0.0f);

		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(arcball->GetRadius(), 10.0f, ABS_ERROR);
	}
}