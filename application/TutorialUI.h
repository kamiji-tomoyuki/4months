#pragma once

#include "Sprite.h"

#include "memory"
#include "initializer_list"
#include "vector"

class Player;

class TutorialUI {

public:

	enum class UIType {
		kRightStick,
		kLeftStick,
		kAButton,
		kButton,
		kXButton,
		kYButton,
		kRButton,
		kLButton,
		kRTrigger,
		kLTrigger
	};

public:

	void Initialize(Player* player, Vector2 pos, std::string textSpriteFileName, std::initializer_list<UIType> uiTypes);

	void Update();

	void Draw();

private:

	Player* player_ = nullptr;

	std::unique_ptr<Sprite> textBackGround_;

	std::unique_ptr<Sprite> textSprite_;

	std::vector<std::unique_ptr<Sprite>> controllerUI_;

	bool isSuccess = false;
};