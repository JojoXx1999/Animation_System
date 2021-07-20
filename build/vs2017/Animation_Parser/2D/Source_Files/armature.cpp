#include "Animation_Parser/2D/Headers/Armature.h"
#include "maths/math_utils.h"
#include <iostream>
#include <string>

//typedefs for different maps
typedef std::pair<std::string, SkinSlot> str_slot;
typedef std::pair<std::string, Bone> str_bone;
typedef std::pair<std::string, AnimationKey> str_animation;
typedef std::pair<int, TranslationKey*> str_translationKey;
typedef std::pair<int, RotationKey*> str_rotationKey;
typedef std::pair<std::string, BoneKey*> str_boneKey;

Armature::Armature()
{
	time_passed = 0.f;
	frame_index = 0.f;
}

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void Armature::ReadSkinSlotFromJson(Armature* armature, rapidjson::Document& ske_document, int armature_num)
{

	//get all skin slots in the skeleton json file
	const rapidjson::Value& slots_array = ske_document["armature"][armature_num]["skin"][0]["slot"];

	//create a map to hold the name and data of the skin slots
	std::map<std::string, SkinSlot> slots;

	//repeat for ever skin slot in the json file
	for (int slot_num = 0; slot_num < (int)slots_array.Size(); ++slot_num)
	{

		//Get data from skin slots, check that the members exist otherwise give them a default value
		//Overloaded check member function location in sub_texture.cpp
		SkinSlot* slot = new SkinSlot;

		const rapidjson::Value& display_array = slots_array[slot_num]["display"];
		for (int display_num = 0; display_num < (int)display_array.Size(); ++display_num)
		{
			if ((int)display_array.Size() > 1) {
				m_sub_texture.CheckMember(slot->name_, display_array[display_num], "name", "");
				m_sub_texture.CheckMember(slot->part_name_, display_array[display_num], "name", "");

				//store name and data in the armature skin slots map
				armature->skin_slots_.insert(str_slot(std::to_string(display_num), *slot));
			}
			else {
				m_sub_texture.CheckMember(slot->name_, slots_array[slot_num], "name", "");
				m_sub_texture.CheckMember(slot->part_name_, display_array[display_num], "name", "");
				if (slots_array[slot_num]["display"][0].HasMember("transform")) {
					m_sub_texture.CheckMember(slot->x_, slots_array[slot_num]["display"][0]["transform"], "x", 0);
					m_sub_texture.CheckMember(slot->y_, slots_array[slot_num]["display"][0]["transform"], "y", 0);
					m_sub_texture.CheckMember(slot->rot_, slots_array[slot_num]["display"][0]["transform"], "skX", 0);
				}
				else
				{
					slot->x_ = slot->y_ = slot->rot_ = 0;
				}

				//build transform
				slot->BuildTransform();

				//store name and data in the armature skin slots map
				armature->skin_slots_.insert(str_slot(slot->name_, *slot));
			}


		}
		delete slot;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void Armature::ReadSlotFromJson(Armature* armature, rapidjson::Document& ske_document)
{
	//get number of slots in the skeleton json file
	const rapidjson::Value& slots_array = ske_document["armature"][0]["slot"];

	//for all slots
	for (int slot_num = 0; slot_num < (int)slots_array.Size(); ++slot_num)
	{
		//get the parent name of the current slot
		std::string parent_name;
		m_sub_texture.CheckMember(parent_name, slots_array[slot_num], "name", "");

		//add slots name to the slots list in armature class
		armature->slots_.push_back(parent_name);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void Armature::ReadBonesFromJSON(Armature* armature, rapidjson::Document& ske_document)
{
	//for all the armature bones
	const rapidjson::Value& bones_array = ske_document["armature"][0]["bone"];

	//create a map to store the name and bones data
	std::map<std::string, Bone> bones;

	//for the number of bones
	for (int bone_num = 0; bone_num < (int)bones_array.Size(); ++bone_num)
	{
		Bone* bone = new Bone();

		//get name and parent name of current bone
		bone->name_ = bones_array[bone_num]["name"].GetString();
		m_sub_texture.CheckMember(bone->parent_name_, bones_array[bone_num], "parent", "none");

		//check that bone contains transform
		if (bones_array[bone_num].HasMember("transform"))
		{
			//get position and rotation data of current bone
			bone->x_ = bones_array[bone_num]["transform"]["x"].GetFloat();
			bone->y_ = bones_array[bone_num]["transform"]["y"].GetFloat();
			bone->rot_ = bones_array[bone_num]["transform"]["skX"].GetFloat();
		}
		else
		{
			//default values
			bone->x_ = bone->y_ = bone->rot_ = 0;
		}

		//build local transform of the bone
		bone->BuildLocalTransform(NULL);

		//add name and data to bones map
		bones.insert(str_bone(bone->name_, *bone));
		delete bone;
	}
	//build world transform for all the bones
	m_bone.BuildBoneWorldTransform(&bones, NULL, frame_index);

	//copy data to armature class for easy access
	armature->bone_ = (bones);
	int x;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

void Armature::ReadAnimationFromJSON(Armature* armature, rapidjson::Document& ske_document)
{
	const rapidjson::Value& animation_array = ske_document["armature"][0]["animation"];

	//create a map to store the name and bones data
	std::map<std::string, AnimationKey*> animations;

	//for the number of bones
	for (int animation_num = 0; animation_num < (int)animation_array.Size(); ++animation_num) {
		AnimationKey* anim = new AnimationKey();

		anim->duration = animation_array[animation_num]["duration"].GetFloat();
		std::string name = animation_array[animation_num]["name"].GetString();

		bool hasBones = animation_array[animation_num].HasMember("bone");
		if (hasBones)
		{
			const rapidjson::Value& bone_array = animation_array[animation_num]["bone"];
			for (int bone_num = 0; bone_num < bone_array.Size(); ++bone_num)
			{
				BoneKey* bone_key = new BoneKey();
				bone_key->name = bone_array[bone_num]["name"].GetString();

				if (bone_array[bone_num].HasMember("translateFrame"))
				{
					for (int i = 0; i < bone_array[bone_num]["translateFrame"].Size(); i++)
					{
						bone_key->translation_keys.push_back(m_anim.ReadTranslationKeyDataFromJSON(bone_array[bone_num]["translateFrame"][i]));
					}
				}

				if (bone_array[bone_num].HasMember("rotateFrame"))
				{
					for (int i = 0; i < bone_array[bone_num]["rotateFrame"].Size(); i++)
					{
						bone_key->rotation_keys.push_back(m_anim.ReadRotationKeyDataFromJSON(bone_array[bone_num]["rotateFrame"][i]));
					}
				}

				anim->bone_keys.push_back(bone_key);
			}
		}

		armature->animation_.insert(str_animation(name, *anim));
		delete anim;
	}
}

void Armature::UpdateAnimation(std::map<std::string, AnimationKey>* anim, std::string* name, float dt, float sprite_speed)
{
	time_passed += dt;

	current_animation = anim->find(*name)->second;

	if (time_passed >= sprite_speed)
	{
		frame_index += 1;

		if (frame_index >= current_animation.duration)
		{
			frame_index = 0;
		}

		time_passed = 0.f;
	}

	m_bone.BuildBoneWorldTransform(&bone_, &current_animation, frame_index);
	//m_anim.CalculateFinalTransform();

}

void SkinSlot::BuildTransform()
{
	//Build transform matrix for skin slot
	m_rotation.SetIdentity();
	m_translation.SetIdentity();
	m_transform.SetIdentity();

	m_rotation.Rotate(gef::DegToRad(rot_));
	m_translation.SetTranslation(gef::Vector2(x_, y_));
	m_transform = m_rotation * m_translation;
}

void Bone::BuildLocalTransform(AnimationKey* anim)
{
	//Build local transform using rotation and position of bone
	gef::Matrix33 m_rotation;
	gef::Matrix33 m_translation;

	m_rotation.SetIdentity();
	m_translation.SetIdentity();
	m_local_transform.SetIdentity();

	m_rotation.Rotate(gef::DegToRad(rot_));
	m_translation.SetTranslation(gef::Vector2(x_, y_));

	m_local_transform = m_rotation * m_translation;
}

void Bone::BuildBoneWorldTransform(std::map<std::string, Bone>* bones, AnimationKey* anim, int current_frame)
{
	if (anim != NULL)
	{
		for (auto& bone_Keys : anim->bone_keys)
		{
			float start_time = 0;
			for (auto& translation_Keys : bone_Keys->translation_keys)
			{
				translation_Keys->start_time = start_time;
				start_time += translation_Keys->time_to_next_key;
			}

			start_time = 0;

			for (auto& rotation_Keys : bone_Keys->rotation_keys)
			{
				rotation_Keys->start_time = start_time;
				start_time += rotation_Keys->time_to_next_key;
			}
		}
	}



	for (auto it = bones->begin(); it != bones->end(); it++)
	{
		gef::Matrix33 m_rotation;
		gef::Matrix33 m_translation;

		m_rotation.SetIdentity();
		m_translation.SetIdentity();
		m_local_transform.SetIdentity();

		float localRot = it->second.rot_;
		float localX = it->second.x_;
		float localY = it->second.y_;

		if (anim != NULL)
		{
			for (auto& bone_Keys : anim->bone_keys)
			{
				if (bone_Keys->name == it->first) {
					for (int currKey = 0; currKey < bone_Keys->rotation_keys.size(); currKey++)
					{
						int nextKey = currKey >= bone_Keys->rotation_keys.size() - 1 ? 0 : currKey + 1;
						if (current_frame >= bone_Keys->rotation_keys[currKey]->start_time && current_frame < bone_Keys->rotation_keys[nextKey]->start_time)
						{
							float time = (current_frame - bone_Keys->rotation_keys[currKey]->start_time) / (bone_Keys->rotation_keys[nextKey]->start_time - bone_Keys->rotation_keys[currKey]->start_time);
							localRot += (m_animation.lerp(bone_Keys->rotation_keys[currKey]->rotation, bone_Keys->rotation_keys[nextKey]->rotation, time));
						}
						else if (currKey == nextKey)
						{
							localRot += bone_Keys->rotation_keys[currKey]->rotation;
						}
					}
					for (int currKey = 0; currKey < bone_Keys->translation_keys.size(); currKey++)
					{
						int nextKey = currKey >= bone_Keys->translation_keys.size() - 1 ? 0 : currKey + 1;
						if (current_frame >= bone_Keys->translation_keys[currKey]->start_time && current_frame < bone_Keys->translation_keys[nextKey]->start_time)
						{
							float time = (current_frame - bone_Keys->translation_keys[currKey]->start_time) / (bone_Keys->translation_keys[nextKey]->start_time - bone_Keys->translation_keys[currKey]->start_time);
							gef::Vector2 lerp_anim_pos = m_animation.lerp(bone_Keys->translation_keys[currKey]->translation, bone_Keys->translation_keys[nextKey]->translation, time);
							localX += lerp_anim_pos.x;
							localY += lerp_anim_pos.y;
						}
						else if (currKey == nextKey)
						{
							localX += bone_Keys->translation_keys[currKey]->translation.x;
							localY += bone_Keys->translation_keys[currKey]->translation.y;
						}
					}
					break;
				}
			}
		}
		m_rotation.Rotate(gef::DegToRad(localRot));
		m_translation.SetTranslation(gef::Vector2(localX, localY));
		it->second.m_local_transform = m_rotation * m_translation;
	}


	//Iterate through all bone parents to calculate correct world transform
	for (auto it = bones->begin(); it != bones->end(); it++)
	{
		gef::Matrix33 world_transform;
		world_transform.SetIdentity();

		//get bone data
		Bone bone = it->second;

		//while there is another parent
		while (true)
		{
			//attach current bone to parent
			world_transform = world_transform * bone.m_local_transform;

			if (bone.parent_name_ != "none")
				bone = bones->at(bone.parent_name_);
			else break;
		}

		//update world transform in current bone
		it->second.m_world_transform = world_transform;
	}
}