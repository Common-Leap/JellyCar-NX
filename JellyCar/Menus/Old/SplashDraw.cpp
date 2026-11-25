#include "../../Game/JellySplash.h"

void JellySplash::Draw(GameManager* manager)
{
	if (_end)
		return;

	//start frame
	_renderManager->StartFrame();

	//clear screen, the old splash is on black
	_renderManager->SetClearColor(0x00000000);
	_renderManager->ClearScreen();

	_sprite->Draw(_projection);

	//end frame
	_renderManager->EndFrame();
}
