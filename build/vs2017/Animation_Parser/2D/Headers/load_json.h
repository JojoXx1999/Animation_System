//-------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------Load Json-----------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <iostream>
#include <string.h>
#include "texture_atlas.h"
#include <graphics/sprite.h>
#include "Armature.h"
#include <map>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class LoadJSON {
public:
	//Default Constructor and Destructor
	inline LoadJSON::LoadJSON() {}
	inline LoadJSON::~LoadJSON() {}

	//Load the json file to be read
	TextureAtlas LoadJson(std::string filepath, std::string skeleton_filepath);
	bool testJson(std::string filepath);

	std::vector<ArmitureAnimation*> animations;
	ArmitureAnimation* current_animation;
	int frame_index;
	bool inline getStatus() { return status; }

private:
	//Read subtexture and armature data from json
	TextureAtlas* ReadTextureAtlasFromJSON(rapidjson::Document &tex_document, rapidjson::Document &ske_document);
	Armature* ReadArmatureFromJSON(const rapidjson::Value& armature_array, rapidjson::Document& ske_document, rapidjson::Document& tex_document, TextureAtlas& texture_atlas, int);

	//Was the file successfully loaded?
	void setStatus(bool b);

	//Objects for reading and storing data
	TextureAtlas m_texture_atlas;
	SubTexture m_sub_texture;
	Armature m_armature;
	ArmitureAnimation m_anim;
	bool status;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//