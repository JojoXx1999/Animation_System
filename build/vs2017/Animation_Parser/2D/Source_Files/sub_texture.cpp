#include "Animation_Parser/2D/Headers/sub_texture.h"
#include "maths/vector2.h"

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void SubTexture::BuildTransform()
{
	//Build transform for sub_texture
	m_transform.SetIdentity();
	m_translation.SetIdentity();
	m_scale.SetIdentity();

	float x = width_ * 0.5f - (frame_width_ * 0.5f + frame_x_);
	float y = height_ * 0.5f - (frame_height_ * 0.5f + frame_y_);

	m_translation.SetTranslation(gef::Vector2(x, y));
	m_scale.Scale(gef::Vector2(width_, height_));

	m_transform = m_scale * m_translation;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
SubTexture* SubTexture::ReadSubTextureFromJSON(const rapidjson::Value& subtexture_array, int i)
{
	//Get subtexture details
	//Check members exist using overloaded check function
	//arguments: variable to be written too, location/object to search, member to look for, deafult if not found
	SubTexture* sub_texture = new SubTexture();
	CheckMember(sub_texture->name_, subtexture_array, "name", "");
	CheckMember(sub_texture->width_, subtexture_array, "width", 0);
	CheckMember(sub_texture->height_, subtexture_array, "height", 0);
	CheckMember(sub_texture->x_, subtexture_array, "x", 0);
	CheckMember(sub_texture->y_, subtexture_array, "y", 0);
	CheckMember(sub_texture->frame_x_, subtexture_array, "frameX", 0);
	CheckMember(sub_texture->frame_y_, subtexture_array, "frameY", 0);
	CheckMember(sub_texture->frame_width_, subtexture_array, "frameWidth", sub_texture->width_);
	CheckMember(sub_texture->frame_height_, subtexture_array, "frameHeight", sub_texture->height_);

	//build the texture for the subtexture
	sub_texture->BuildTransform();

	return sub_texture;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
float* SubTexture::CheckMember(float& var, const rapidjson::Value& member_array, const char* name, float default_value)
{
	//Check if member exists and get float data otherwise use default value
	if (member_array.HasMember(name))
		var = member_array[name].GetFloat();
	else var = default_value;
	return &var;

}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
std::string* SubTexture::CheckMember(std::string& var, const rapidjson::Value& member_array, const char* name, std::string default_value)
{
	//Check if member exists and get string data otherwise use default value
	if (member_array.HasMember(name))
		var = member_array[name].GetString();
	else var = default_value;
	return &var;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//