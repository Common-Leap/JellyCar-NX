#include "JellyCore.h"

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Sprite.h>
#include <Andromeda/Utils/Logger.h>

#include "../Utils/RectangleDrawer.h"

JellyCore::JellyCore()
{
	_menuAtlas = 0;
	_menuFont = 0;
	_smallFont = 0;
	_titleFont = 0;
	_spriteShader = 0;
}

JellyCore::~JellyCore()
{
	delete _smallFont;
	delete _menuFont;
	delete _titleFont;

	delete _menuAtlas;
}

void JellyCore::Init()
{
	Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: begin\n");

	RectangleDrawer::Instance()->Init();

	//load basic shader
	_spriteShader = ShaderManager::Instance()->LoadFromFile("font", "Assets/Shaders/font", "Assets/Shaders/font", TextureColor);
	if (_spriteShader == 0)
	{
		Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: font shader load failed\n");
		return;
	}

	//create atlas (1024 avoids glyph packing failures on some runtime combos)
	_menuAtlas = new TextureAtlas(1024, 1024);
	Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: atlas created\n");

	//load fonts (fallback to Vera when JellyFont fails to initialize on some builds)
	auto buildFont = [this](float size, const char* label) -> TexturedFont*
	{
		TexturedFont* font = new TexturedFont(_menuAtlas, size, "Assets/Fonts/JellyFont.ttf");
		font->SetShader(_spriteShader);
		Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: cache %s font\n", label);
		int result = font->CacheGlyphs(_cache);
		Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: cache %s font result=%d\n", label, result);

		if (result < 0)
		{
			Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: fallback %s font -> Assets/Fonts/Vera.ttf\n", label);
			delete font;
			font = new TexturedFont(_menuAtlas, size, "Assets/Fonts/Vera.ttf");
			font->SetShader(_spriteShader);
			result = font->CacheGlyphs(_cache);
			Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: fallback cache %s font result=%d\n", label, result);
		}

		return font;
	};

	_smallFont = buildFont(16, "small");
	_menuFont = buildFont(32, "menu");
	_titleFont = buildFont(64, "title");

	//upload texture
	Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: create atlas texture\n");
	_menuAtlas->CreateTexture();
	Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: done\n");
}
