#include "Player.h"
#include <cassert>
#include <algorithm>
#include <numbers>
#include "Input.h"
#include "Easing.h"
#include "GlobalVariables.h"
#include "CollisionTypeIdDef.h"
#include "myMath.h"
#include "FollowCamera.h"
#include "LockOn.h"
#include <Audio.h>

using namespace std::numbers;

Player::Player() {
	id_ = playerID_;
	playerID_++;
}

void Player::Init() {
	//基底クラスの初期化
	BaseObject::Init();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));
	InitializeFloatingGimmick();
	//プレイヤーの手
	for (std::unique_ptr<PlayerArm>& playerArm : arms_) {
		playerArm = std::make_unique<PlayerArm>();
		playerArm->SetPlayer(this);
	}

	BaseObject::CreateModel("player/playerBody.obj");
	arms_[kSword]->Initialize("player/playerArm.gltf", "player/playerPalm.obj");
	//arms_[kSword]->Initialize("player/playerArm.gltf", "player/playerPalm.obj");

	arms_[kSword]->SetID(id_);
	arms_[kSword]->SetTranslation(Vector3(1.7f, 0.0f, 1.3f));
	arms_[kSword]->SetScale(Vector3(0.8f, 0.8f, 0.8f));

	//arms_[kRArm]->SetID(id_);
	//arms_[kRArm]->SetTranslation(Vector3(-1.7f, 0.0f, 1.3f));
	//arms_[kRArm]->SetScale(Vector3(0.8f, 0.8f, 0.8f));
	//imgui
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	Input::GetInstance()->SetJoystickDeadZone(0, 4000, 4000);

	/*for (int i = 0; i < 2; ++i) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		emitters_.push_back(std::move(emitter_));
	}
	emitters_[0]->Initialize("Attack" + std::to_string(id_), "GameScene/planeSpark.obj");
	emitters_[1]->Initialize("Smoke" + std::to_string(id_), "GameScene/planeSmoke.obj");*/

	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "kAcceleration", kAcceleration_);
	globalVariables->AddItem(groupName, "kAttenuation", kAttenuation_);
	globalVariables->AddItem(groupName, "kLimitRunSpeed", kLimitRunSpeed_);
	//globalVariables->AddItem(groupName, "attackVelocity_", attackVelocity_);
	globalVariables->AddItem(groupName, "size", size_);
	globalVariables->AddItem(groupName, "kHp_", kHp_);
	//globalVariables->AddItem(groupName, "attackPower_", attackPower_);
	//globalVariables->AddItem(groupName, "powerMagnification_", powerMagnification_);
	ApplyGlobalVariables();

	hp_ = kHp_;
}

void Player::Update() {
	ApplyGlobalVariables();

	//基底クラス更新
	BaseObject::Update();

	if (behaviorRequest_) {
		//振るまいを変更する
		behavior_ = behaviorRequest_.value();

	(this->*BehaviorInitFuncTable[static_cast<size_t>(behavior_)])();

		behaviorRequest_ = std::nullopt;
	}

	(this->*BehaviorUpdateFuncTable[static_cast<size_t>(behavior_)])();

	//if (transform_.translation_.x < -75.0f || transform_.translation_.x > 75.0f) {
	//	transform_.translation_.x = std::clamp(transform_.translation_.x, -75.0f, 75.0f);
	//	velocity_.x *= -1.0f;
	//	if (!isGameOver_) {  // GameOver時はチェックしない
	//		Audio::GetInstance()->PlayWave(11, 1.0f, false);
	//	}
	//}
	//if (transform_.translation_.z < -135.0f || transform_.translation_.z > 12.0f) {
	//	transform_.translation_.z = std::clamp(transform_.translation_.z, -135.0f, 12.0f);
	//	velocity_.z *= -1.0f;
	//	if (!isGameOver_) {  // GameOver時はチェックしない
	//		Audio::GetInstance()->PlayWave(11, 1.0f, false);
	//	}
	//}

	// 速度に減衰をかける
	velocity_.x *= (1.0f - kAttenuation_);
	velocity_.z *= (1.0f - kAttenuation_);
	//Yなし
	velocity_.y = 0.0f;

	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed_, kLimitRunSpeed_);
	velocity_.z = std::clamp(velocity_.z, -kLimitRunSpeed_, kLimitRunSpeed_);

	transform_.scale_ = { size_,size_ ,size_ };
	Collider::SetRadius(size_ * 1.2f);
	Collider::SetAABBScale({ 0.0f,0.0f,0.0f });

	transform_.translation_ += velocity_ * timeManager_->deltaTime_;
	transform_.UpdateMatrix();

	for (const std::unique_ptr<PlayerArm>& arm : arms_) {
		arm->SetSize(size_ * 1.3f);
		arm->Update();
	}
}

