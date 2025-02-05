#pragma once

namespace Sapphire
{
	class Camera;

	class CameraControler
	{
	public:
		// Just be able to set the camera you want to work on
		void SetCamera(Camera* camera)
		{
			this->camera = camera;
		}
		// Move
		virtual void MoveForward(float step) = 0;
		virtual void MoveBackward(float step) = 0;
		virtual void MoveRight(float step) = 0;
		virtual void MoveLeft(float step) = 0;
		// Rotate
		virtual void Rotate(float x, float y, float z) = 0;
	protected:
		// Pointer to the underlaying camera
		Camera* camera;
	};
}