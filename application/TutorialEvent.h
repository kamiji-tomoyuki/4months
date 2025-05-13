#pragma once
#include "Sprite.h"
#include "Player.h"
#include "TutorialUI.h"

#include "vector"
#include "memory"

class TutorialEvent {

private:

	enum EventType {
		MOVE,
		ATTACKDIRECTION,
		TOPDEFENSE,
		BOTTOMDEFENSE,
		LEFTDEFENSE,
		RIGHTDEFENSE,
		DOWNSWING,
		THRUST,
		LEFTSLASH,
		RIGHTSLASH,
		END
	};

public:

	void Initialize(Player* player);

	void Update();

	void Draw();

private:

	Player* player_ = nullptr;

	std::vector<std::unique_ptr<TutorialUI>> tutorialUI_;

	uint32_t eventCount_ = 1;
};

