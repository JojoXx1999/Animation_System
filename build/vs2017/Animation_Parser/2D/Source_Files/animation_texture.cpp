#include "Animation_Parser/2D/Headers/animation_texture.h"
#include "Animation_Parser/2D/Headers/texture_atlas.h"
#include "gef.h"
#include "load_texture.h"
#include <system/platform.h>
#include <graphics/texture.h>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
gef::Sprite AnimationTexture::InitTexture(const char* filepath, gef::Platform &platform, TextureAtlas texture_atlas, std::string name, bool init)
{
	//Create sprite and set its initial properties
	m_sprite_texture = CreateTextureFromPNG(filepath, platform);
	m_sprite.set_texture(m_sprite_texture);
	m_sprite_pos = gef::Vector2(platform.width()*0.5f, platform.height()*0.5f);
	setFrameSizeandPos(&m_sprite, m_sprite_pos.x, m_sprite_pos.y, &texture_atlas, name);
	m_sprite.set_texture(m_sprite_texture);
	m_sprite.set_position(gef::Vector4(platform.width()*0.5f, platform.height()*0.5f, -0.99f));
	m_sprite.set_height(texture_atlas.height_);
	m_sprite.set_width(texture_atlas.width_);


	return m_sprite;

}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationTexture::Update(TextureAtlas* texture_atlas, gef::Sprite* sprite, std::string name)
{
	///Probably change this function so the sprite can move across the screen if needed
	setFrameSizeandPos(sprite, m_sprite_pos.x, m_sprite_pos.y, texture_atlas, name);
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
void AnimationTexture::setFrameSizeandPos(gef::Sprite* sprite, float screen_x, float screen_y, TextureAtlas* texture_atlas, std::string name)
{
	//Get data of current subtexture being rendered, accessed via name
	float width = texture_atlas->sub_textures_.find(name)->second.width_;
	float height = texture_atlas->sub_textures_.find(name)->second.height_;
	float x = texture_atlas->sub_textures_.find(name)->second.x_;
	float y = texture_atlas->sub_textures_.find(name)->second.y_;
	float frame_width = texture_atlas->sub_textures_.find(name)->second.frame_width_;
	float frame_height = texture_atlas->sub_textures_.find(name)->second.frame_height_;
	float frame_x = texture_atlas->sub_textures_.find(name)->second.frame_x_;
	float frame_y = texture_atlas->sub_textures_.find(name)->second.frame_y_;

	//Update sprite properties
	sprite->set_width(width);
	sprite->set_height(height);
	sprite->set_uv_width(width / texture_atlas->width_);
	sprite->set_uv_height(height / texture_atlas->height_);

	//Uodate u,v and normalise
	float u = x / texture_atlas->width_;
	float v = y / texture_atlas->height_;
	sprite->set_uv_position(gef::Vector2(u, v));

	//Get centre of sprite
	float sprite_x = width * 0.5f - (frame_width * 0.5f + frame_x);
	float sprite_y = height * 0.5f - (frame_height * 0.5f + frame_y);

	//Centre on screen
	sprite->set_position(gef::Vector4(screen_x + sprite_x, screen_y + sprite_y, 0.0f));
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//