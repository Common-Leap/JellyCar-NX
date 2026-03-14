#include "RectangleDrawer.h"
#include <Andromeda/Utils/Logger.h>
#include <Andromeda/Graphics/VertexTypes.h>
#include <Andromeda/Graphics/Sprite.h>
#include <Andromeda/Graphics/TextureManager.h>

#define LOG(...) Andromeda::Utils::Logger::Instance()->Info(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

using namespace Andromeda::Graphics;

RectangleDrawer* RectangleDrawer::_instance = nullptr;

RectangleDrawer::RectangleDrawer()
	: _colorShader(nullptr), _rectVAO(nullptr), _initialized(false)
{
}

RectangleDrawer* RectangleDrawer::Instance()
{
	if (_instance == nullptr)
	{
		_instance = new RectangleDrawer();
	}
	return _instance;
}

void RectangleDrawer::Init()
{
	if (_initialized)
		return;

	LOG("RectangleDrawer::Init begin\n");
	_initialized = true;
	LOG("RectangleDrawer::Init done\n");
}

void RectangleDrawer::Cleanup()
{
	_initialized = false;
}

void RectangleDrawer::DrawRect(int x, int y, int width, int height, glm::vec4 color, glm::mat4& projection)
{
	if (!_initialized)
	{
		LOG("RectangleDrawer::DrawRect not initialized\n");
		return;
	}

	// For now, just log that we tried to draw
	LOG("RectangleDrawer::DrawRect rect at (%d,%d) size (%d,%d) color (%.2f,%.2f,%.2f,%.2f)\n", 
		x, y, width, height, color.r, color.g, color.b, color.a);
}

void RectangleDrawer::DrawRectOutline(int x, int y, int width, int height, glm::vec4 color, int lineWidth, glm::mat4& projection)
{
	// Not implemented
}

void RectangleDrawer::DrawRoundedRect(int x, int y, int width, int height, int cornerRadius, glm::vec4 color, glm::mat4& projection)
{
	// Not implemented
}
