#include "scene_app.h"
#include <system/platform.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <input/sony_controller_input_manager.h>
#include <string>
#include <string.h>
#include <graphics/sprite_renderer.h>
#include <input/input_manager.h>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Create the scene and platform
SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	m_sprite_renderer(NULL),
	m_input_manager(NULL),
	m_font(NULL),
	camera()
{
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Variable initialisation
void SceneApp::Init()
{
	camera.Init();
	m_imgui = new ImGuiHandler(image, m_animation_handler_2D_, m_animation_handler_3D_, animation, platform_);
	m_renderer_3d = gef::Renderer3D::Create(platform_);

	InitFont();
	SetupLights();

	//Create sprite renderer and input manager using gef framework
	m_sprite_renderer = gef::SpriteRenderer::Create(platform_);
	m_input_manager = gef::InputManager::Create(platform_);

	//default 3D mesh and animation
	image = "xbot/xbot";
	animation = "xbot/walking_inplace";
	m_animation_handler_3D_.Init(image, animation, platform_, *m_renderer_3d);

	//default 2D texture
	image = "Dragon";

	if (m_animation_handler_2D_.Init(image, platform_) == false)
		m_imgui->modelType = m_imgui->ModelType::D3;
	else
		m_imgui->modelType = m_imgui->ModelType::D2;

}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Destroy objects to avoid memory leaks
void SceneApp::CleanUp()
{
	//m_animation_handler_2D_.CleanUp();
	m_animation_handler_3D_.CleanUp();

	delete m_input_manager;
	m_input_manager = NULL;

	delete m_sprite_renderer;
	m_sprite_renderer = NULL;

	delete m_font;
	m_font = NULL;

	delete m_imgui;
	m_imgui = NULL;

}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Update the scene every frame
bool SceneApp::Update(float frame_time)
{
	gef::Keyboard* keyboard = m_input_manager->keyboard();

	//Frame rate
	m_fps = 1.0f / frame_time;

	m_input_manager->Update();

	if (m_imgui->modelType == m_imgui->D2)
	{
		m_animation_handler_2D_.Update(frame_time);
		m_animation_handler_2D_.Input(frame_time, keyboard);
	}
	else
	{
		m_animation_handler_3D_.Update(frame_time);
		m_animation_handler_3D_.Input(frame_time, keyboard);
	}

	camera.update(platform_);
	camera.move(keyboard, frame_time);

	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void SceneApp::Render()
{
	//Render start---------------------------------------------
	m_sprite_renderer->Begin();

	if (m_imgui->modelType == m_imgui->D2)
		m_animation_handler_2D_.Render(m_sprite_renderer);
	else
		m_animation_handler_3D_.Render(*m_renderer_3d, camera.getProjectionMatrix(), camera.getViewMatrix());

	gui();

	DrawHUD();
	m_sprite_renderer->End();

	//Render end---------------------------------------------
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Set up font for text displayed on screen
void SceneApp::InitFont()
{
	m_font = new gef::Font(platform_);
	m_font->Load("comic_sans");
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Display text on screen
void SceneApp::gui()
{

	m_imgui->MenuBar();

}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Display text on screen
void SceneApp::DrawHUD()
{
	if (m_font)
	{
		m_font->RenderText(m_sprite_renderer, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", m_fps);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

void SceneApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));


	gef::Default3DShaderData& default_shader_data = m_renderer_3d->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}



