#include "Animation_Parser/2D/Headers/load_json.h"
#include <fstream>
#include <iostream>

typedef std::pair<std::string, SubTexture> str_subtex;
typedef std::pair<int, Armature> str_armature;

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

bool LoadJSON::testJson(std::string filepath)
{
	rapidjson::Document doc_tex, doc_ske;
	std::ifstream ifs(filepath);
	auto v = ifs.good();
	return ifs.good();
}

TextureAtlas LoadJSON::LoadJson(std::string filepath, std::string skeleton_filepath)
{


	//Read in texture json document
	rapidjson::Document doc_tex, doc_ske;
	std::ifstream ifs(filepath);

	rapidjson::IStreamWrapper isw(ifs);
	doc_tex.ParseStream(isw);

	//Read in skeleton json document
	std::ifstream ifs_S(skeleton_filepath);
	rapidjson::IStreamWrapper isw_S(ifs_S);
	doc_ske.ParseStream(isw_S);

	//Read the texture atlas
	m_texture_atlas = *ReadTextureAtlasFromJSON(doc_tex, doc_ske);
	return m_texture_atlas;


}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

void LoadJSON::setStatus(bool b)
{
	status = b;
}

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
TextureAtlas* LoadJSON::ReadTextureAtlasFromJSON(rapidjson::Document &tex_document, rapidjson::Document &ske_document)
{
	TextureAtlas* texture_atlas = new TextureAtlas;

	//Get global texture atlas data from texture json
	texture_atlas->width_ = tex_document["width"].GetFloat();
	texture_atlas->height_ = tex_document["height"].GetFloat();
	texture_atlas->file_path_ = tex_document["imagePath"].GetString();
	texture_atlas->frame_name = tex_document["name"].GetString();

	//For the number of subtextures in the json
	const rapidjson::Value& subtexture_array = tex_document["SubTexture"];
	for (int subtex_num = 0; subtex_num < (int)subtexture_array.Size(); ++subtex_num)
	{
		//Read the subtexture data and store it in a map found in the texture_atlas class
		SubTexture* subtexture = m_sub_texture.ReadSubTextureFromJSON(subtexture_array[subtex_num], subtex_num);
		texture_atlas->sub_textures_.insert(str_subtex(subtexture->name_, *subtexture));
		delete subtexture;
	}

	//For the number of armatures in the skeleton json
	const rapidjson::Value& armature_array = ske_document["armature"];
	for (int armature_num = 0; armature_num < (int)armature_array.Size(); ++armature_num)
	{
		//Get armature data and store it in a mpa found in the texture_atlas class
		Armature* armature = ReadArmatureFromJSON(armature_array[armature_num], ske_document, tex_document, *texture_atlas, armature_num);
		texture_atlas->armature_details_.insert(str_armature(armature_num, *armature));
		delete armature;
	}

	return texture_atlas;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
Armature* LoadJSON::ReadArmatureFromJSON(const rapidjson::Value& armature_array, rapidjson::Document& ske_document, rapidjson::Document& tex_document, TextureAtlas& texture_atlas, int armature_num)
{
	Armature* armature = new Armature();

	//Get global texture atlas data from skeleton json
	m_sub_texture.CheckMember(armature->frame_rate_, armature_array, "frameRate", 0);
	m_sub_texture.CheckMember(armature->name_, armature_array, "name", "");
	m_sub_texture.CheckMember(armature->duration_, armature_array["animation"][0], "duration", 0);

	//Read skin slots data
	m_armature.ReadSkinSlotFromJson(armature, ske_document, armature_num);

	//Read bones data
	if (armature_array.HasMember("bone"))
		m_armature.ReadBonesFromJSON(armature, ske_document);

	//Read slots data for render order
	m_armature.ReadSlotFromJson(armature, ske_document);

	if (armature_array.HasMember("animation"))
		m_armature.ReadAnimationFromJSON(armature, ske_document);

	return armature;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