void Player::UpdateParticle(const ViewProjection& viewProjection) {
	/*if (timeManager_->GetTimer("Smoke" + std::to_string(id_)).isStart &&
		!timeManager_->GetTimer("SmokeCoolTime" + std::to_string(id_)).isStart) {
		emitters_[1]->SetEmitActive(true);
		timeManager_->SetTimer("SmokeCoolTime" + std::to_string(id_), 0.1f);
	}
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->SetEmitPosition(GetCenterPosition());
		emitter_->UpdateOnce(viewProjection);
	}*/
}

void Player::Draw(const ViewProjection& viewProjection) {
	//基底クラス描画
	BaseObject::Draw(viewProjection);
	for (const std::unique_ptr<PlayerArm>& arm : arms_) {
		arm->Draw(viewProjection);
	}

}

void Player::DrawParticle(const ViewProjection& viewProjection) {
	//for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
	//	emitter_->Draw();
	//	//emitter_->DrawEmitter();
	//}
}

void Player::DrawAnimation(const ViewProjection& viewProjection)
{
	for (const std::unique_ptr<PlayerArm>& arm : arms_) {
		arm->DrawAnimation(viewProjection);
	}
}

void Player::OnCollision([[maybe_unused]] Collider* other) {

	//if (isGameOver_) return; // GameOver中は衝突処理をスキップ

	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon)) {
		PlayerArm* enemyArm = static_cast<PlayerArm*>(other);
		
	}
}

void Player::OnCollisionEnter([[maybe_unused]] Collider* other) {
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
}

void Player::OnCollisionOut([[maybe_unused]] Collider* other) {

}

// 浮遊動作(無入力)の初期化
void Player::InitializeFloatingGimmick() {
	root_.floatingParameter = 0.0f;
}

// 浮遊動作(無入力)の更新
void Player::UpdateFloatingGimmick() {
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * std::numbers::pi_v<float> / root_.period;
	//パラメータを1ステップ分加算
	root_.floatingParameter += step;
	//2πを超えたら0に戻す
	root_.floatingParameter = std::fmod(root_.floatingParameter, 2.0f * std::numbers::pi_v<float>);

	//浮遊を座標に反映
	transform_.translation_.y = std::sin(root_.floatingParameter) * root_.floatingAmplitude;
	//
	/*arms_[kLArm]->SetRotationX(std::sin(root_.floatingParameter) * root_.armAmplitude);
	arms_[kRArm]->SetRotationX(std::sin(root_.floatingParameter) * root_.armAmplitude);*/
}

// 通常動作の初期化
void Player::BehaviorRootInitialize() {
	attack_.isAttack = false;
	arms_[kSword]->SetIsAttack(false);
	//arms_[kRArm]->SetIsAttack(false);
	grab_.isGrab = false;
	arms_[kSword]->SetIsGrab(false);
	//arms_[kRArm]->SetIsGrab(false);
}

// 通常動作の更新
void Player::BehaviorRootUpdate() {
	UpdateFloatingGimmick();

	// 移動処理
	Move();
	if (aimingDirection_.x == 0 && aimingDirection_.z == 0) {
		arms_[kSword]->SetTranslation({ 1.5f, 0.0f, 0.0f });
	}

	// ゲームパッド入力処理
	XINPUT_STATE joyState;
	// 予備動作(攻撃防御方向入力)
	if (Input::GetInstance()->GetJoystickState(0, joyState) && (joyState.Gamepad.sThumbRX != 0 || joyState.Gamepad.sThumbRY != 0)) {
		// 方向取得
		DirectionPreliminaryAction();

		// 攻撃の構え処理
		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			behaviorRequest_ = Behavior::kPreliminary;
		}
		// 攻撃の処理
		// Rボタンを離した時
		//else if (Input::GetInstance()->GetJoystickStatePrevious(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		//	behaviorRequest_ = Behavior::kAttack;

		//	// フェイク可能時間をセット

		//}

		// 防御処理
		// 　フェイク可能時間内の条件追加
		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER || Input::GetInstance()->TriggerKey(DIK_J)) {
			behaviorRequest_ = Behavior::kProtection;
			attack_.isLeft = true;
		}
	}
	// 攻撃処理
	/*if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || Input::GetInstance()->TriggerKey(DIK_K)) {
		behaviorRequest_ = Behavior::kAttack;
		attack_.isLeft = false;
	}*/
	// ダッシュ処理
	if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X ||
		Input::GetInstance()->TriggerKey(DIK_L)) {
		behaviorRequest_ = Behavior::kDash;
	}
}

