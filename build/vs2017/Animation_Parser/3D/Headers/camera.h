//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------Camera------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include "Load3D.h"
#include <directxmath.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

using namespace DirectX;

namespace gef
{
	class Platform;
	class Keyboard;
}

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class Camera
{
public:
	inline Camera() {}
	inline ~Camera() {}

	void Init();

	void update(gef::Platform& platform);

	inline void setFrameTime(float t) { frameTime = t; }

	void moveForwardAndBackward(int direction);
	void moveUpAndDown(int direction);
	void rotateUpAndDown(int direction);
	void rotateLeftAndRight(int direction);

	void move(gef::Keyboard* keyboard, float dt);
	inline gef::Matrix44 getProjectionMatrix() { return projection_matrix; }
	inline gef::Matrix44 getViewMatrix() { return view_matrix; }

private:
	float speed, frameTime;
	float lookSpeed;

	gef::Platform* m_platform;
	gef::Matrix44 projection_matrix;
	gef::Matrix44 view_matrix;

	gef::Vector4 camera_eye_;
	gef::Vector4 camera_lookat_;
	gef::Vector4 camera_up_;
	float camera_fov_;
	float near_plane_;
	float far_plane_;

};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
