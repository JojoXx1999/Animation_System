//-------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------3D Physics Handler-------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include "ragdoll_handler.h"
#include <chrono>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Forward Declarations
namespace gef
{
	class Keyboard;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class PhysicsHandler3D
{
public:
	PhysicsHandler3D();
	~PhysicsHandler3D();

	//Standard functions for Physics handler
	void Init(std::string filename, gef::SkinnedMeshInstance* player, gef::Renderer3D* renderer_3d, gef::Platform& platform);
	void CleanUp();																				
	void Update(float frame_time, gef::SkeletonPose* clip, float speed);

	inline bool* getRagdollAnimStatus() { return &m_animation_ragdoll_toggle; }		//Is ragdoll being simulated ----------------------------//
	inline bool* getRagdollStatus() { return &m_simulating_toggle; }				//Is physics simulating ---------------------------------//			

	inline void setRagdollAnimStatus(bool s) { m_animation_ragdoll_toggle = s; }	//Set ragdoll on/off ------------------------------------//
	inline void setRagdollStatus(bool s) { m_simulating_toggle = s; }				//Set physics on/off ------------------------------------//

	void setRagdollManual();
	void Render(gef::Renderer3D* renderer_3d);
	void Input(gef::Keyboard* keyboard, float anim_blend);
	inline void setToggle(bool toggle) { m_animation_ragdoll_toggle = toggle; }
	inline void setRagdollSim() { m_ragdoll_handler->setRagdollSimulation(m_simulating_toggle); }

private:
	int i{ 1 };
	RagdollHandler* m_ragdoll_handler;
	gef::SkinnedMeshInstance* m_player_copy;
	bool m_simulating_toggle, m_animation_ragdoll_toggle, key_toggle;
	std::chrono::steady_clock::time_point t_start;

};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
