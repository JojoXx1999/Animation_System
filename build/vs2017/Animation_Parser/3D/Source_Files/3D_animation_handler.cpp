#include "Animation_Parser/3D/Headers/3D_animation_handler.h"


#include <graphics/renderer_3d.h>
#include <graphics/font.h>
#include <input/keyboard.h>

#include <iostream>
#include <system/debug_log.h>

void AnimationHandler3D::CleanUp()
{
	//clean up pointers
	if (model3D->player_) {
		delete model3D->player_;
		model3D->player_ = NULL;
	}

	if (model3D->mesh_) {
		delete model3D->mesh_;
		model3D->mesh_ = NULL;
	}

	if (model3D->model_scene_) {
		delete model3D->model_scene_;
		model3D->model_scene_ = NULL;
	}
	model3D->physics_handler3D->CleanUp();

	if (blend) {
		delete blend;
		blend = NULL;
	}

	if (model3D->physics_handler3D)
	{
		delete model3D->physics_handler3D;
		model3D->physics_handler3D = NULL;
	}

	if (model3D)
	{
		delete model3D;
		model3D = NULL;
	}
}

void AnimationHandler3D::Init(std::string filename, std::string animation_filename, gef::Platform& platform, gef::Renderer3D& renderer_3d)
{
	m_renderer_3d = &renderer_3d;
	m_platform = &platform;
	dataToggle = false;
	button_toggle = false;
	multiplier = 1.f;
	rot_ = 15;
	scale_ = 0.3;
	speed_ = 1.f;
	anim_blend_ = 0.f;

	anims_inplay = 0;
	minimum_transition_speed = 0;
	media_folder = std::experimental::filesystem::current_path();
	std::string s = "xbot/running_inplace";

	Anim_1 = media_folder.u8string() + "/" + animation_filename;
	Anim_2 = media_folder.u8string() + "/" + s;
	filename = media_folder.u8string() + "/" + filename;
	animation2_input = Anim_2;
	m_filename = filename;
	model3D = new Model();
	current_pose = new gef::SkeletonPose();
	next_pose = new gef::SkeletonPose();

	Load(filename, Anim_2, next_anim_);
	Load(filename, Anim_1, current_anim_);

	blend_tree_toggle = false;

	InitBlendTree();
}

void AnimationHandler3D::Update(float frame_time)
{

	anims_inplay = 0;
	if (model3D->player_)
	{
		for (std::map<std::string, blending>::iterator it = anim_players_.begin(); it != anim_players_.end(); ++it)
		{
			//update all the animations
			it->second.Update(frame_time, model3D->player_->bind_pose());
		}

		getAnimBlend(anim_blend_);

		//sync up timings of the animations to avoid jumping
		if (anim_players_.find(&Anim_1[0]) != anim_players_.end())
		{
			if (anim_players_.find(&Anim_2[0]) != anim_players_.end())
			{
				auto tmp = &anim_players_.find(&Anim_1[0])->second;


				tmp->speed_mod_ = (tmp->speed_max_ - tmp->speed_min_) * anim_blend_;

				tmp->set_playback_speed(tmp->speed_min_ + tmp->speed_mod_);
			}
		}
		if (anim_players_.find(&Anim_2[0]) != anim_players_.end())
		{
			if (anim_players_.find(&Anim_1[0]) != anim_players_.end())
			{
				auto tmp = &anim_players_.find(&Anim_2[0])->second;
				tmp->speed_mod_ = (tmp->speed_max_ - tmp->speed_min_) * anim_blend_;

				tmp->set_playback_speed(tmp->speed_min_ + tmp->speed_mod_);
			}
		}


		if (blend_tree_toggle == false)
		{
			//use default blending
			model3D->blended_pose_.Linear2PoseBlend(*current_pose, *next_pose, anim_blend_);
			model3D->player_->UpdateBoneMatrices(model3D->blended_pose_);
		}
		else
		{
			//use blend tree
			blend_tree.m_variables["walk_run_blend"] = anim_blend_;
			blend_tree.Update(frame_time);
			model3D->blended_pose_ = blend_tree.m_output->m_output_pose;
			model3D->player_->UpdateBoneMatrices(model3D->blended_pose_);
		}
		
	}


	// build a transformation matrix that will position the character
// use this to move the player around, scale it, etc.
	if (model3D->player_)
	{

		gef::Matrix44 player_transform, player_scale, player_rotation, player_translate;
		player_transform.SetIdentity();
		player_scale.SetIdentity();
		player_rotation.SetIdentity();
		player_translate.SetIdentity();
		player_scale.Scale(gef::Vector4(scale_, scale_, scale_, 1));
		player_rotation.RotationY(gef::DegToRad(rot_));
		player_translate.SetTranslation(gef::Vector4(25, -20, -100, 1));
		player_transform = player_scale * player_rotation * player_translate;
		model3D->player_->set_transform(player_transform);

		if (anim_players_.find(&Anim_1[0]) != anim_players_.end())
		{
			if (*model3D->physics_handler3D->getRagdollStatus() == true)
			{
				blending blended;
				model3D->physics_handler3D->Update(frame_time, &model3D->blended_pose_, speed_);
			}
		}
	}
}

