#pragma once
#include "Sprite.h"
#include "Player.h"
#include "Enemy.h"
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
		BLOCK,
		ATTACK,
		END
	};

public:

	void Initialize();

	void Update();

	void Draw();

public:

	bool GetIsSceneChange() { return isSceneChange_; }

	void SetPlayer(Player* ptr) { player_ = ptr; }

	void SetEnemy(Enemy* ptr) { enemy_ = ptr; }

private:

	void AddEvent(EventType event);

	void UpdateEvent();

private:

	Player* player_ = nullptr;

	Enemy* enemy_ = nullptr;

	std::vector<std::unique_ptr<TutorialUI>> tutorialUI_;

	std::vector<std::unique_ptr<Sprite>> tutorialText_;

	std::unique_ptr<Sprite> tutorialTextBG_;

	std::unique_ptr<Sprite> nextUI_;

	std::unique_ptr<Sprite> successUI_;

	std::vector<EventType> eventList_;

	uint32_t eventCount_ = 1;

	bool canNext_;

	int attackCount_;

	int blockCount_;

	float alphaTimer_;

	float alphaSpeed_;

	float successTimer_;

	Vector2 successUISize_;

	bool isSceneChange_;
};

