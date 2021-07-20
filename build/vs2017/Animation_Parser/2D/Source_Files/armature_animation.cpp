#include "Animation_Parser/2D/Headers/armature_animation.h"

#include "maths/math_utils.h"

ArmitureAnimation::ArmitureAnimation()
{

}

TranslationKey* ArmitureAnimation::ReadTranslationKeyDataFromJSON(const rapidjson::Value& array_pos)
{
	//get the translation data from the json file
	TranslationKey* trans_key = new TranslationKey();

	float time_to_next_key = array_pos["duration"].GetFloat();

	float translationX = array_pos.HasMember("x") ? array_pos["x"].GetFloat() : 0;
	float translationY = array_pos.HasMember("y") ? array_pos["y"].GetFloat() : 0;

	trans_key->time_to_next_key = time_to_next_key;
	trans_key->translation = gef::Vector2(translationX, translationY);

	return trans_key;
}

RotationKey* ArmitureAnimation::ReadRotationKeyDataFromJSON(const rapidjson::Value& array_pos)
{
	//get the rotation data from the json file
	RotationKey* rot_key = new RotationKey();
	float time_to_next_key = array_pos["duration"].GetFloat();

	float rotation = array_pos.HasMember("rotate") ? array_pos["rotate"].GetFloat() : 0;

	rot_key->time_to_next_key = time_to_next_key;
	rot_key->rotation = rotation;

	return rot_key;
}

gef::Vector2 ArmitureAnimation::lerp(gef::Vector2 start_pos, gef::Vector2 end_pos, float time)
{
	//lerp between current and next frame positions in the rig animation
	float x = (1.f - time) * start_pos.x + time * end_pos.x;
	float y = (1.f - time) * start_pos.y + time * end_pos.y;

	return gef::Vector2(x, y);
}

float ArmitureAnimation::lerp(float start_angle, float end_angle, float time)
{
	//lerp between current and next frame rotations in the rig animation
	float angle_diff = end_angle - start_angle;
	int x = 1;

	float angle;

	if (angle_diff > 180)
	{
		angle_diff -= 360;
	}
	else if (angle_diff < -180)
	{
		angle_diff += 360;
	}

	angle = start_angle + time * angle_diff;
	return angle;
}