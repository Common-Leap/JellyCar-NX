#include "RectangleDrawer.h"
#include "UiTheme.h"

#include <Andromeda/Graphics/VertexTypes.h>

using namespace Andromeda::Graphics;

RectangleDrawer* RectangleDrawer::_instance = nullptr;

RectangleDrawer::RectangleDrawer()
	: _roundedShader(nullptr), _gradientShader(nullptr), _quadVAO(nullptr), _initialized(false)
{
}

RectangleDrawer* RectangleDrawer::Instance()
{
	if (_instance == nullptr)
		_instance = new RectangleDrawer();
	return _instance;
}

void RectangleDrawer::Init()
{
	if (_initialized)
		return;

	ShaderManager* shaderManager = ShaderManager::Instance();
	RenderManager* renderManager = RenderManager::Instance();

	_roundedShader = shaderManager->LoadFromFile(
		"rounded_ui", "Assets/Shaders/rounded_ui", "Assets/Shaders/rounded_ui", Textured);
	_gradientShader = shaderManager->LoadFromFile(
		"ui_gradient", "Assets/Shaders/ui_gradient", "Assets/Shaders/ui_gradient", Textured);

	_quadVAO = renderManager->CreateVertexArrayObject(Textured, StaticDraw);
	_quadVAO->CreateVertices(4);
	_quadVAO->CreateIndices(6);

	TextureVertex* verts = static_cast<TextureVertex*>(_quadVAO->GetVertices());
	verts[0].x = 0.0f; verts[0].y = 0.0f; verts[0].z = 0.0f; verts[0].u = 0.0f; verts[0].v = 0.0f;
	verts[1].x = 0.0f; verts[1].y = 1.0f; verts[1].z = 0.0f; verts[1].u = 0.0f; verts[1].v = 1.0f;
	verts[2].x = 1.0f; verts[2].y = 0.0f; verts[2].z = 0.0f; verts[2].u = 1.0f; verts[2].v = 0.0f;
	verts[3].x = 1.0f; verts[3].y = 1.0f; verts[3].z = 0.0f; verts[3].u = 1.0f; verts[3].v = 1.0f;

	unsigned short* indices = static_cast<unsigned short*>(_quadVAO->GetIndices());
	indices[0] = 1; indices[1] = 2; indices[2] = 0;
	indices[3] = 1; indices[4] = 3; indices[5] = 2;

	_quadVAO->Generate(false);
	_initialized = true;
}

void RectangleDrawer::Cleanup()
{
	_initialized = false;
}

void RectangleDrawer::DrawQuad(int x, int y, int width, int height, Shader* shader, glm::mat4& projection)
{
	glm::mat4 model(1.0f);
	glm::vec2 size((float)width, (float)height);
	glm::vec2 pos((float)x, (float)y);

	model = glm::translate(model, glm::vec3(pos, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	glm::mat4 translation = projection * model;
	shader->SetUniform(VertexShader, "translation", translation);
	_quadVAO->Draw();
}

void RectangleDrawer::DrawPaperBackground(Sprite* paper, int screenW, int screenH, glm::mat4& projection)
{
	if (paper == nullptr)
		return;

	int bw = paper->GetTexture()->GetWidth();
	int bh = paper->GetTexture()->GetHeight();
	if (bw <= 0 || bh <= 0)
		return;

	int cols = (int)ceil((float)screenW / (float)bw);
	int rows = (int)ceil((float)screenH / (float)bh);

	paper->SetScale(glm::vec2(1.0f, 1.0f));
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			paper->SetPosition(glm::vec2(bw * x + bw / 2, bh * y + bh / 2));
			paper->Draw(projection);
		}
	}
}

void RectangleDrawer::DrawRoundedRect(int x, int y, int width, int height, float cornerRadius,
	glm::vec4 fillColor, glm::vec4 borderColor, float borderWidth, glm::mat4& projection)
{
	DrawRoundedRect(x, y, width, height, cornerRadius, fillColor, fillColor, borderColor, borderWidth, projection);
}

