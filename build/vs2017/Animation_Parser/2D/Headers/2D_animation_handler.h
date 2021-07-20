//-------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------2D Animation Handler------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include "load_json.h"
#include "animation_texture.h"

//ImGui Files contained within gef_abertay -> ImGui
#include <ImGui/imconfig.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Forward declaration
namespace gef
{
	class SpriteRenderer;
	class InputManager;
	class Keyboard;
	class SpriteRenderer;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class AnimationHandler2D
{
public:
	inline AnimationHandler2D() {}														//Default Constructor -------------------------------//
	inline ~AnimationHandler2D() {}														//Default Deconstructor -----------------------------//

	bool Init(std::string filepath, gef::Platform& platform);							//Set up 2D animation handler -----------------------//
	bool Update(float frame_time);														//Update the 2D animation ---------------------------//
	void Render(gef::SpriteRenderer* sprite_renderer);									//Render the animation to the screen ----------------//
	void Input(float dt, gef::Keyboard* keyboard);										//Get user input ------------------------------------//
	void gui(gef::Platform& platform);													//Display 2D animation gui --------------------------//
	void CleanUp();																		//Prevent memory leaks by removing old objects ------//
	void setStatus(bool b);																//Determine if file is valid for 2D -----------------//
	inline bool getStatus() { return status; }											//For error handling, if false try loading as 3D ----//
	bool LoadFile(std::string img);														//Load the 2D animation and texture -----------------//
	inline std::string getFilename() { return m_filename; }								//Return file name so it is displayed on the gui ----//
	inline void setFilename(std::string filename) { m_filename = filename; }			//Set the file name when user changes the animation -//
	inline bool getToggle() { return toggle; }											//Debounce for changing animation through button ----//
	inline void setToggle(bool t) { toggle = t; }										//Debounce for changing animation through button ----//
																																			
private:
	gef::Platform* m_platform;															//Application Platform ------------------------------//
	gef::Sprite m_sprite;																//2D Sprite Object to be rendered -------------------//
	std::string m_filename;																//Filename of 2D animation for gui ------------------//

	bool toggle;																		//Debounce for switching between sprite and rig -----//

	//Loading data from json files
	TextureAtlas m_texture_atlas;														//Current 2D texture atlas --------------------------//
	LoadJSON m_loadJson;																//Access functions to load data from json files -----//
	ArmitureAnimation m_armature_anim;													//Access function to load in animation data ---------//
	std::vector<std::string> m_sprite_animations;										//List of sprite animations read from json file -----//

	//set up and update the animation
	AnimationTexture m_anim;															//Texture to use for the animation ------------------//
	bool status;																		//Is a valid 2D file --------------------------------//
	enum animationType { SPRITE = 0, RIG };												//Possible types of 2D animations -------------------//
	animationType anim_type;															//Current type of 2D animation ----------------------//
	std::string image, imageTexJson, imageSkeJson;										//Image file and it's texture/skeleton json files ---//
	int m_frame;																		//Current frame of the animation --------------------//


	//-----------------------------------------------------------Rig Only Variables----------------------------------------------------------//
	float scale;																		//Scale of the texture ------------------------------//
	std::list<std::string> parts;														//list 2D bone componants keeping insertion order ---//
	std::string currentAnim;															//Name of the current animation being played --------//
	std::string current_rig_texture_atlas;												//Current texture atlas name ------------------------//
	float m_rig_position_x, m_rig_position_y, m_speed, m_jumpSpeed, m_maxJumpSpeed;     //Rig positions and speed for moving ----------------//
	float elapsed_time, sprite_speed;													//Speed of the animation ----------------------------//
	float maxJump, groundLevel;															//Max and min heights for the rig animation ---------//
	bool m_flipped, isJumping, onGround;												//Variables for Rig moving and jumping --------------//
	gef::Matrix33 m_rig_transform, m_combined_transform;							    //Transforms for rendering correctly on the screen --//
	//---------------------------------------------------------Sprite Only Variables---------------------------------------------------------//
	float m_animation_timer;														    //Animation fps -------------------------------------//
	int m_playing_animation;															//Current playing animation int used for map keys ---//
	int m_random;																		//Random number to play a random animation in atlas -//
	std::string current_sprite_texture_atlas;											//Current texture atlas name ------------------------//
    //---------------------------------------------------------------------------------------------------------------------------------------//


	/* Textures and atlases that have been loaded previously
	are saved, so that their data can be extracted if the user
	wishes to use these again at a later data. Therefore these
	do not have to be recreated each time saving memory*/
	std::map<std::string, gef::Sprite> loadedTextures;									//Store all loaded textures to reduce memory costs --//
	std::map<std::string, TextureAtlas> loadedAtlases;									//Store all loaded atlases to reduce memory costs ---//

};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//