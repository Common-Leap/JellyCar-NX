#include "../../Game/JellySplash.h"

#include "../../Utils/RectangleDrawer.h"
#include "../../Utils/UiTheme.h"

void JellySplash::Draw(GameManager* manager)
{
	if (_end)
		return;

	//start frame
	_renderManager->StartFrame();
	_renderManager->ClearScreen();

	int screenW = _renderManager->GetWidth();
	int screenH = _renderManager->GetHeight();
	RectangleDrawer* ui = RectangleDrawer::Instance();
	ui->DrawPaperBackground(_backSprite, screenW, screenH, _projection);

	int logoW = (int)(_sprite->GetTexture()->GetWidth());
	int logoH = (int)(_sprite->GetTexture()->GetHeight());
	int panelPad = 28;
	ui->DrawPanel(
		(screenW - logoW) / 2 - panelPad,
		(screenH - logoH) / 2 - panelPad,
		logoW + panelPad * 2,
		logoH + panelPad * 2,
		UiTheme::PanelRadius(),
		_projection);

	_sprite->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}
