#pragma once

#include "Sprite.h"

#include "memory"
#include "initializer_list"
#include "vector"

class TutorialUI {

public:

	enum class UIType {
		kRStickUp,
		kRStickDown,
		kRStickLeft,
		kRStickRight,
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

	void Initialize(Vector2 pos, std::string textSpriteFileName, std::initializer_list<UIType> uiTypes);

	void Update();

	void Draw();

	void SetIsSuccess(bool flag) { isSuccess = flag; }

private:

	std::unique_ptr<Sprite> textBackGround_;

	std::unique_ptr<Sprite> textSprite_;

	std::vector<UIType> uiTypes_;

	std::vector<std::unique_ptr<Sprite>> controllerUI_;

	bool isSuccess = false;
};