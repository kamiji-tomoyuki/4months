#pragma once
#include "Sprite.h"
#include "Player.h"
#include "TutorialUI.h"

#include "vector"
#include "memory"

class TutorialEvent {

public:

	void Initialize(Player* player);

	void Update();

	void Draw();

private:

	Player* player_ = nullptr;

	std::vector<std::unique_ptr<TutorialUI>> tutorialUI_;
};