void AnimationHandler3D::Load(std::string filename, std::string animation_filename, gef::Animation* anim)
{
	model = filename + ".scn";
	animation = animation_filename + ".scn";

	blend = new blending;
	blend->speed_ = speed_;

	//if model scene doesnt exist create it and save the data
	if (model_scenes.find(filename) == model_scenes.end())
	{
		model3D->model_scene_ = new gef::Scene();

		model3D->model_scene_->ReadSceneFromFile(*m_platform, model.c_str());

		// we do want to render the data stored in the scene file so lets create the materials from the material data present in the scene file
		model3D->model_scene_->CreateMaterials(*m_platform);
		model3D->mesh_ = load3d.GetFirstMesh(model3D->model_scene_, m_platform, model3D->model_scene_);
		// get the first skeleton in the scene
		model3D->skeleton = load3d.GetFirstSkeleton(model3D->model_scene_);

		if (model3D->skeleton)
		{
			model3D->player_ = new gef::SkinnedMeshInstance(*model3D->skeleton);
			model3D->anim_player_.Init(model3D->player_->bind_pose());

			model3D->physics_handler3D = new PhysicsHandler3D();
			model3D->physics_handler3D->Init(filename, model3D->player_, m_renderer_3d, *m_platform);
		}
		
		model_scenes.insert(std::pair<std::string, Model>(filename, *model3D));
	}
	else
	{
		//if model scene exists already load in data
		model3D->model_scene_ = model_scenes.find(filename)->second.model_scene_;
		model3D->mesh_ = model_scenes.find(filename)->second.mesh_;
		model3D->skeleton = model_scenes.find(filename)->second.skeleton;
	}

	//if skeleton exists
	if (model3D->skeleton)
	{
		//load in model data
		model3D->player_ = model_scenes.find(filename)->second.player_;
		model3D->anim_player_ = model_scenes.find(filename)->second.anim_player_;

		if (anim_players_.find(&animation_filename[0]) == anim_players_.end())
		{
			anim_players_.insert(std::pair<std::string, blending>(&animation_filename[0], model3D->anim_player_));
			model3D->anim_player_.Init(model3D->player_->bind_pose());
		}

		model3D->blended_pose_ = model3D->player_->bind_pose();
		model3D->player_->set_mesh(model3D->mesh_);

		model3D->physics_handler3D = model_scenes.find(filename)->second.physics_handler3D;
	}

	Blending(anim, &animation_filename[0]);


}

void AnimationHandler3D::Render(gef::Renderer3D& renderer_3d, gef::Matrix44 projection_matrix, gef::Matrix44 view_matrix)
{

	renderer_3d.set_projection_matrix(projection_matrix);
	renderer_3d.set_view_matrix(view_matrix);

	// draw meshes here
	renderer_3d.Begin();

	// draw the player, the pose is defined by the bone matrices
	if (model3D->player_)
		renderer_3d.DrawSkinnedMesh(*model3D->player_, model3D->player_->bone_matrices());

	model3D->physics_handler3D->Render(&renderer_3d);

	renderer_3d.End();
}