// ダッシュ動作の初期化
void Player::BehaviorDashInitialize(){
	workDash_.DashTime_ = 0;
}

// ダッシュ動作の更新
void Player::BehaviorDashUpdate(){
	float startSpeed = kAcceleration_;
	float endSpeed = kAcceleration_ * workDash_.kAttenuation_;
	Vector3 move{};
	float armStartTheta = 0;
	float armEndTheta = std::numbers::pi_v<float> / 2.0f;
	float armTheta{};
	workDash_.DashTime_ += timeManager_->deltaTime_;
	if (workDash_.DashTime_ / workDash_.kDashTime_ >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}
	/*armTheta = EaseOutExpo(armStartTheta, armEndTheta,workDash_.DashTime_ , workDash_.kDashTime_);*/
	move.z = EaseOutExpo(startSpeed, endSpeed,workDash_.DashTime_ , workDash_.kDashTime_);
	Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotation_.y);
	move = Transformation(move, rotateMatrix);
	// 移動
	transform_.translation_ = (transform_.translation_ + move);
	transform_.UpdateMatrix();

	/*arms_[kLArm]->SetRotationX(armTheta);
	arms_[kRArm]->SetRotationX(armTheta);*/
}

// 攻撃の構えの動作の初期化
void Player::BehaviorPostureAttackInitialize()
{
	transform_.UpdateMatrix();
	attack_.time = 0;
	attack_.isAttack = true;
	attack_.isLeft = !attack_.isLeft;
	attack_.armStart = arms_[kSword]->GetTranslation().z;
	arms_[kSword]->SetIsAttack(true);
}

