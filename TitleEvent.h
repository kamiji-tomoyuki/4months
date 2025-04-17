#pragma once

#include "ViewProjection.h"
#include "Ground.h"

#include "vector"

class TitleEvent {

public:
	enum State {
		TITLE,
		STAGESELECT,
		GAMESTART
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void ImGui();

public:

	void SetViewProjection(ViewProjection* vp) { vp_ = vp; }

	void SetState(State state) { state_ = state; }

	State GetState() const { return state_; }

	const char* GetStateString() const {
		switch (state_) {
		case TITLE: return "TITLE";
		case STAGESELECT: return "STAGESELECT";
		case GAMESTART: return "GAMESTART";
		default: return "UNKNOWN";
		}
	}

	void AddGround(Ground* ground) {
		grounds_.push_back(ground);
	}

	bool IsStandby() const {
		if (timer_ >= maxTime_) {
			return true;
		} else {
			return false;
		}
	}

private:

	ViewProjection* vp_ = nullptr; 

	std::vector<Ground*> grounds_;

	State state_;

	float timer_ = 0.0f;

	float maxTime_ = 2.0f;

	Vector3 titleTranslation_ = { 0.0f, 1.0f, 0.0f };
	Vector3 titleRotation_ = { -0.1f, 0.0f, 0.0f };

	Vector3 stageSelectTranslation_ = { 0.0f, 7000.0f, -7000.0f };
	Vector3 stageSelectRotation_ = { 0.7f, 0.0f, 0.0f };

	Vector3 gameStartTranslation_ = { 0.0f, 0.0f, 0.0f };
	Vector3 gameStartRotation_ = { 0.0f, 0.0f, 0.0f };
};