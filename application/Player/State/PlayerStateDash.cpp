#include "PlayerStateDash.h"
#include "Player.h"
#include "Easing.h"
#include "PlayerStateRoot.h"
#include "PlayerAttackStateNoInput.h"

using namespace std::numbers;

// 初期化
void PlayerStateDash::Initialize()
{
	// プレイヤーのデータ取得
	Player::WorkDash workDash = player_->GetWorkDashData();
	
	workDash.DashTime_ = 0;

	// プレイヤーのデータセット
	player_->SetWorkDashData(workDash);

	// プレイヤーの攻撃ステートセット
	attackState_ = std::make_unique<PlayerAttackStateNoInput>(player_);
	player_->SetAttackType(Player::AttackType::kNullType);
}

// 更新
void PlayerStateDash::Update()
{
	// プレイヤーのデータ取得
	Player::WorkDash workDash = player_->GetWorkDashData();
	TimeManager* timeManager = player_->GetTimeManager();
	WorldTransform transform = player_->GetTransform();

	float startSpeed = player_->GetAcceleration();
	float endSpeed = player_->GetAcceleration() * workDash.kAttenuation_;
	Vector3 move{};
	float armStartTheta = 0;
	float armEndTheta = pi_v<float> / 2.0f;
	float armTheta{};
	
	workDash.DashTime_ += timeManager->deltaTime_;
	move.z = EaseOutExpo(startSpeed, endSpeed, workDash.DashTime_, workDash.kDashTime_);
	
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform.rotation_.y);
	move = Transformation(move, rotateMatrix);
	// 移動
	transform.translation_ = (transform.translation_ + move);
	transform.UpdateMatrix();
	
	// プレイヤーデータセット
	player_->SetWorkDashData(workDash);
	player_->SetTransform(transform);

	if (workDash.DashTime_ / workDash.kDashTime_ >= 1.0f) {
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
		return;
	}
}