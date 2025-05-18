#pragma once

#include "ViewProjection.h"
#include "Ground.h"
#include "Sprite.h"

#include "vector"

class SceneManager;

class TitleEvent {

public:
	enum State {
		TITLE,
		STAGESELECT,
		GAMESTART,
		STATEEND
	};

	enum StageSelect {
		TUTORIAL,
		STAGE1,
		STAGEEND
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
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

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

	const char* GetStageSelectString() const {
		switch (stageSelectState_) {
		case TUTORIAL: return "TUTORIAL";
		case STAGE1: return "STAGE1";
		case STAGEEND: return "STAGEEND";
		default: return "UNKNOWN";
		}
	}

	StageSelect GetStageSelect() const { return stageSelectState_; }

	void AddGround(Ground* ground) {
		grounds_.push_back(ground);
	}

	bool IsEventEnd() const {
		return isEventEnd_;
	}

	bool IsSceneChange() const {
		return isSceneChange_;
	}

private:

	ViewProjection* vp_ = nullptr; 

	SceneManager* sceneManager_ = nullptr;

	std::vector<Ground*> grounds_;

	std::unique_ptr<Sprite> padUI_;

	std::unique_ptr<Sprite> stageSelectUI_;

	std::unique_ptr<Sprite> startUI_;

	std::unique_ptr<Sprite> selectUI_;

	std::unique_ptr<Sprite> tutorialStageUI_;

	std::unique_ptr<Sprite> stage1UI_;

	State state_;

	StageSelect stageSelectState_;

	bool isEventEnd_ = false;

	bool isSceneChange_ = false;

	float timer_ = 0.0f;

	float alphaTimer_ = 0.0f;

	float alphaSpeed_ = 0.02f;

	float maxTime_ = 2.0f;

	Vector3 titleTranslation_;
	Vector3 titleRotation_;

	Vector3 stageSelectTranslation_;
	Vector3 stageSelectRotation_;

	Vector3 gameStartTranslation_;
	Vector3 gameStartRotation_;

	float stageDistance_;
};