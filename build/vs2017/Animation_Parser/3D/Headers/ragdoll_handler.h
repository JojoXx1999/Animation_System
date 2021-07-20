//-------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------Ragdoll Handler--------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <graphics/skinned_mesh_instance.h>
#include "primitive_builder.h"
#include "primitive_renderer.h"
#include <graphics/renderer_3d.h>
#include "ragdoll.h"
#include <graphics/sprite.h>
#include <vector>
#include "motion_clip_player.h"
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
struct blending : MotionClipPlayer
{
	float speed_min_, speed_max_, speed_, speed_mod_;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class RagdollHandler
{
public:
	RagdollHandler();
	~RagdollHandler();

	//Intilialise the ragdoll
	void InitRagdoll(std::string filename, gef::SkinnedMeshInstance* player, gef::Renderer3D* renderer_3d, gef::Platform& platform);

	//intiailise physics
	void InitPhysicsWorld(gef::Renderer3D* renderer_3d);
	void UpdatePhysicsWorld(float delta_time);

	void CreateRigidBodies();
	void UpdateRigidBodies();
	void Render(gef::Renderer3D* renderer_3d);
	void CleanUp();
	void Update(gef::SkinnedMeshInstance* player, gef::SkeletonPose* clip_player, bool toggle, float speed);

	//is physics simulating
	void setRagdollSimulation(bool toggle) { m_is_ragdoll_simulating = toggle; }

private:
	void CleanUpRagdoll();
	void CleanUpRigidBodies();
	void CleanUpPhysicsWorld();

	Ragdoll* m_ragdoll;
	bool m_is_ragdoll_simulating;

	PrimitiveBuilder* primitive_builder_;
	PrimitiveRenderer* primitive_renderer_;

	btDiscreteDynamicsWorld* dynamics_world_;
	btSequentialImpulseConstraintSolver* solver_;
	btBroadphaseInterface* overlapping_pair_cache_;
	btCollisionDispatcher* dispatcher_;
	btAlignedObjectArray<btCollisionShape*> collision_shapes_;


	gef::Mesh* floor_mesh_;
	gef::MeshInstance floor_gfx_;

	btRigidBody* sphere_rb_;
	gef::Mesh* sphere_mesh_;
	gef::MeshInstance sphere_gfx_;

};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//