// 攻撃の構えの動作の更新
void Player::BehaviorPostureAttackUpdate()
{
	// 移動処理
	Move();
	// 方向取得
	DirectionPreliminaryAction();
	if (aimingDirection_.x == 0 && aimingDirection_.z == 0) {
		behaviorRequest_ = Behavior::kRoot;
	}

	// 武器を入力方向に移動
	float cosTheta = atan2f(aimingDirection_.z, aimingDirection_.x);
	// 上
	if (cosTheta > 0.25f * pi && cosTheta < 0.75f * pi) {
		arms_[kSword]->SetTranslationX(aimingDirection_.x);
		arms_[kSword]->SetTranslationY(aimingDirection_.z);
		arms_[kSword]->SetTranslationZ(0.0f);
		attackTypeRequest_ = AttackType::kDownSwing;
	}
	// 下
	else if (cosTheta < -0.25f * pi && cosTheta > -0.75f * pi) {
		arms_[kSword]->SetTranslationX(1.2f);
		arms_[kSword]->SetTranslationY(0.0f);
		arms_[kSword]->SetTranslationZ(-0.75f);
		attackTypeRequest_ = AttackType::kThrust;
	}
	// 左
	else if (cosTheta >= 0.75f * pi || cosTheta <= -0.75f * pi) {
		arms_[kSword]->SetTranslationX(aimingDirection_.x);
		arms_[kSword]->SetTranslationY(0.0f);
		arms_[kSword]->SetTranslationZ(aimingDirection_.z);
		attackTypeRequest_ = AttackType::kRightSlash;
	}
	// 右
	else {
		arms_[kSword]->SetTranslationX(aimingDirection_.x);
		arms_[kSword]->SetTranslationY(0.0f);
		arms_[kSword]->SetTranslationZ(aimingDirection_.z);
		attackTypeRequest_ = AttackType::kLeftSlash;
	}

	// ゲームパッド入力処理
	XINPUT_STATE joyState;
	// 攻撃の処理
	if (Input::GetInstance()->GetJoystickState(0, joyState) && !(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kAttack;
		
		// フェイク可能時間をセット

	}
}

// 攻撃動作の初期化
void Player::BehaviorAttackInitialize() {
	transform_.UpdateMatrix();
	attack_.time = 0;
	attack_.isAttack = true;
	attack_.isLeft = !attack_.isLeft;
	if (attack_.isLeft) {
		attack_.armStart = arms_[kSword]->GetTranslation().z;
		arms_[kSword]->SetIsAttack(true);
	} else {
		//attack_.armStart = arms_[kRArm]->GetTranslation().z;
		//arms_[kRArm]->SetIsAttack(true);
	}
}

// 攻撃動作の更新
void Player::BehaviorAttackUpdate() {
	// 移動処理
	Move();

	// アームの開始角度
	float speed = kAcceleration_ * 2.0f;
	Vector3 move{};
	attack_.time += timeManager_->deltaTime_;

	if (attack_.time / attack_.kLimitTime > 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}

	if (attackTypeRequest_) {
		//振るまいを変更する
		attackType_ = attackTypeRequest_.value();

		(this->*AttackTypeInitFuncTable[static_cast<size_t>(attackType_)])();

		attackTypeRequest_ = std::nullopt;
	}

	(this->*AttackTypeUpdateFuncTable[static_cast<size_t>(attackType_)])();

	// 攻撃方向
	/*XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);*/
	
	// 方向が未入力の時にセット
	/*if (aimingDirection_.x == 0 && aimingDirection_.z == 0)
	{
		aimingDirection_ = { InputDirection().x, 0.0f, InputDirection().y };
		aimingDirection_ *= 5.0f;
	}
	Vector3 armNow = { 0.0f };*/

	// 攻撃モーション
	
	/*if (attack_.time / attack_.kLimitTime < 0.5f) {
		attack_.armEnd = aimingDirection_.x;
		armNow.x = EaseInSine(1.7f, 1.7f + attack_.armEnd, attack_.time * 2.0f, attack_.kLimitTime);
		attack_.armEnd = aimingDirection_.z;
		armNow.z = EaseInSine(attack_.armStart, attack_.armStart + attack_.armEnd, attack_.time * 2.0f, attack_.kLimitTime);
	} else {
		attack_.armEnd = aimingDirection_.x;
		armNow.x = EaseOutSine(1.7f + attack_.armEnd, 1.7f, attack_.time * 2.0f - attack_.kLimitTime, attack_.kLimitTime);
		attack_.armEnd = aimingDirection_.z;
		armNow.z = EaseOutSine(attack_.armStart + attack_.armEnd, attack_.armStart, attack_.time * 2.0f - attack_.kLimitTime, attack_.kLimitTime);
	}*/

	// 攻撃モーションが終わり次第攻撃方向をリセット
	//if (attack_.time / attack_.kLimitTime >= 1.0f) {
	//	aimingDirection_ = { 0.0f, 0.0f, 0.0f };
	//}

	//if (attack_.isLeft) {
	//	arms_[kSword]->SetTranslationX(armNow.x);
	//	arms_[kSword]->SetTranslationZ(armNow.z);
	//} else {
	//	//arms_[kRArm]->SetTranslationX(armNow.x);
	//	//arms_[kRArm]->SetTranslationZ(armNow.z);
	//}

	//ロックオン中
	//if (lockOn_ && lockOn_->ExistTarget()) {
	//	// ロックオン座標
	//	Vector3 lockOnPos = lockOn_->GetTargetPosition();
	//	// 追従対象からロックオン対象へのベクトル
	//	Vector3 sub = lockOnPos - GetWorldPosition();

	//	//距離
	//	float distance = sub.Length();
	//	//距離しきい値
	//	const float threshold = 0.4f;
	//	
	//	//しきい値より離れている時のみ
	//	if (distance > threshold) {
	//		//Y軸周り角度
	//		transform_.rotation_.y = std::atan2f(sub.x, sub.z);
	//	}
	//}
}

// 防御動作の初期化
void Player::BehaviorProtectionInitialize() {
	transform_.UpdateMatrix();
	attack_.time = 0;
	attack_.isAttack = true;
	attack_.isLeft = !attack_.isLeft;
	if (attack_.isLeft) {
		attack_.armStart = arms_[kSword]->GetTranslation().z;
		arms_[kSword]->SetIsAttack(true);
	}
	else {
		//attack_.armStart = arms_[kRArm]->GetTranslation().z;
		//arms_[kRArm]->SetIsAttack(true);
	}
	/*transform_.UpdateMatrix();
	grab_.time = 0;
	grab_.isGrab = false;
	grab_.armStartL = arms_[kLArm]->GetTranslation().z;
	grab_.armStartR = arms_[kRArm]->GetTranslation().z;
	arms_[kLArm]->SetIsGrab(true);
	arms_[kRArm]->SetIsGrab(true);*/
}

// 防御動作の更新
void Player::BehaviorProtectionUpdate() {
	Move();
	//アームの開始角度
	float speed = kAcceleration_ * 2.0f;
	Vector3 move{};
	attack_.time += timeManager_->deltaTime_;

	if (attack_.time / attack_.kLimitTime > 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}

	// 攻撃方向
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);

	// 方向が未入力の時にセット
	//if (aimingDirection_.x == 0 && aimingDirection_.z == 0)
	//{
	//	aimingDirection_ = { (float)joyState.Gamepad.sThumbRX / SHRT_MAX, 0.0, (float)joyState.Gamepad.sThumbRY / SHRT_MAX };
	//	aimingDirection_ = aimingDirection_.Normalize();
	//	aimingDirection_ *= 5.0f;
	//}
	//Vector3 armNow = { 0.0f };

	//// 攻撃モーション
	//if (attack_.time / attack_.kLimitTime < 0.5f) {
	//	attack_.armEnd = aimingDirection_.x;
	//	armNow.x = EaseInSine(-1.7f, -1.7f + attack_.armEnd, attack_.time * 2.0f, attack_.kLimitTime);
	//	attack_.armEnd = aimingDirection_.z;
	//	armNow.z = EaseInSine(attack_.armStart, attack_.armStart + attack_.armEnd, attack_.time * 2.0f, attack_.kLimitTime);
	//}
	//else {
	//	attack_.armEnd = aimingDirection_.x;
	//	armNow.x = EaseOutSine(-1.7f + attack_.armEnd, -1.7f, attack_.time * 2.0f - attack_.kLimitTime, attack_.kLimitTime);
	//	attack_.armEnd = aimingDirection_.z;
	//	armNow.z = EaseOutSine(attack_.armStart + attack_.armEnd, attack_.armStart, attack_.time * 2.0f - attack_.kLimitTime, attack_.kLimitTime);
	//}

	//// 攻撃モーションが終わり次第攻撃方向をリセット
	//if (attack_.time / attack_.kLimitTime >= 1.0f) {
	//	aimingDirection_ = { 0.0f, 0.0f, 0.0f };
	//}

	//if (attack_.isLeft) {
	//	arms_[kSword]->SetTranslationX(armNow.x);
	//	arms_[kSword]->SetTranslationZ(armNow.z);
	//}
	//else {
	//	//arms_[kRArm]->SetTranslationX(armNow.x);
	//	//arms_[kRArm]->SetTranslationZ(armNow.z);
	//}
	
	//アームの開始角度
	/*float armNowL = 0.0f;
	float armNowR = 0.0f;

	grab_.time += timeManager_->deltaTime_;

	if (grab_.time / grab_.kLimitTime > 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}

	if (grab_.time / grab_.kLimitTime < 0.5f) {
		armNowL = EaseInSine(grab_.armStartL, grab_.armStartL + grab_.armEnd, grab_.time * 2.0f, grab_.kLimitTime);
		armNowR = EaseInSine(grab_.armStartR, grab_.armStartR + grab_.armEnd, grab_.time * 2.0f, grab_.kLimitTime);
	} else {
		armNowL = EaseOutSine(grab_.armStartL + grab_.armEnd, grab_.armStartL, grab_.time * 2.0f - grab_.kLimitTime, grab_.kLimitTime);
		armNowR = EaseOutSine(grab_.armStartR + grab_.armEnd, grab_.armStartR, grab_.time * 2.0f - grab_.kLimitTime, grab_.kLimitTime);
	}
	
	arms_[kLArm]->SetTranslationZ(armNowL);
	arms_[kRArm]->SetTranslationZ(armNowR);*/
}

