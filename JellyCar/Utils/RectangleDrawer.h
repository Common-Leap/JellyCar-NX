#ifndef RectangleDrawer_H
#define RectangleDrawer_H

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Shader.h>
#include <Andromeda/Graphics/VertexArrayObject.h>
#include <Andromeda/Graphics/Sprite.h>
#include <glm/glm.hpp>

using namespace Andromeda::Graphics;

class RectangleDrawer
{
private:
	static RectangleDrawer* _instance;

	Shader* _roundedShader;
	Shader* _gradientShader;
	VertexArrayObject* _quadVAO;
	bool _initialized;

	RectangleDrawer();

	void DrawQuad(int x, int y, int width, int height, Shader* shader, glm::mat4& projection);

public:
	static RectangleDrawer* Instance();

	void Init();
	void Cleanup();

	void DrawPaperBackground(Sprite* paper, int screenW, int screenH, glm::mat4& projection);

	void DrawRoundedRect(int x, int y, int width, int height, float cornerRadius,
		glm::vec4 fillColor, glm::vec4 borderColor, float borderWidth, glm::mat4& projection);

	void DrawRoundedRect(int x, int y, int width, int height, float cornerRadius,
		glm::vec4 fillColor, glm::vec4 fillColor2, glm::vec4 borderColor, float borderWidth, glm::mat4& projection);

	void DrawPanel(int x, int y, int width, int height, float cornerRadius, glm::mat4& projection);

	void DrawCell(int x, int y, int width, int height, bool selected, float pulse, glm::mat4& projection);

	void DrawProgressBar(int x, int y, int width, int height, float cornerRadius,
		float fillAmount, bool focused, glm::mat4& projection);

	void DrawDimOverlay(int screenW, int screenH, float alpha, glm::mat4& projection);

	void DrawHighlightRow(int x, int y, int width, int height, float cornerRadius, glm::mat4& projection);

	void DrawBottomBar(int screenW, int screenH, glm::mat4& projection);

	void GetBottomBarRect(int screenW, int screenH, int& x, int& y, int& w, int& h);
};

#endif
