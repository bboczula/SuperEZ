#include <gtest/gtest.h>

#include "../engine/camera/Camera.h"
#include "../engine/camera/Camera.h"
#include "../engine/camera/Orbit.h"

namespace CameraUnitTest
{
	TEST(OrbitCameraSuite, RotateYRight90Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, -3.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 0.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYLeft90Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 3.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 0.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYRight180Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 180.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYLeft180Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -180.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYRight360Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 360.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYLeft360Degrees)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -360.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYRight90Degrees_StartOnXAxis)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, 3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotateYLeft90Degrees_StartOnXAxis)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f));
		Orbit* arcball = new Orbit(camera);

		const float ABS_ERROR = 0.000001f;
		arcball->Rotate(0.0f, -90.0f, 0.0f);
		auto positionTest = camera->GetPosition();
		EXPECT_NEAR(positionTest.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(positionTest.z, -3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RadiusLength_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);

		ASSERT_EQ(arcball->GetRadius(), 3.0f);
	}

	TEST(OrbitCameraSuite, RadiusLength_2)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 5.0f));
		Orbit* arcball = new Orbit(camera);

		ASSERT_EQ(arcball->GetRadius(), 5.0f);
	}

	TEST(OrbitCameraSuite, RadiusLength_3)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, -5.0f, 0.0f));
		Orbit* arcball = new Orbit(camera);

		ASSERT_EQ(arcball->GetRadius(), 5.0f);
	}

	TEST(OrbitCameraSuite, RadiusLength_4)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(4.0f, 0.0f, 0.0f));
		Orbit* arcball = new Orbit(camera);

		ASSERT_EQ(arcball->GetRadius(), 4.0f);
	}

	TEST(OrbitCameraSuite, RadiusLength_5)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, -3.0f));
		Orbit* arcball = new Orbit(camera);
		camera->SetPosition(DirectX::SimpleMath::Vector3(0.0f, -6.0f, 0.0f));

		ASSERT_EQ(arcball->GetRadius(), 6.0f);
	}

	TEST(OrbitCameraSuite, SetRadiusLength_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->SetRadius(10.0f);

		ASSERT_EQ(arcball->GetRadius(), 10.0f);
	}

	TEST(OrbitCameraSuite, DefaultRadiusRotation_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->Rotate(0.0f, 90.0f, 0.0f);

		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(arcball->GetRadius(), 3.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, SetRadiusRotation_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->SetRadius(10.0f);
		arcball->Rotate(0.0f, 90.0f, 0.0f);

		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(arcball->GetRadius(), 10.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, RotationSetRadius_1)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->Rotate(0.0f, 90.0f, 0.0f);
		arcball->SetRadius(10.0f);

		ASSERT_EQ(arcball->GetRadius(), 10.0f);
	}

	TEST(OrbitCameraSuite, RotationSetRadius_2)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->SetRadius(10.0f);
		arcball->Rotate(0.0f, 90.0f, 0.0f);

		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(arcball->GetRadius(), 10.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, CameraSmallYawRotation)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->Rotate(0.0f, 10.0f, 0.0f);

		auto pos = camera->GetPosition();
		ASSERT_EQ(pos.y, 0.0f);
	}

	TEST(OrbitCameraSuite, CameraSmallPitchRotation)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->Rotate(10.0f, 0.0f, 0.0f);

		auto pos = camera->GetPosition();
		ASSERT_EQ(pos.x, 0.0f);
	}

	TEST(OrbitCameraSuite, CameraPitchRotationAccumulation)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->Rotate(5.0f, 0.0f, 0.0f);
		arcball->Rotate(5.0f, 0.0f, 0.0f);

		auto totalPitch = camera->GetPitch();
		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(totalPitch, 10.0f, ABS_ERROR);
	}

	TEST(OrbitCameraSuite, CameraYawRotationAccumulation)
	{
		Camera* camera = new Camera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f));
		Orbit* arcball = new Orbit(camera);
		arcball->Rotate(0.0f, 5.0f, 0.0f);
		arcball->Rotate(0.0f, 5.0f, 0.0f);

		auto totalPitch = camera->GetYaw();
		const float ABS_ERROR = 0.000001f;
		EXPECT_NEAR(totalPitch, 10.0f, ABS_ERROR);
	}
}