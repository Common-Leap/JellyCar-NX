#ifndef JellyCore_H
#define JellyCore_H

#include <Andromeda/Graphics/Text/TextureAtlas.h>
#include <Andromeda/Graphics/Text/TexturedFont.h>
#include <Andromeda/Graphics/Shader.h>

using namespace Andromeda::Graphics;

class JellyCore
{
public:

	const char * _cache = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	//const char * _titleCache = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	//atlases
	TextureAtlas* _menuAtlas;

	//fonts
	TexturedFont* _menuFont;

	//
	TexturedFont* _smallFont;

	//between small and menu
	TexturedFont* _bodyFont;

	//letters on buttons
	TexturedFont* _badgeFont;

	//title
	TexturedFont* _titleFont;

	//shader
	Shader* _spriteShader;

public:

	JellyCore();
	~JellyCore();

	void Init();

	//draws the text added to every font since the last call
	void DrawText(glm::mat4& projection);

};

#endif