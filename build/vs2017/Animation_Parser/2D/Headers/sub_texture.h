//-------------------------------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------Sub Texture----------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <string.h>
#include <map>
#include "maths/matrix33.h"
#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/istreamwrapper.h"
#include "gef.h"

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class SubTexture
{
public:
	inline SubTexture() {}
	inline ~SubTexture() {}

	//Build sub_texture transform
	void BuildTransform();

	//Read subtextures from json file
	SubTexture* ReadSubTextureFromJSON(const rapidjson::Value& subtexture_array, int i);

	//Overloaded check member function to find member and store data to prevent crashing if member doesn't exist
	float* CheckMember(float& var, const rapidjson::Value& member_array, const char*, float default_value);
	std::string* CheckMember(std::string& var, const rapidjson::Value& member_array, const char*, std::string default_value);

	//public vairables for storing subtexture data
	std::string name_;
	float width_;
	float height_;
	float frame_width_;
	float frame_height_;
	float x_;
	float y_;
	float frame_x_;
	float frame_y_;

	//matrices for building subtexture transform
	gef::Matrix33 m_transform;
	gef::Matrix33 m_translation;
	gef::Matrix33 m_scale;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
