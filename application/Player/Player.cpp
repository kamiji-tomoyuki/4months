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
#include "Enemy.h"
#include "PlayerStateRoot.h"
#include "PlayerAttackStateNoInput.h"

using namespace std::numbers;

Player::Player() {
	id_ = playerID_;
	playerID_++;
}

// 初期化
void Player::Init() {
	//基底クラスの初期化
	BaseObject::Init();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));
	Collider::SetAABBScale({ 0.0f,0.0f,0.0f });
	InitializeFloatingGimmick();
	//プレイヤーの剣
	sword_ = std::make_unique<PlayerSword>();
	sword_->SetPlayer(this);
	sword_->SetTimeManager(timeManager_);

	// ステートの初期化
	state_ = std::make_unique<PlayerStateRoot>(this);
	state_->Initialize();

	model_ = std::make_unique<Object3d>();
	model_->Initialize("player/player.gltf");

	sword_->Initialize("sword/sword.obj");

	sword_->SetID(id_);
	sword_->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
	sword_->SetTranslation(Vector3(1.5f, 0.0f, 0.0f));
	sword_->SetScale(Vector3(0.8f, 0.8f, 0.8f));
	
	attackDirection_ = { 0.0f, 0.0f, 0.0f };

	//imgui
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	Input::GetInstance()->SetJoystickDeadZone(0, 4000, 4000);

	for (int i = 0; i < 2; ++i) {
		std::unique_ptr<ParticleEmitter> emitter_;
		emitter_ = std::make_unique<ParticleEmitter>();
		emitters_.push_back(std::move(emitter_));
	}
	emitters_[0]->Initialize("Dust.json");
	emitters_[0]->Start();

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
	Collider::SetRadius(size_ * 1.3f);

	transform_.scale_ = { size_,size_ ,size_ };

	deleteScale_ = transform_.scale_;
}

// 更新
void Player::Update() {
	ApplyGlobalVariables();

	//基底クラス更新
	BaseObject::Update();

	if (isAlive_) {

		state_->Update();

	} else {

		Dead();
	}

	// 速度に減衰をかける
	velocity_.x *= (1.0f - kAttenuation_);
	velocity_.z *= (1.0f - kAttenuation_);
	//Yなし
	velocity_.y = 0.0f;

	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed_, kLimitRunSpeed_);
	velocity_.z = std::clamp(velocity_.z, -kLimitRunSpeed_, kLimitRunSpeed_);

	Collider::SetRadius(size_ * 1.2f);
	Collider::SetAABBScale({ 0.0f,0.0f,0.0f });

	transform_.translation_ += velocity_ * timeManager_->deltaTime_;
	transform_.translation_.y = GetRadius();
	transform_.UpdateMatrix();

	sword_->SetSize(size_ * 1.3f);
	sword_->Update();
	
	model_->AnimationUpdate(true);
}

// パーティクルの更新
void Player::UpdateParticle(const ViewProjection& viewProjection) {
	for (std::unique_ptr<ParticleEmitter>& emitter_ : emitters_) {
		emitter_->SetPosition(GetCenterPosition());
		emitter_->Update();
	}
	sword_->UpdateParticle(viewProjection);
}

// 描画
void Player::Draw(const ViewProjection& viewProjection) {
	// 剣の描画
	sword_->Draw(viewProjection);
}

// パーティクルの描画
void Player::DrawParticle(const ViewProjection& viewProjection) {
	
	// 剣のパーティクル描画
	sword_->DrawParticle(viewProjection);
}

// アニメーションの描画
void Player::DrawAnimation(const ViewProjection& viewProjection)
{
	// プレイヤーの描画
	model_->Draw(BaseObject::GetWorldTransform(), viewProjection);
}

