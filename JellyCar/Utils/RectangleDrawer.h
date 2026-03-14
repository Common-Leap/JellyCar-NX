#ifndef RectangleDrawer_H
#define RectangleDrawer_H

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/VertexArrayObject.h>
#include <glm/glm.hpp>

using namespace Andromeda::Graphics;

class RectangleDrawer
{
private:
	static RectangleDrawer* _instance;

	Shader* _colorShader;
	VertexArrayObject* _rectVAO;
	bool _initialized;

	RectangleDrawer();

public:
	static RectangleDrawer* Instance();

	void Init();
	void Cleanup();

	// Draw a filled rectangle
	void DrawRect(int x, int y, int width, int height, glm::vec4 color, glm::mat4& projection);

	// Draw a rectangle outline
	void DrawRectOutline(int x, int y, int width, int height, glm::vec4 color, int lineWidth, glm::mat4& projection);

	// Draw a rounded rectangle (approximated with corners)
	void DrawRoundedRect(int x, int y, int width, int height, int cornerRadius, glm::vec4 color, glm::mat4& projection);
};

#endif
