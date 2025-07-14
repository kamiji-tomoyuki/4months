#pragma once

#include "Sprite.h"

#include "Input.h"

#include "memory"

class Pause {

public:

	enum ReturnScene {
		NONE,
		TITLE,
	};

public:

	void Initialize();

	void Update();

	void Draw();

public:

	bool GetIsPause() { return isPause_; }

	ReturnScene GetReturnScene() { return returnScene_; }

private:

	std::unique_ptr<Sprite> pauseBGSprite_;

	std::unique_ptr<Sprite> pauseUI_;

	std::unique_ptr<Sprite> returnGameUI_;

	std::unique_ptr<Sprite> returnTitleUI_;

	std::unique_ptr<Sprite> checkReturnTitle_;

	std::unique_ptr<Sprite> yesOrNoUI_;

	XINPUT_STATE preJoyState_;

	ReturnScene returnScene_;

	bool isPause_;

	bool isCheckReturnTitle_;

	float alphaTimer_;

	float alphaSpeed_;

};