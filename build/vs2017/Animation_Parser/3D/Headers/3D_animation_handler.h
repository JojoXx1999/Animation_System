//-------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------3D Animation Handler-----------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include "Load3D.h"

#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <maths/math_utils.h>

#include <ImGui/imconfig.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>

#include "Animation_Parser/3D/Headers/3D_physics_handler.h"
#include "Animation_Parser/3D/Headers/blend_tree.h"

#include <filesystem>
#include <map>
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//Forward Declarations
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class Scene;
	class Skeleton;
	class InputManager;
	class Font;
	class Keyboard;
	class SkinnedMeshInstance;
	class Animation;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
struct Model
{
	//current scene
	gef::Scene* model_scene_;	

	//current mesh, skeelton & player
	gef::Mesh* mesh_;
	gef::Skeleton* skeleton;
	gef::SkinnedMeshInstance* player_;

	//current motion clip player
	blending anim_player_;

	//map of clip players
	std::map<std::string, blending> anim_players_;
	PhysicsHandler3D* physics_handler3D;
	gef::SkeletonPose blended_pose_;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class AnimationHandler3D
{
public:
	inline AnimationHandler3D() {}
	inline ~AnimationHandler3D() {}

	void CleanUp();
	void Init(std::string filename, std::string animation_filename, gef::Platform& platform, gef::Renderer3D& renderer_3d);
	void Update(float frame_time);

	//Load 3D meshes and animations
	void Load(std::string filename, std::string animation_filename, gef::Animation* anim);
	void Render(gef::Renderer3D& renderer_3d, gef::Matrix44 projection_matrix, gef::Matrix44 viewMatrix);
	void gui(std::string& image, std::string& animation);
	void Input(float dt, gef::Keyboard* keyboard);

	//filename for gui
	inline std::string getFilename() { return m_filename; }
	inline void setFilename(std::string filename) {  m_filename = filename; }

	//what animation is playing currently
	inline gef::Animation* getCurrentAnim() { return current_anim_; }

	//initialise the blend tree
	void InitBlendTree();
private:
	//blend two animations together without blend tree
	void Blending(gef::Animation* anim, std::string filename);
	std::string m_filename;
	Load3D load3d;
	std::map<std::string, blending> anim_players_;
	std::map<std::string, gef::Animation*> animations_;
	void getAnimBlend(float& anim_blend);

	blending _anim_players_, tmp;
	gef::Platform* m_platform;
	Model* model3D;

	//how the mesh appears
	float rot_, scale_;

	//how fast animations blend
	float multiplier;

	gef::Animation* current_anim_, *next_anim_, first_anim_;

	std::string model, animation;

	std::map<std::string, Model> model_scenes;

	bool dataToggle;
	blending* blend;
	blending* current;
	float anim_blend_, speed_;

	//file paths to animations
	std::string Anim_1, Anim_2;

	gef::Renderer3D* m_renderer_3d;

	//poses for displaying blended animation
	const gef::SkeletonPose* current_pose, *next_pose;

	//number of animations
	int anims_inplay;
	bool button_toggle;

	//minimum and maximum speeds for animations
	float min_speed, max_speed;
	std::string animation2_input;

	//blend tree for animation
	BlendTree blend_tree;
	bool blend_tree_toggle;

	int speeds[2] = { 0, 0 };

	//How slow can the character walk
	float minimum_transition_speed;

	//filepath to the medial folder
	std::experimental::filesystem::path media_folder;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