// 当たってる間
void Player::OnCollision([[maybe_unused]] Collider* other) {

	if (isGameOver_) return; // GameOver中は衝突処理をスキップ

	if (timeManager_->GetTimer("start").isStart || timeManager_->GetTimer("collision").isStart) {
		return;
	}
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy) ||
		typeID == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
		Enemy* enemy = static_cast<Enemy*>(other);
		if (enemy->GetSerialNumber() == enemy->GetNextSerialNumber() - 1) {
			return;
		}
		// 衝突後の新しい速度を計算
		auto [newVelocity1, newVelocity2] = ComputeCollisionVelocities(
			1.0f, GetVelocity(), 1.0f, enemy->GetVelocity(), 1.0f, Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Normalize()
		);

		// 計算した速度でボールの速度を更新
		SetVelocity(newVelocity1);
		enemy->SetVelocity(newVelocity2);

		float distance = Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Length();

		Vector3 correction = Vector3(GetCenterPosition() - enemy->GetCenterPosition()).Normalize() * (GetRadius() + enemy->GetRadius() - distance) * 0.750f;
		transform_.translation_ += correction;
		enemy->SetTranslation(enemy->GetTransform().translation_ - correction);

		//timeManager_->SetTimer("collision", timeManager_->deltaTime_ * 3.0f);

	}

	transform_.UpdateMatrix();
}

// 当たった瞬間
void Player::OnCollisionEnter([[maybe_unused]] Collider* other) {
	
}

// 当たり終わった瞬間
void Player::OnCollisionOut([[maybe_unused]] Collider* other) {

}

#ifdef _DEBUG
// ImGui
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
		if (/*Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER*/
			Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.bRightTrigger/* & XINPUT_GAMEPAD_RIGHT_SHOULDER*/) {
			ImGui::Text("RB:true");
			ImGui::Text("RStick:(%4.2f, %4.2f)", aimingDirection_.x, aimingDirection_.y);
			float cosTheta = atan2f(aimingDirection_.y, aimingDirection_.x);
			ImGui::Text("cosTheta:(%4.2f)", cosTheta);
		}
		else {
			ImGui::Text("RB:false");
		}
		ImGui::PopID();
		ImGui::End();
	}

	sword_->ImGui();
}
#endif // _DEBUG

// 移動処理
void Player::Move() {
	Matrix4x4 rotateMatrix;
	Vector3 move{};

	// ゲームパッド入力処理
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		Input::GetInstance()->SetJoystickDeadZone(0, 3000, 3000);
		// 移動量
		move = { (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f, (float)joyState.Gamepad.sThumbLY / SHRT_MAX };
		move = kAcceleration_ * move;
	}
#ifdef _DEBUG
	// キーボード入力処理
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
#endif // _DEBUG

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

// 入力方向
int Player::InputDirection()
{
	// 使用する変数
	Vector3 joyStickDireciton{};
	float cosTheta = 0.0f;

	// 方向をセット
	joyStickDireciton = { InputDirectionGampad().x, InputDirectionGampad().y, 0.0f };
	// ゲームパッドの入力角度を算出
	cosTheta = atan2f(joyStickDireciton.y, joyStickDireciton.x);

	// 上
	if (cosTheta > 0.25f * pi_v<float> && cosTheta < 0.75f * pi_v<float>) {
		return TOP;
	}
	// 下
	if (cosTheta < -0.25f * pi_v<float> && cosTheta > -0.75f * pi_v<float>) {
		return DOWN;
	}
	// 左
	if (cosTheta >= 0.75f * pi_v<float> || cosTheta <= -0.75f * pi_v<float>) {
		return LEFT;
	}
	// 右
	if ((cosTheta <= 0.25f * pi_v<float> && cosTheta >= -0.25f * pi_v<float>) && !(joyStickDireciton.x == 0.0f && joyStickDireciton.y == 0.0f)) {
		return RIGHT;
	}
	return Nothing;
}

// 入力方向の設定
void Player::SetInputDirection()
{
	attackDirection_ = { 0.0f, 0.0f, 0.0f };
	// ゲームパッド接続されているか
	bool isGamPadConnect = Input::GetInstance()->IsAnyJoystickConnected();

	// ゲームパッド入力処理
	if (isGamPadConnect) {
		// 上
		if (InputDirection() == TOP) {
			attackDirection_.y += 1.0f;
		}
		// 下
		if (InputDirection() == DOWN) {
			attackDirection_.y -= 1.0f;
		}
		// 左
		if (InputDirection() == LEFT) {
			attackDirection_.x -= 1.0f;
			attackDirection_.y = 0.0f;
		}
		// 右
		if (InputDirection() == RIGHT) {
			attackDirection_.x += 1.0f;
			attackDirection_.y = 0.0f;
		}
	}

#ifdef _DEBUG
	// キーボード入力処理
	if (attackDirection_.Length() == 0)
	{
		// 上
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			attackDirection_.y += 1.0f;
		}
		// 下
		if (Input::GetInstance()->PushKey(DIK_DOWN)) {
			attackDirection_.y -= 1.0f;
		}
		// 左
		if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			attackDirection_.x -= 1.0f;
			attackDirection_.y = 0.0f;
		}
		// 右
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			attackDirection_.x += 1.0f;
			attackDirection_.y = 0.0f;
		}
	}