void AnimationHandler3D::gui(std::string& image, std::string& animation)
{
	if (ImGui::InputText("Animation 1", &animation[0], 256, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		model3D->physics_handler3D->setRagdollStatus(false);
		if (animations_.find(&animation[0]) != animations_.end())
		{
			if (animations_.find(&animation2_input[0]) != animations_.end())
			{
				//if both animations are going to become equal swap their values
				if (animation == &Anim_2[0])
				{
					std::swap(Anim_1, Anim_2);
					animation2_input = Anim_2;
				}
				//load animation
				Anim_1 = animation;
				current_anim_ = animations_.find(&Anim_1[0])->second;
				Load(image, Anim_1, current_anim_);
			}
		}
		else
		{
			if (animations_.find(&animation2_input[0]) != animations_.end())
			{
				//create animation
				Load(image, &animation[0], current_anim_);
				Anim_1 = animation;
				current_anim_ = animations_.find(&Anim_1[0])->second;
				Load(image, &animation[0], current_anim_);
			}
		}
	}

	if (ImGui::InputFloat("Animation 1 Speed", &anim_players_.find(&Anim_1[0])->second.speed_))
	{

		if (anim_players_.find(&Anim_1[0])->second.speed_ == anim_players_.find(&Anim_2[0])->second.speed_)
		{
			anim_players_.find(&Anim_1[0])->second.speed_ = anim_players_.find(&Anim_2[0])->second.speed_ - 0.02f * multiplier;
		}

		//calculate minimum and maximum speed
		anim_players_.find(&Anim_1[0])->second.speed_ < anim_players_.find(&Anim_2[0])->second.speed_ ? max_speed = anim_players_.find(&Anim_2[0])->second.speed_ : max_speed = anim_players_.find(&Anim_1[0])->second.speed_;
		anim_players_.find(&Anim_1[0])->second.speed_ < anim_players_.find(&Anim_2[0])->second.speed_ ? min_speed = anim_players_.find(&Anim_1[0])->second.speed_ : min_speed = anim_players_.find(&Anim_2[0])->second.speed_;

		if (speed_ < min_speed)
		{
			speed_ = min_speed;
		}
		else if (speed_ > max_speed)
		{
			speed_ = max_speed;
		}

		getAnimBlend(anim_blend_);
	}

	
	if (ImGui::InputText("Animation 2", &animation2_input[0], 256, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		model3D->physics_handler3D->setRagdollStatus(false);
		if (anim_players_.find(&animation2_input[0]) != anim_players_.end())
		{
			if (animations_.find(&animation2_input[0]) != animations_.end())
			{
				//if both animations are going to become equal swap their values
				if (animation2_input == &Anim_1[0])
				{
					std::swap(Anim_2, Anim_1);
				}
				//get animation data
				Anim_2 = animation2_input;
				next_anim_ = animations_.find(&Anim_2[0])->second;
				Load(image, &animation2_input[0], next_anim_);
			}
		}
		else
		{
			//create animation
				Load(image, &animation2_input[0], next_anim_);
				Anim_2 = animation2_input;
				next_anim_ = animations_.find(&Anim_2[0])->second;
				Load(image, &animation2_input[0], next_anim_);
		}
	}

	if (ImGui::InputFloat("Animation 2 Speed", &anim_players_.find(&Anim_2[0])->second.speed_))
	{
		//don't allow 2 animation speeds to be identical
		if (anim_players_.find(&Anim_1[0])->second.speed_ == anim_players_.find(&Anim_2[0])->second.speed_)
		{
			anim_players_.find(&Anim_2[0])->second.speed_ = anim_players_.find(&Anim_1[0])->second.speed_ + 0.02f * multiplier;
		}

		//calculate maximum and minimum speeds
		anim_players_.find(&Anim_1[0])->second.speed_ < anim_players_.find(&Anim_2[0])->second.speed_ ? max_speed = anim_players_.find(&Anim_2[0])->second.speed_ : max_speed = anim_players_.find(&Anim_1[0])->second.speed_;
		anim_players_.find(&Anim_1[0])->second.speed_ < anim_players_.find(&Anim_2[0])->second.speed_ ? min_speed = anim_players_.find(&Anim_1[0])->second.speed_ : min_speed = anim_players_.find(&Anim_2[0])->second.speed_;

		//clamp speed
		if (speed_ < min_speed)
		{
			speed_ = min_speed;
		}
		else if (speed_ > max_speed)
		{
			speed_ = max_speed;
		}

		getAnimBlend(anim_blend_);
	}

	//clamp speed
	if (ImGui::SliderFloat("Speed", &speed_, min_speed, max_speed))
	{
		speed_ = (speed_ >= max_speed) ? max_speed : speed_;
		speed_ = (speed_ <= min_speed) ? min_speed : speed_;
	}

	ImGui::SliderFloat("Speed Multiplier", &multiplier, 1, 10);

	ImGui::SliderFloat("Minimum Transition Speed", &minimum_transition_speed, min_speed, max_speed);
	for (int i = 0; i < 2; ++i) ImGui::Spacing();

		ImGui::DragFloat("Character Rotation", &rot_);
		ImGui::SliderFloat("Character Size", &scale_, 0.01, 1);

		//speed cannot be inbetween mimum speed and minimum transition speed
		if (speed_ < minimum_transition_speed && speed_ != min_speed)
		{
			speed_ = minimum_transition_speed;
		}

		ImGui::Checkbox("Use Blend Tree", &blend_tree_toggle);

	//rotate the mesh
	if (rot_ < 0)
		rot_ = 360;

	if (rot_ > 360)
		rot_ = 0;

	for (int i = 0; i < 5; ++i) ImGui::Spacing();

	//display speed and anim blend values
	ImGui::Text("Speed (%.2f)", speed_);
	ImGui::Text("Blend (%.0f)%%", anim_blend_ * 100);

}


void AnimationHandler3D::Blending(gef::Animation* anim, std::string filename)
{

	if (anim_players_.find(&filename[0]) != anim_players_.end())
	{
		std::string fullfile_name = filename + ".scn";

		auto tmp = anim_players_.find(&filename[0])->second;
		if (animations_.find(&filename[0]) == animations_.end())
		{
			//load the new animation
			anim = load3d.LoadAnimation(fullfile_name.c_str(), "", m_platform);
			animations_.insert(std::pair<std::string, gef::Animation*>(&filename[0], anim));
		}
		else
		{
			anim = animations_.find(&filename[0])->second;
		}

		//init blend tree when animation is changed 
		InitBlendTree();
	}

	//set current and next animation
	if (animations_.find(&Anim_1[0]) != animations_.end())
		current_anim_ = animations_.find(&Anim_1[0])->second;
	if (animations_.find(&Anim_2[0]) != animations_.end())
		next_anim_ = animations_.find(&Anim_2[0])->second;

	if (anim_players_.find(&Anim_1[0]) != anim_players_.end())
	{
		auto tmp = &anim_players_.find(&Anim_1[0])->second;
		animations_;
		anim_players_;
		current_pose = &anim_players_.find(animations_.find(&Anim_1[0])->first)->second.pose();
		tmp->set_clip(current_anim_);
		tmp->set_looping(true);
		tmp->set_anim_time(0.0f);


		if (anim_players_.find(&Anim_2[0]) != anim_players_.end())
		{
			//calculate mimimum speed for current animation
			tmp->speed_max_ = animations_.find(&Anim_1[0])->second->duration() / animations_.find(&Anim_2[0])->second->duration();
		}

		tmp->speed_min_ = 1.f;
		tmp->speed_ = 1.f;

		//clamp speed value
		min_speed = tmp->speed_;
		speed_ = (speed_ >= max_speed) ? max_speed : speed_;
		speed_ = (speed_ <= min_speed) ? min_speed : speed_;
	}

	if (anim_players_.find(&Anim_2[0]) != anim_players_.end())
	{
		auto tmp = &anim_players_.find(&Anim_2[0])->second;
		next_pose = &anim_players_.find(animations_.find(&Anim_2[0])->first)->second.pose();
		tmp->set_clip(next_anim_);
		tmp->set_looping(true);
		tmp->set_anim_time(0.0f);

		if (anim_players_.find(&Anim_1[0]) != anim_players_.end())
		{
			//calculate mimimum speed for next animation
			tmp->speed_min_ = animations_.find(&Anim_2[0])->second->duration() / animations_.find(&Anim_1[0])->second->duration();
		}


		tmp->speed_max_ = 1.f;
		tmp->speed_ = 4.f;

		//clamp speed value
		max_speed = tmp->speed_;
		speed_ = (speed_ >= max_speed) ? max_speed : speed_;
		speed_ = (speed_ <= min_speed) ? min_speed : speed_;
	}
	
}

void AnimationHandler3D::Input(float dt, gef::Keyboard* keyboard)
{
	model3D->physics_handler3D->Input(keyboard, anim_blend_);

	//Increase blend value
	if (keyboard->IsKeyDown(keyboard->KC_L))
	{
		//anim_blend_ = (anim_blend_ <= 1.f) ? 1.f : 1.f + 0.02f * multiplier;
		speed_ = (speed_ >= max_speed) ? max_speed : speed_ + 0.02f * multiplier;
	}

	//Decrease Blend value
	if (keyboard->IsKeyDown(keyboard->KC_K))
	{
		//anim_blend_ = (anim_blend_ <= 0.f) ? 0.f : 0.f - 0.02f * multiplier;
		speed_ = (speed_ <= min_speed) ? min_speed : speed_ - 0.02f * multiplier;

		if (speed_ < minimum_transition_speed && speed_ != min_speed)
		{
			speed_ = min_speed;
		}
	}

	//rotate character clockwise
	if (keyboard->IsKeyDown(keyboard->KC_A))
	{
		rot_ -= dt * 85;

		if (rot_ < 0)
			rot_ = 360;
	}

	//rotate character anticlockwise
	if (keyboard->IsKeyDown(keyboard->KC_D))
	{
		rot_ += dt * 85;

		if (rot_ > 360)
			rot_ = 0;
	}

	//increase mesh size
	if (keyboard->IsKeyDown(keyboard->KC_W))
	{
		scale_ += dt * 1;

		if (scale_ > 1)
			scale_ = 1;
	}

	//decrease mesh size
	if (keyboard->IsKeyDown(keyboard->KC_S))
	{
		scale_ -= dt * 1;

		if (scale_ < 0.01)
			scale_ = 0.01;
	}

}

void AnimationHandler3D::getAnimBlend(float& anim_blend)
{
	//work out the blending value
	anim_blend_ = (((speed_ - min_speed) / (max_speed - min_speed)));

}

void AnimationHandler3D::InitBlendTree()
{
	anim_players_;
	int g = 0;


	//If player and skeleton exist
	if (model3D->player_ && model3D->player_->bind_pose().skeleton())
	{
		//Intilaise the blend tree with the bind pose
		blend_tree.Init(model3D->player_->bind_pose());

		//Create an input node of the current animation
		InputNode* current_clip_node = new InputNode(&blend_tree);
		current_clip_node->setClip(current_anim_);

		//Create an input node of the next animation
		InputNode* next_clip_node = new InputNode(&blend_tree);
		next_clip_node->setClip(next_anim_);
		
		//Create a bkending object
		Linear2BlendNode* l2b_node = new Linear2BlendNode(&blend_tree);
		
		//set variable walk_run_blend to the 0th element
		l2b_node->setVariable(0, std::string{ "walk_run_blend" });
	
		//set input 0 to the current animation
		l2b_node->setInput(0, current_clip_node, 0);

		//set input 1 to the next animation
		l2b_node->setInput(1, next_clip_node, 1);

		//set the input of the output node to the blended node
		blend_tree.m_output->setInput(0, l2b_node, 0);
		
		//start the blend tree
		blend_tree.Start();
	}
	
}



