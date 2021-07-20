//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------Animation Texture-------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <string>
#include <graphics/sprite.h>

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//forward declaration

class TextureAtlas;

namespace gef {
	class Platform;
	class Sprite;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class AnimationTexture
{
public:
	inline AnimationTexture() {}
	inline ~AnimationTexture() {}

	//Set up sprite
	gef::Sprite InitTexture(const char* filepath, gef::Platform& platform, TextureAtlas texture_atlas, std::string name, bool init);

	//Update sprite
	void Update(TextureAtlas* texture_atlas, gef::Sprite* sprite, std::string name);

	//Destroy objects to prevent memeory leaks
	inline void CleanUp() { delete m_sprite_texture; m_sprite_texture = NULL; }

	//Return the position of the sprite
	inline gef::Vector2 getSpritePosition() { return m_sprite_pos; }

	//Set the size and position for each from, this centres each frame in the same position to avoid 'jumping'
	void setFrameSizeandPos(gef::Sprite* sprite, float screen_x, float screen_y, TextureAtlas* texture_atlas, std::string name);

private:
	//Sprite and texture objects
	gef::Sprite m_sprite;
	gef::Texture* m_sprite_texture;
	gef::Vector2 m_sprite_pos;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
