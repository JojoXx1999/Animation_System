#include "Animation_Parser/3D/Headers/camera.h"
#include "input/keyboard.h"
#include "maths/math_utils.h"

void Camera::Init()
{
	lookSpeed = 4.0f;
	camera_eye_ = gef::Vector4(-1.0f, 1.0f, 4.0f);
	camera_lookat_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_fov_ = gef::DegToRad(45.0f);
	near_plane_ = 0.01f;
	far_plane_ = 1000.f;
}

void Camera::update(gef::Platform& platform)
{
	projection_matrix = platform.PerspectiveProjectionFov(camera_fov_, (float)platform.width() / (float)platform.height(), near_plane_, far_plane_);
	view_matrix.LookAt(camera_eye_, camera_lookat_, camera_up_);
}

void Camera::moveForwardAndBackward(int direction)
{
	speed = direction * frameTime * lookSpeed;
	camera_fov_ -= speed;
}

void Camera::moveUpAndDown(int direction)
{
	speed = direction * frameTime * lookSpeed;
	camera_eye_.set_w(camera_eye_.w() + speed);

}

void Camera::rotateUpAndDown(int direction)
{
	speed = direction * frameTime * lookSpeed;
	camera_lookat_.set_y(camera_lookat_.y() - (speed));
}

void Camera::rotateLeftAndRight(int direction)
{
	speed = direction * frameTime * lookSpeed;
	camera_lookat_.set_x(camera_lookat_.x() - (speed));
}

void Camera::move(gef::Keyboard* keyboard, float dt)
{
	setFrameTime(dt);
	// Handle the input.

	if (keyboard->IsKeyDown(gef::Keyboard::KC_A))
	{
		// Strafe Left


	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_D))
	{
		// Strafe Right
	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_Q))
	{
		// Down
		moveUpAndDown(1);
	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_E))
	{
		// Up
		moveUpAndDown(-1);
	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_UP))
	{
		// rotate up
		rotateUpAndDown(-1);
	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_DOWN))
	{
		// rotate down
		rotateUpAndDown(1);
	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_LEFT))
	{
		// rotate left
		rotateLeftAndRight(1);

	}
	if (keyboard->IsKeyDown(gef::Keyboard::KC_RIGHT))
	{
		// rotate right
		rotateLeftAndRight(-1);
	}
}