// 勝利(喜ぶ)動作の初期化
void Player::BehaviorCelebrateInitialize()
{
	arms_[kSword]->SetRotationX(0.0f);
	//arms_[kRArm]->SetRotationX(0.0f);

	//celebrateTime_ = 0.0f;
}

// 勝利(喜ぶ)動作更新
void Player::BehaviorCelebrateUpdate()
{
	// 腕を振る動作 (sin波を使って上下)
	float wave = std::sin(timeManager_->GetCurrentTime() * 10.0f) * 0.5f; // -0.5 〜 0.5 の範囲
	arms_[kSword]->SetRotationX(wave);
	//arms_[kRArm]->SetRotationX(-wave); // 反対方向
}

// 調整項目の適用
void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	kAcceleration_ = globalVariables->GetFloatValue(groupName, "kAcceleration");
	kAttenuation_ = globalVariables->GetFloatValue(groupName, "kAttenuation");
	kLimitRunSpeed_ = globalVariables->GetFloatValue(groupName, "kLimitRunSpeed");
	//attackVelocity_ = globalVariables->GetVector3Value(groupName, "attackVelocity_");
	size_ = globalVariables->GetFloatValue(groupName, "size");
	kHp_ = globalVariables->GetIntValue(groupName, "kHp_");
	//attackPower_ = globalVariables->GetFloatValue(groupName, "attackPower_");
	//powerMagnification_ = globalVariables->GetFloatValue(groupName, "powerMagnification_");
}

