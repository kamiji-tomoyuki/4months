#include "FollowCamera.h"
#include "Input.h"
#include <algorithm> 
#include <cmath>
#include <random>
#include "TimeManager.h"
#include "LockOn.h"
#include "GlobalVariables.h"

void FollowCamera::Initialize() {
	viewProjection_.Initialize();
	destinationAngle = Quaternion::IdentityQuaternion();
	//imgui
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "offset_", offset_);
	globalVariables->AddItem(groupName, "destinationAngleX_", destinationAngleX_);
	globalVariables->AddItem(groupName, "destinationAngleY_", destinationAngleY_);

	destinationAngleX_ = globalVariables->GetFloatValue(groupName, "destinationAngleX_");
	destinationAngleY_ = globalVariables->GetFloatValue(groupName, "destinationAngleY_");
}

void FollowCamera::Update() {
	ApplyGlobalVariables();
	// ジョイスティック
	XINPUT_STATE joyState;
	// 速さ
	Vector3 move{};
	const float speed = 0.03f;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 移動量

		move = { -(float)joyState.Gamepad.sThumbRY / SHRT_MAX, (float)joyState.Gamepad.sThumbRX / SHRT_MAX, 0.0f };

		// 右Rスティック
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			//destinationAngleX_ = target_->rotation_.x;
			destinationAngleY_ = target_->rotation_.y;
		}

		if (move.Length() != 0) {
			move = move.Normalize();
			//destinationAngleX_ += (speed * move).x;
			destinationAngleY_ += (speed * move).y;
		}
	}
	else {
		// 移動量
		if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			move += Vector3(0.0f, -1.0f, 0.0f);
		}
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			move += Vector3(0.0f, 1.0f, 0.0f);
		}
		//if (Input::GetInstance()->PushKey(DIK_UP)) {
		//	move += Vector3(-1.0f, 0.0f, 0.0f);
		//}
		//if (Input::GetInstance()->PushKey(DIK_DOWN)) {
		//	move += Vector3(1.0f, 0.0f, 0.0f);
		//}

		// Enter
		if (Input::GetInstance()->PushKey(DIK_RETURN)) {
			//destinationAngleX_ = target_->rotation_.x;
			destinationAngleY_ = target_->rotation_.y;
		}

		if (move.Length() != 0) {
			move = speed * move.Normalize();
			//destinationAngleX_ += move.x;
			destinationAngleY_ += move.y;
		}
	}

	if (lockOn_->ExistTarget()) {
		//ロックオン座標
		Vector3 lockOnPos = lockOn_->GetTargetPosition();
		//追従対象からのロックオン対象へのベクトル
		Vector3 sub = lockOnPos - target_->translation_;
		//Y軸周りの角度
		//viewProjection_.rotation_.y = std::atan2(sub.x, sub.z);
		destinationAngleY_ = std::atan2(sub.x, sub.z);
	}

	destinationAngle = Quaternion::Sleap(destinationAngle, Quaternion::MakeRotateAxisAngleQuaternion({ 0,0,-1 }, destinationAngleX_) * Quaternion::MakeRotateAxisAngleQuaternion({ 0,-1,0 }, destinationAngleY_), 0.1f);
	viewProjection_.rotation_ = destinationAngle.ToEulerAngles();
	// 追従対象がいれば
	if (target_) {
		// 追従座標の補完
		interTarget_ = Lerp(interTarget_, target_->translation_, 0.2f);

		// 追従対象からのオフセット
		Vector3 offset = MakeOffset();
		// カメラ座標
		viewProjection_.translation_ = interTarget_ + offset;
	}
	Shaking();
	viewProjection_.UpdateMatrix();
}

void FollowCamera::Reset() {
	if (target_) {
		//追従対象の初期化
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	destinationAngleY_ = viewProjection_.rotation_.y;

	//追従対象からのオフセット
	Vector3 offset = MakeOffset();
	viewProjection_.translation_ = interTarget_ + offset;
}

void FollowCamera::ShakeStart(Vector2 move,float kTime){
	shake_.move = move;
	shake_.preMove = {};
	shake_.kTime = kTime;
	shake_.time = 0.0f;
	shake_.isShake = true;
}
Vector3 FollowCamera::MakeOffset() {
	//回転行列の合成
	Matrix4x4 rotateMatrix = MakeAffineMatrix({1, 1, 1}, viewProjection_.rotation_, {});
	//オフセットをカメラの回転に合わせて回転させる
	Vector3 result = TransformNormal(offset_,rotateMatrix);
	return result;
}
void FollowCamera::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	offset_ = globalVariables->GetVector3Value(groupName, "offset_");
}
void FollowCamera::Shaking(){
	if (!shake_.isShake) {
		return;
	}
	shake_.time += TimeManager::deltaTime_;
	viewProjection_.translation_ -= Vector3(shake_.preMove.x, shake_.preMove.y, 0);
	float timeCount = shake_.time / shake_.kTime;
	if (timeCount > 1.0f) {
		shake_.isShake = false;
		return;
	}
	static std::random_device rd;
	static std::mt19937 gen(rd());
	float shakeSize = 0;
	if (timeCount <= 0.5f) {
		shakeSize = timeCount * 2.0f;
	}
	else {
		shakeSize = 1.0f - (timeCount * 2.0f - 0.5f);
	}
	static std::uniform_real_distribution<float> distX(-shake_.move.x * shakeSize, shake_.move.x * shakeSize);
	static std::uniform_real_distribution<float> distY(-shake_.move.y * shakeSize, shake_.move.y * shakeSize);

	shake_.preMove = { distX(gen), distY(gen) };
	viewProjection_.translation_ += Vector3(shake_.preMove.x, shake_.preMove.y, 0);
}
float FollowCamera::LerpShortAngle(const float& v1, const float& v2, float t) {
	float v3;
	v3 = v1 + t * (v2 - v1);
	return v3;
}
void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}
