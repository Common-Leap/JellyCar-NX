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
	_bodyFont = 0;
	_badgeFont = 0;
	_titleFont = 0;
	_spriteShader = 0;
}

JellyCore::~JellyCore()
{
	delete _smallFont;
	delete _bodyFont;
	delete _badgeFont;
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
	auto buildFont = [this](float size, const char* label, const char* file) -> TexturedFont*
	{
		TexturedFont* font = new TexturedFont(_menuAtlas, size, file);
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

	const char* jellyFont = "Assets/Fonts/JellyFont.ttf";
	_smallFont = buildFont(16, "small", jellyFont);
	_bodyFont = buildFont(24, "body", jellyFont);
	_menuFont = buildFont(32, "menu", jellyFont);
	_titleFont = buildFont(64, "title", jellyFont);
	_badgeFont = buildFont(18, "badge", "Assets/Fonts/Vera.ttf");

	//upload texture
	Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: create atlas texture\n");
	_menuAtlas->CreateTexture();
	Andromeda::Utils::Logger::Instance()->Log("JellyCore::Init: done\n");
}

void JellyCore::DrawText(glm::mat4& projection)
{
	_menuFont->Draw(projection);
	_titleFont->Draw(projection);
	_bodyFont->Draw(projection);
	_smallFont->Draw(projection);
	_badgeFont->Draw(projection);
}