void RectangleDrawer::DrawRoundedRect(int x, int y, int width, int height, float cornerRadius,
	glm::vec4 fillColor, glm::vec4 fillColor2, glm::vec4 borderColor, float borderWidth, glm::mat4& projection)
{
	if (!_initialized || width <= 0 || height <= 0)
		return;

	RenderManager::Instance()->SetDepth(false);

	float r = cornerRadius;
	if (r > (float)width * 0.5f)  r = (float)width * 0.5f;
	if (r > (float)height * 0.5f) r = (float)height * 0.5f;

	_roundedShader->Bind();
	_roundedShader->SetUniform(FragmentShader, "fillColor", fillColor);
	_roundedShader->SetUniform(FragmentShader, "fillColor2", fillColor2);
	_roundedShader->SetUniform(FragmentShader, "borderColor", borderColor);
	_roundedShader->SetUniform(FragmentShader, "rectWidth", (float)width);
	_roundedShader->SetUniform(FragmentShader, "rectHeight", (float)height);
	_roundedShader->SetUniform(FragmentShader, "radius", r);
	_roundedShader->SetUniform(FragmentShader, "borderWidth", borderWidth);
	_roundedShader->SetUniform(FragmentShader, "useGradient", fillColor == fillColor2 ? 0.0f : 1.0f);

	DrawQuad(x, y, width, height, _roundedShader, projection);
}

void RectangleDrawer::DrawPanel(int x, int y, int width, int height, float cornerRadius, glm::mat4& projection)
{
	DrawRoundedRect(x + 3, y + 5, width, height, cornerRadius,
		UiTheme::PanelShadow(), glm::vec4(0, 0, 0, 0), 0.0f, projection);

	DrawRoundedRect(x, y, width, height, cornerRadius,
		UiTheme::PanelFill(), UiTheme::PanelBorder(), 1.5f, projection);
}

void RectangleDrawer::DrawCell(int x, int y, int width, int height, bool selected, float pulse, glm::mat4& projection)
{
	if (selected)
	{
		glm::vec4 glow = UiTheme::CellBorderSelected();
		glow.a *= 0.25f + pulse * 0.20f;
		DrawRoundedRect(x - 2, y - 2, width + 4, height + 4, UiTheme::CellRadius() + 3.0f,
			glow, glm::vec4(0, 0, 0, 0), 0.0f, projection);
	}

	glm::vec4 fill = selected ? UiTheme::CellFillSelected() : UiTheme::CellFill();
	glm::vec4 border = selected ? UiTheme::CellBorderSelected() : UiTheme::CellBorder();
	float borderW = selected ? 2.0f : 1.0f;

	DrawRoundedRect(x, y, width, height, UiTheme::CellRadius(), fill, border, borderW, projection);
}

void RectangleDrawer::DrawProgressBar(int x, int y, int width, int height, float cornerRadius,
	float fillAmount, bool focused, glm::mat4& projection)
{
	if (fillAmount < 0.0f) fillAmount = 0.0f;
	if (fillAmount > 1.0f) fillAmount = 1.0f;

	glm::vec4 track = glm::vec4(1.0f, 1.0f, 1.0f, 0.55f);
	glm::vec4 trackBorder = UiTheme::PanelBorder();
	DrawRoundedRect(x, y, width, height, cornerRadius, track, trackBorder, 1.0f, projection);

	if (fillAmount > 0.001f)
	{
		int fillW = (int)(width * fillAmount);
		if (fillW < (int)cornerRadius * 2)
			fillW = (int)cornerRadius * 2;

		glm::vec4 fillA = focused ? glm::vec4(0.78f, 0.30f, 0.34f, 0.78f) : glm::vec4(0.42f, 0.52f, 0.78f, 0.72f);
		glm::vec4 fillB = focused ? glm::vec4(0.90f, 0.45f, 0.48f, 0.78f) : glm::vec4(0.34f, 0.58f, 0.82f, 0.72f);
		DrawRoundedRect(x, y, fillW, height, cornerRadius, fillA, fillB, glm::vec4(0, 0, 0, 0), 0.0f, projection);
	}
}

void RectangleDrawer::DrawDimOverlay(int screenW, int screenH, float alpha, glm::mat4& projection)
{
	glm::vec4 dim = UiTheme::DimOverlay();
	dim.a = alpha;
	DrawRoundedRect(0, 0, screenW, screenH, 0.0f, dim, glm::vec4(0, 0, 0, 0), 0.0f, projection);
}

void RectangleDrawer::DrawHighlightRow(int x, int y, int width, int height, float cornerRadius, glm::mat4& projection)
{
	DrawRoundedRect(x, y, width, height, cornerRadius, UiTheme::HighlightFill(), UiTheme::CellBorderSelected(), 1.5f, projection);
}

void RectangleDrawer::GetBottomBarRect(int screenW, int screenH, int& x, int& y, int& w, int& h)
{
	w = 500;
	h = 52;
	x = (screenW - w) / 2;
	y = screenH - h - 14;
}

void RectangleDrawer::DrawBottomBar(int screenW, int screenH, glm::mat4& projection)
{
	int barX, barY, barW, barH;
	GetBottomBarRect(screenW, screenH, barX, barY, barW, barH);
	DrawPanel(barX, barY, barW, barH, UiTheme::BarRadius(), projection);
}