// 移動処理
void Player::Move() {
	Matrix4x4 rotateMatrix;
	Vector3 move{};
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		Input::GetInstance()->SetJoystickDeadZone(0, 3000, 3000);
		// 移動量
		move = { (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f, (float)joyState.Gamepad.sThumbLY / SHRT_MAX };
		move = kAcceleration_ * move;
	}
	if (move.Length() == 0) {
		if (Input::GetInstance()->PushKey(DIK_D)) {
			move.x += kAcceleration_;
		}
		if (Input::GetInstance()->PushKey(DIK_A)) {
			move.x -= kAcceleration_;
		}
		if (Input::GetInstance()->PushKey(DIK_W)) {
			move.z += kAcceleration_;
		}
		if (Input::GetInstance()->PushKey(DIK_S)) {
			move.z -= kAcceleration_;
		}
	}

	if (move.Length() != 0) {
		// 入力がある場合の処理
		if (velocity_.x < 0.0f && move.x > 0.0f ||
			velocity_.x > 0.0f && move.x < 0.0f) {
			velocity_.x *= (1.0f - kAttenuation_);
		}
		if (velocity_.z < 0.0f && move.z > 0.0f ||
			velocity_.z > 0.0f && move.z < 0.0f) {
			velocity_.z *= (1.0f - kAttenuation_);
		}

		rotateMatrix = MakeRotateXYZMatrix(viewProjection_->rotation_);
		move = Transformation(move, rotateMatrix);
		move.y = 0.0f;
		VectorRotation(move);

		velocity_ += move;
	}
}

// 予備動作の方向
void Player::DirectionPreliminaryAction()
{
	// 方向をセット
	aimingDirection_ = { InputDirection().x, 0.0, InputDirection ().y };
	aimingDirection_ *= 5.0f;
}

// 振り下ろし(上入力攻撃)の初期化
void Player::AttackTypeDownSwingInitialize()
{
	attack_.armStart = arms_[kSword]->GetTranslation();
	attack_.armEnd = { arms_[kSword]->GetTranslation().x + aimingDirection_.x, arms_[kSword]->GetTranslation().y - aimingDirection_.z, arms_[kSword]->GetTranslation().z + aimingDirection_.z };
	attack_.time = 0.0f;
}

// 振り下ろし(上入力攻撃)の更新
void Player::AttackTypeDownSwingUpdate()
{
	Vector3 newPos = EaseInSine(attack_.armStart, attack_.armEnd, attack_.time, attack_.kLimitTime);

	arms_[kSword]->SetTranslation(newPos);
}

// 突き(下入力攻撃)の初期化
void Player::AttackTypeThrustInitialize()
{

}

// 突き(下入力攻撃)の更新
void Player::AttackTypeThrustUpdate()
{

}

// 右振り抜き(左入力攻撃)の初期化
void Player::AttackTypeLeftSwingInitialize()
{

}

// 右振り抜き(左入力攻撃)の更新
void Player::AttackTypeLeftSwingUpdate()
{

}

// 左振り抜き(右入力攻撃)の初期化
void Player::AttackTypeRightSwingInitialize()
{

}

