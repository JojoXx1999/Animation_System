#include "Animation_Parser/3D/Headers/3D_physics_handler.h"
#include <input/keyboard.h>
#include <graphics/skinned_mesh_instance.h>
#include <animation/skeleton.h>
#include <animation/animation.h>
#include <time.h>

PhysicsHandler3D::PhysicsHandler3D()
{
	m_simulating_toggle = false;
	m_animation_ragdoll_toggle = false;
	key_toggle = false;
	m_ragdoll_handler = new RagdollHandler();
	setRagdollSim();

	t_start = std::chrono::high_resolution_clock::now();
}

PhysicsHandler3D::~PhysicsHandler3D()
{

}

void PhysicsHandler3D::Init(std::string filename, gef::SkinnedMeshInstance* player, gef::Renderer3D* renderer_3d, gef::Platform& platform)
{
	m_ragdoll_handler->InitRagdoll(filename, player, renderer_3d, platform);
	m_player_copy = player;

	gef::Matrix44 player_transform;
	const float scale = 0.01f;
	player_transform.Scale(gef::Vector4(scale, scale, scale));
	player->set_transform(player_transform);
}

void PhysicsHandler3D::CleanUp()
{
	m_ragdoll_handler->CleanUp();

	if (m_ragdoll_handler)
	{
		delete m_ragdoll_handler;
		m_ragdoll_handler = NULL;
	}
}

void PhysicsHandler3D::Update(float frame_time, gef::SkeletonPose* clip, float speed)
{
	//update physics, rigid body and the ragdoll handler
		m_ragdoll_handler->UpdatePhysicsWorld(frame_time);
		m_ragdoll_handler->UpdateRigidBodies();
		m_ragdoll_handler->Update(m_player_copy, clip, m_animation_ragdoll_toggle, speed);
}

void PhysicsHandler3D::Input(gef::Keyboard* keyboard, float anim_blend)
{
	if (keyboard)
	{
		//toggle ragdoll on and off (including physics)
		if (keyboard->IsKeyPressed(gef::Keyboard::KC_SPACE))
		{
			m_animation_ragdoll_toggle = true;
			m_simulating_toggle = !m_simulating_toggle;
			setRagdollSim();
		}
		if (keyboard->IsKeyReleased(gef::Keyboard::KC_SPACE))
		{
			m_animation_ragdoll_toggle = false;
		}
	}
}

void PhysicsHandler3D::Render(gef::Renderer3D* renderer_3d)
{
	m_ragdoll_handler->Render(renderer_3d);
}

void PhysicsHandler3D::setRagdollManual()
{
	//simulate ragdoll physics
	m_ragdoll_handler->setRagdollSimulation(m_simulating_toggle);
}
