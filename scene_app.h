//-------------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------Scene App----------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <system/application.h>
#include <map>
#include "maths/matrix33.h"
#include "Animation_Parser/2D/Headers/2D_animation_handler.h"
#include "Animation_Parser/3D/Headers/3D_animation_handler.h"
#include "Animation_Parser/3D/Headers/camera.h"
#include "ImGui_handler.h"

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//forward definition
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class InputManager;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Scene App inherits from Application
class SceneApp : public gef::Application
{
public:
	//Basic functions for required functionality
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp(); //Ensure no memory leaks
	bool Update(float frame_time); //Play animation
	void Render();

private:
	void InitFont();
	void gui();
	void DrawHUD(); //display text on the screen
	void SetupLights();

	//Animations handlers to display both 2D and 3D
	AnimationHandler2D m_animation_handler_2D_;
	AnimationHandler3D m_animation_handler_3D_;

	//Rendering 2D sprites
	gef::SpriteRenderer* m_sprite_renderer;
	gef::Renderer3D* m_renderer_3d;
	gef::Font* m_font;
	gef::InputManager* m_input_manager;
	gef::Keyboard* m_key;

	//2D texture atlas
	TextureAtlas m_texture_atlas;

	//Camera for 3D
	Camera camera;

	float m_fps;

	//File and animation names
	std::string image;
	std::string animation;

	//ImGui
	ImGuiHandler* m_imgui;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