// 左振り抜き(右入力攻撃)の更新
void Player::AttackTypeRightSwingUpdate()
{

}

// 未入力
void Player::AttackTypeNullInitialize()
{
}
void Player::AttackTypeNullUpdate()
{
}

// 向きをセット
void Player::VectorRotation(const Vector3& direction) {
	Vector3 move = direction;
	transform_.rotation_.y = std::atan2f(move.x, move.z);
	Vector3 velocityZ = Transformation(move, MakeRotateYMatrix(-transform_.rotation_.y));
	transform_.rotation_.x = std::atan2f(-velocityZ.y, velocityZ.z);
}

// 方向を取得
Vector2 Player::InputDirection()
{
	XINPUT_STATE joyState;
	Input::GetInstance()->GetJoystickState(0, joyState);
	Vector3 input = { (float)joyState.Gamepad.sThumbRX / SHRT_MAX, (float)joyState.Gamepad.sThumbRY / SHRT_MAX, 0.0f };
	input = input.Normalize();
	return { input.x, input.y };
}

void Player::ImGui()
{
	if (ImGui::Begin("Player Coordinates")) {
		ImGui::PushID(id_);
		// 座標情報を表示し、DragFloat3で編集可能にする
		ImGui::Text("Position:");
		ImGui::DragFloat3("Translation", &transform_.translation_.x, 0.1f);

		ImGui::Text("velocity_:");
		ImGui::DragFloat3("velocity_", &velocity_.x, 0.1f);

		//ImGui::Text("Scale:");
		//ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);

		//ImGui::Text("velocityLength:");
		//velocityLength = velocity_.Length();
		//ImGui::DragFloat("velocityLength", &velocityLength);

		//ImGui::Text("velocityLengthW:");
		//velocityLengthW = (attackPower_ + velocity_.Length() * powerMagnification_);
		//ImGui::DragFloat("velocityLengthW", &velocityLengthW);

		ImGui::Text("HP:");
		ImGui::DragInt("HP", &hp_);
		XINPUT_STATE joyState;
		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			ImGui::Text("RB:true");
			ImGui::Text("RStick:(%4.2f, %4.2f)", aimingDirection_.x, aimingDirection_.z);
		}
		else {
			ImGui::Text("RB:false");
		}
		ImGui::PopID();
		ImGui::End();
	}
	int emitterId = 0;
	/*for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		ImGui::PushID(emitterId);
		emitter_->imgui();
		ImGui::PopID();
		++emitterId;
	}*/
}

Vector3 Player::GetWorldPosition()
{
	return Vector3();
}

void Player::SetBehavior(Behavior newBehavior)
{
	if (behavior_ != newBehavior) {
		behavior_ = newBehavior;
		(this->*BehaviorInitFuncTable[static_cast<size_t>(behavior_)])();
	}
}

Vector3 Player::GetCenterPosition() const {
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

Vector3 Player::GetCenterRotation() const {
	return transform_.rotation_;
}
void(Player::* Player::BehaviorInitFuncTable[])() = {
	&Player::BehaviorRootInitialize,
	&Player::BehaviorDashInitialize,
	&Player::BehaviorPostureAttackInitialize,
	&Player::BehaviorAttackInitialize,
	&Player::BehaviorProtectionInitialize,
	&Player::BehaviorCelebrateInitialize,
};
void(Player::* Player::BehaviorUpdateFuncTable[])() = {
	&Player::BehaviorRootUpdate,
	&Player::BehaviorDashUpdate,
	&Player::BehaviorPostureAttackUpdate,
	&Player::BehaviorAttackUpdate,
	&Player::BehaviorProtectionUpdate,
	&Player::BehaviorCelebrateUpdate,
};

void(Player::* Player::AttackTypeInitFuncTable[])() = {
	&Player::AttackTypeDownSwingInitialize,
	&Player::AttackTypeThrustInitialize,
	&Player::AttackTypeLeftSwingInitialize,
	&Player::AttackTypeRightSwingInitialize,
	&Player::AttackTypeNullInitialize,
};
void(Player::* Player::AttackTypeUpdateFuncTable[])() = {
	&Player::AttackTypeDownSwingUpdate,
	&Player::AttackTypeThrustUpdate,
	&Player::AttackTypeLeftSwingUpdate,
	&Player::AttackTypeRightSwingUpdate,
	&Player::AttackTypeNullUpdate,
};