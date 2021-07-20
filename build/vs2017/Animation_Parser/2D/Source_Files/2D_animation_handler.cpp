//-------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------2D Animation Handler------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#include "Animation_Parser/2D/Headers/2D_animation_handler.h"
#include "input/input_manager.h"
#include "input/keyboard.h"
#include <graphics/sprite_renderer.h>
#include <random>
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationHandler2D::CleanUp()
{
	m_anim.CleanUp();															//Clean up 2D animation handler -----------------------------//
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
bool AnimationHandler2D::Init(std::string filepath, gef::Platform& platform)
{
	toggle = false;																//Debounce for switching between rig and sprite -------------//

	m_platform = &platform;														//Create variable to point to the application platform ------//
	m_filename = filepath;														//Copy of the filename --------------------------------------//

	/*Attempt to load the file
	if the loading failed set status to false
	otherwise the status will be true*/
	if (!LoadFile(filepath)) {
		setStatus(false);
		return false; 
	}
	else setStatus(true);

	m_frame = m_animation_timer = m_random = m_playing_animation = 0;			//Set animation to start ------------------------------------//
	scale = 0.5f;																//Scale for the sprite --------------------------------------//

	//Booleans for jumping
	m_flipped = isJumping = false;				
	onGround = true;
	groundLevel = 100;															//Height of the ground --------------------------------------//

	//Initial Rig properties 
	m_rig_position_x = 0; m_rig_position_y = groundLevel;
	m_speed = 250;

	//Speed calculated from frame rate
	sprite_speed = 1 / (m_texture_atlas.armature_details_.at(0).frame_rate_);

	maxJump = m_rig_position_y - 120;											//Maximum height the rig can jump ---------------------------//
	m_maxJumpSpeed = 900.f;														//Acceleration when jumping, maximum speed ------------------//
	m_jumpSpeed = m_maxJumpSpeed;												//Current jump speed is maximum -----------------------------//

	current_rig_texture_atlas = "Dragon";										//Initial Rig file to be loaded -----------------------------//
	current_sprite_texture_atlas = "girl_and_boy";								//Initial Sprite file to be loaded --------------------------//
	currentAnim = "stand";														//Initial Rig animation to play -----------------------------//

	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
bool AnimationHandler2D::LoadFile(std::string img)
{
	image = img;																//Get file name to be loaded --------------------------------//
	imageTexJson = image + "_tex.json";											//Get name of the texture json ------------------------------//
	imageSkeJson = image + "_ske.json";											//Get name of the skeleton json -----------------------------//


	/*If the file being loaded has not been loaded previously
	Load the file and save it's data into a map
	If the file has been loaded already then load in the data
	in the map*/
	if (loadedAtlases.find(image) == loadedAtlases.end()) {					
		if (m_loadJson.testJson(imageTexJson) == false)
		{
			imageTexJson = image + "_tex.json";
			imageSkeJson = image + "_ske.json";
			return false;
		}

		m_texture_atlas = m_loadJson.LoadJson(imageTexJson, imageSkeJson);
		loadedAtlases.insert(std::pair<std::string, TextureAtlas>(image, m_texture_atlas));
		
	}
	else
		m_texture_atlas = loadedAtlases.find(image)->second;

	//If the armature has more than 1 bone then set animation type to rig, otherwise it is a sprite
	(m_texture_atlas.armature_details_.at(0).bone_.size() > 1 ? anim_type = animationType::RIG : anim_type = animationType::SPRITE);



	//Getting part of the texture and filepath from the texture atlas 
	parts = m_texture_atlas.armature_details_.at(0).slots_;
	std::string part_name;

	if (anim_type == RIG)
	{
		//Get the name of the texture atlas
		current_rig_texture_atlas = &m_texture_atlas.frame_name[0];

		//Get the name of the rig part
		part_name = m_texture_atlas.armature_details_.find(0)->second.skin_slots_.find(parts.front())->second.part_name_;
	}
	else
	{
		//Getting part of the texture and filepath from the texture atlas 
		current_sprite_texture_atlas = &m_texture_atlas.frame_name[0];

		//Get the name of the first skin slot
		part_name = m_texture_atlas.armature_details_.find(m_playing_animation)->second.skin_slots_.find("0")->second.name_;

		//If there are loaded sprite animations
		if (m_sprite_animations.size() > 0)
		{
			//If the current texture is the one stored in the map then empty the animation map
			if (m_texture_atlas.armature_details_.find(0)->second.name_ != m_sprite_animations.at(0))
			{
				m_sprite_animations.clear();
			}
		}

		//If the animation map is empty
		if (m_sprite_animations.size() == 0)
		{
			//Save all animations from the json file into the animation map
			for (int i = 0; i < m_texture_atlas.armature_details_.size(); ++i)
			{
				m_sprite_animations.push_back(m_texture_atlas.armature_details_.find(i)->second.name_);
			}
		}
	}

	//Get the file path from the json file
	char* imageFilePath = const_cast<char*>(m_texture_atlas.file_path_.c_str());

	/*If the current file has not been loaded in before
	Intiliase the texture and store it in a map otherwise
	load the texture from the map*/
	if (loadedTextures.find(image) == loadedTextures.end()) {
		m_sprite = m_anim.InitTexture(imageFilePath, *m_platform, m_texture_atlas, part_name, true);
		loadedTextures.insert(std::pair<std::string, gef::Sprite>(image, m_sprite));
	}
	else
		m_sprite = loadedTextures.find(image)->second;

	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationHandler2D::setStatus(bool b)
{
	status = b;																	//Set the status to the inputted boolean --------------------//
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
bool AnimationHandler2D::Update(float frame_time)
{
	if (anim_type == SPRITE)
	{
		/* play animation by looping through all the frames in the current animation using the sprite speed*/
		m_animation_timer += frame_time;
		if (m_animation_timer >= (1.f / (m_texture_atlas.armature_details_.at(m_playing_animation).duration_)))
		{
			m_animation_timer = 0;
			m_frame++;
		}
		if (m_frame >= (m_texture_atlas.armature_details_.at(m_playing_animation).duration_))
		{
			m_frame = 0;
		}
	}

	if (anim_type == RIG) {
		/*Update the rig animation, if the user is jumping then move the character upwards playing the jump animation
		speed decreases as the character goes upwards then accelerate and okay the fall animation as the character
		falls back downwards*/
		m_texture_atlas.armature_details_.at(0).UpdateAnimation(&m_texture_atlas.armature_details_.at(0).animation_, &currentAnim, sprite_speed, frame_time);
		if (isJumping)
		{
			if (onGround)
			{
				if (m_jumpSpeed > 200) m_jumpSpeed -= (m_jumpSpeed / 9.81);

				currentAnim = "jump";
				if (m_rig_position_y > maxJump) m_rig_position_y -= m_jumpSpeed * frame_time;
				if (m_rig_position_y <= maxJump) { onGround = false; };
			}
			else
			{
				if (m_jumpSpeed < m_maxJumpSpeed) m_jumpSpeed += (m_jumpSpeed / 9.81);
				currentAnim = "fall";
				if (m_rig_position_y < groundLevel) m_rig_position_y += m_jumpSpeed * frame_time;
				if (m_rig_position_y >= groundLevel) { m_jumpSpeed = m_maxJumpSpeed; m_rig_position_y = groundLevel; onGround = true; isJumping = false; currentAnim = "stand"; }
			}
		}
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationHandler2D::Render(gef::SpriteRenderer* sprite_renderer)
{
	//Set up transform matrix for scale and position of rig
	m_rig_transform.SetIdentity();
	(m_flipped == false ? m_rig_transform.Scale(gef::Vector2(scale, scale)) : m_rig_transform.Scale(gef::Vector2(-scale, scale)));
	m_rig_transform.SetTranslation(m_anim.getSpritePosition() + gef::Vector2(m_rig_position_x, m_rig_position_y));

	if (anim_type == RIG) {
		//For every part contained in the rig
		for each(std::string part in parts)
		{

			gef::Matrix33 sprite_offset_transform = m_texture_atlas.armature_details_.at(0).skin_slots_.find(part)->second.m_transform;


			gef::Matrix33 world_bone_transform = m_texture_atlas.armature_details_.at(0).bone_.find(part)->second.m_world_transform;
			std::string part_name = m_texture_atlas.armature_details_.at(0).skin_slots_.find(part)->second.part_name_;
			gef::Matrix33 m_sub_texture_transform = m_texture_atlas.sub_textures_.find(part_name)->second.m_transform;
			gef::Matrix33 m_local_bone_transform = m_texture_atlas.armature_details_.at(0).bone_.find(part)->second.m_local_transform;


			//Update the sprite
			m_anim.Update(&m_texture_atlas, &m_sprite, part_name);


			//Draw sprite to the screen with correct position and rotation using matrices
			sprite_renderer->DrawSprite(m_sprite, m_sub_texture_transform
												* sprite_offset_transform
												   * world_bone_transform
														* m_rig_transform
			);
		}
	}
	else
	{
		m_anim.Update(&m_texture_atlas, &m_sprite, m_texture_atlas.armature_details_.at((int)m_playing_animation).skin_slots_.find(std::to_string(m_frame))->second.name_);
		sprite_renderer->DrawSprite(m_sprite);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationHandler2D::Input(float dt, gef::Keyboard* keyboard)
{
	if (anim_type == RIG) {
		if (keyboard->IsKeyDown(gef::Keyboard::KC_RIGHT))
		{
			/*Move the character to the right and play
			the walking animation*/
			if (isJumping == false) currentAnim = "walk";
			if (m_flipped == false) m_flipped = true;
			m_rig_position_x += m_speed * dt;

		}
		else if (keyboard->IsKeyDown(gef::Keyboard::KC_LEFT))
		{
			/*Move the character to the left and play
			the walking animation*/
			if (isJumping == false) currentAnim = "walk";
			if (m_flipped == true) m_flipped = false;
			m_rig_position_x -= m_speed * dt;
		}

		if (keyboard->IsKeyReleased(gef::Keyboard::KC_RIGHT))
		{
			/*If the player stops moving and is not jumping
			set the animation to stand/idle*/
			if (isJumping == false) currentAnim = "stand";
		}
		else if (keyboard->IsKeyReleased(gef::Keyboard::KC_LEFT))
		{
			/*If the player stops moving and is not jumping
			set the animation to stand/idle*/
			if (isJumping == false) currentAnim = "stand";
		}

		if (keyboard->IsKeyDown(gef::Keyboard::KC_UP))
		{
			//Increase the scale of the rig
			scale += 0.05;
		}
		else if (keyboard->IsKeyDown(gef::Keyboard::KC_DOWN))
		{
			//Decrease the scale of the rig
			if (scale >= 0.05) scale -= 0.05;
		}

		if (keyboard->IsKeyDown(gef::Keyboard::KC_SPACE))
		{
			//If player is not already jumping then make them jump
			if (isJumping == false)
			{
				isJumping = true;
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationHandler2D::gui(gef::Platform& platform)
{

	if (anim_type == RIG)
	{
		/*If the user selects sprite animation change to the loaded sprite animation*/
		if (ImGui::Button("Sprite Animation"))
		{
				toggle = true;
				m_filename = &current_sprite_texture_atlas[0];
				anim_type == SPRITE;
				m_frame = 0;
				LoadFile(&current_sprite_texture_atlas[0]);
		}

		//If the user presses the Walk button set the animation to walk
		if (ImGui::Button("Walk"))
		{
			currentAnim = "walk";
		}

		//If the user presses the Idle button set the animation to walk
		if (ImGui::Button("Idle"))
		{
			currentAnim = "stand";
		}

		//If the user presses the Jump button set the jumping boolean to true
		if (ImGui::Button("Jump"))
		{
			isJumping = true;
		}

		float tmp = -maxJump;
		if (ImGui::InputFloat("Max Jump Height", &tmp, 1.f, 5.f, "%.2f"))
		{
			maxJump = -tmp;
		}

		tmp = groundLevel;
		if (ImGui::InputFloat("Ground Level", &groundLevel, 1.f, 5.f, "%.2f",  ImGuiInputTextFlags_EnterReturnsTrue))
		{
			isJumping = true;
		}
	}

	if (anim_type == SPRITE)
	{
		/*If the user selects rig animation change to the loaded rig animation*/
		if (ImGui::Button("Rig Animation"))
		{
			toggle = true;
			m_filename = &current_rig_texture_atlas[0];
			anim_type == RIG;
			m_frame = 0;
			LoadFile(&current_rig_texture_atlas[0]);
		}

		/*If the user selects next animation increase the playing animation by 1
		if this is greater than the number of animations then set playing animation
		back to the start (0)*/
		if (ImGui::Button("Next Animation From Texture Atlas"))
		{	
			if(m_playing_animation + 1 <= m_sprite_animations.size()-1)
				m_playing_animation = ((int)(m_playing_animation + 1));
			else
				m_playing_animation = (0);
		}

		/*If the user selects previous animation decrease the playing animation by 1
		if this is less than 0 then set playing animation
		to the end of the animations*/
		if (ImGui::Button("Previous Animation From Texture Atlas"))
		{
			if (m_playing_animation - 1 >= 0)
				m_playing_animation = ((int)(m_playing_animation - 1));
			else
				m_playing_animation = ((int)(m_sprite_animations.size()-1));
		}

		/*If the user selects random then generate a random number between 0 and
		the number of animations loaded from the json
		If the number of animations is greater than 1 then ensure random animation
		is not the same as the animation currently playing
		set frame back to 0*/
		if (ImGui::Button("Random From Texture Atlas"))
		{
			std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distr(0, m_sprite_animations.size()-1);

				if (m_sprite_animations.size() > 1)
				{
					while (m_playing_animation == (m_random))
						m_random = (distr(gen));
				}

				m_playing_animation = (m_random);
				m_frame = 0;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
