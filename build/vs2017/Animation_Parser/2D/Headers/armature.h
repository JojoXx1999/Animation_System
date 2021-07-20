//-------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------Armature------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <list>
#include "sub_texture.h"
#include <vector>

#include "armature_animation.h"

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Struct to hold all data from the skin slots read in from the json files
struct SkinSlot
{
	//Build skin slot transform
	void BuildTransform();

	//Public variables for storing skin slot data
	std::string name_;
	std::string part_name_;
	float x_;
	float y_;
	float rot_;

	//Matrices for building the slot transform
	gef::Matrix33 m_transform;
	gef::Matrix33 m_translation;
	gef::Matrix33 m_rotation;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Struct to hold all data from the bones read in from the json files
struct Bone
{
	//Public variables for creating local and world bone transforms
	void BuildLocalTransform(AnimationKey* anim);
	void BuildBoneWorldTransform(std::map<std::string, Bone>* bones, AnimationKey* anim, int current_frame);

	//Public variables to store bone data
	std::string name_;
	std::string parent_name_;
	float x_;
	float y_;
	float skX_;
	float rot_;

	//For updating the armature animation
	ArmitureAnimation m_animation;

	//Matrices to store local and world bone transforms
	gef::Matrix33 m_local_transform, m_world_transform, m_modified_local_transform;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class Armature
{
public:
	Armature();
	inline ~Armature() {}

	//Public functions for reading data from json
	void ReadSkinSlotFromJson(Armature* armatrue, rapidjson::Document& ske_document, int armature_num);
	void ReadSlotFromJson(Armature* armature, rapidjson::Document& ske_document);
	void ReadBonesFromJSON(Armature* armatrue, rapidjson::Document& ske_document);
	void ReadAnimationFromJSON(Armature* armatrue, rapidjson::Document& ske_document);
	void UpdateAnimation(std::map<std::string, AnimationKey>* anim, std::string* name, float dt, float sprite_speed);

	//Global skeleton json variables
	std::string name_;
	float frame_rate_;
	float duration_;

	//Maps to access skin slots and bones
	std::map <std::string, SkinSlot> skin_slots_;
	std::map <std::string, Bone> bone_;
	std::map <std::string, AnimationKey> animation_;

	//Current animation key
	AnimationKey current_animation;

	//Current animation frame
	int frame_index;

	//List for render order of rig
	std::list<std::string> slots_;

private:
	//Object variables to access functions within other classes
	SubTexture m_sub_texture;
	Bone m_bone;
	ArmitureAnimation m_anim;

	//Speed of animation
	float time_passed, sprite_speed;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