#endif // _DEBUG

	attackDirection_ *= 5.0f;
}

// ステートチェンジ
void Player::ChangeState(std::unique_ptr<PlayerBaseState> state)
{
	state_ = std::move(state);
	state_->Initialize();
}

// ダッシュ入力
bool Player::IsDashInput()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			return true;
		}
	}
	return false;
}

// 予備動作入力
bool Player::IsPreliminaryInput()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && IsAttackDirectionInput()) {
		return true;
	}
	return false;
}

// 攻撃入力
bool Player::IsAttackInput()
{
	XINPUT_STATE joyState;
	if ((Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) &&
		!attack_.isAttack) {
		return true;
	}
	return false;
}

// 防御入力
bool Player::IsGuard()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		return true;
	}
	return false;
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
}

/// -------------getter-------------

// 中心座標を取得
Vector3 Player::GetCenterPosition() const {
	//ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	//ワールド座標に変換
	Vector3 worldPos = Transformation(offset, transform_.matWorld_);
	return worldPos;
}

// 中心回転を取得
Vector3 Player::GetCenterRotation() const {
	return transform_.rotation_;
}

// ワールド座標を取得
Vector3 Player::GetWorldPosition()
{
	return Vector3();
}

// 方向を取得
Vector2 Player::InputDirectionGampad()
{
	XINPUT_STATE joyState;
	Vector3 input{};
	if (Input::GetInstance()->IsAnyJoystickConnected())
	{
		Input::GetInstance()->GetJoystickState(0, joyState);
		input = { (float)joyState.Gamepad.sThumbRX / SHRT_MAX, (float)joyState.Gamepad.sThumbRY / SHRT_MAX, 0.0f };
		input = input.Normalize();
	}
	return { input.x, input.y };
}

Vector3 Player::GetAttackDirection()
{
	if (lockOn_) {
		return lockOn_->GetTargetPosition() - transform_.translation_;
	}
	return aimingDirection_;
}

/// -------------getter-------------

/// -------------setter-------------

// 向きをセット
void Player::VectorRotation(const Vector3& direction) {
	Vector3 move = direction;
	transform_.rotation_.y = std::atan2f(move.x, move.z);
	Vector3 velocityZ = Transformation(move, MakeRotateYMatrix(-transform_.rotation_.y));
	transform_.rotation_.x = std::atan2f(-velocityZ.y, velocityZ.z);
}

/// -------------setter-------------

/// -----------メンバ関数-----------

// 攻撃方向入力されたか
bool Player::IsAttackDirectionInput()
{
	if (attackDirection_.x != 0.0f || attackDirection_.y != 0.0f || attackDirection_.z != 0.0f) {
		return true;
	}
	return false;
}

void Player::Dead() {

	deleteTimer_ += deleteSpeed_;

	if (deleteTimer_ >= 1.0f) {
		deleteTimer_ = 1.0f;
	}

	transform_.scale_ = EaseInBack(deleteScale_, Vector3(0.0f, 0.0f, 0.0f), deleteTimer_, 1.0f);

	if (deleteTimer_ >= 1.0f) {

		isGameOver_ = true;
	}
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
}
