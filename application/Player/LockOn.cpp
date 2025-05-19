#include "LockOn.h"
#include "TextureManager.h"
#include "Input.h"
#include "myMath.h"
#include <WinApp.h>
#include "Enemy.h"

void LockOn::Initialize() {

}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies,const ViewProjection& viewProjection) {
	XINPUT_STATE joyState;
	
	if (target_) {
		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A && !preLockOnButton_ ||
			Input::GetInstance()->TriggerKey(DIK_R)) {
			// ロックオンを外す
			target_ = nullptr;
		}
		//範囲外判定
		else if (IsOutRange(viewProjection)) {
			//ロックオンを外す
			target_ = nullptr;
		}
	} else {
		//ロックオンボタンを押したら
		if (Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A && !preLockOnButton_ ||
			Input::GetInstance()->TriggerKey(DIK_R)) {
			//ロックオン対象の検索
			Search(enemies, viewProjection);
		}
	}

	//ロックオン状態ならロックオン継続
	//if (target_) {
	//	//敵のロックオン座標取得
	//	Vector3 positionWorld = target_->GetCenterPosition();
	//	//ワールド座標からスクリーン座標
	//	Vector3 positionScreen = Transformation(positionWorld, MakeViewProjectionViewPort(viewProjection));
	//	//Vector2に変換
	//	Vector2 positionScreenV2(positionScreen.x, positionScreen.y);
	//	//スプライトの座標を設定
	//	lockOnMark_->SetPosition(positionScreenV2);
	//}

	preLockOnButton_ = Input::GetInstance()->GetJoystickState(0, joyState) && joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
}

void LockOn::Draw() { 
	if (target_) {
		//lockOnMark_->Draw();
	}
}

void LockOn::Search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	//ロックオンの絞り込み
	//目標
	std::list<std::pair<float, Enemy*>> targets;

	//全ての敵に対して順にロックオン判定
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		//敵のロックオン座標取得
		Vector3 positionWorld = enemy->GetCenterPosition();

		//ワールドビュー座標変換
		Vector3 positionView = Transformation(positionWorld, viewProjection.matView_);

		//距離条件チェック
		if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
			//カメラ前方との角度を計算
			float arcTangent = std::atan2(std::sqrtf(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);
			//角度条件チェック
			if (std::fabs(arcTangent) <= angleRange) {
				targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
			}
		}
	}

	//距離でソートしてロックオン対象を決定
	//ロックオン対象をリセット
	target_ = nullptr;
	if (targets.begin() != targets.end()) {
		//距離で昇順にソート
		targets.sort([](auto& pair1, auto& pair2) { return pair1.first < pair2.first; });
		//ソートの結果一番近い敵をロックオン対象とする
		target_ = targets.front().second;
	}
}

bool LockOn::IsOutRange(const ViewProjection& viewProjection) {
	// 敵のロックオン座標取得
	Vector3 positionWorld = target_->GetCenterPosition();
	// ワールド座標からスクリーン座標
	Vector3 positionView = Transformation(positionWorld, viewProjection.matView_);
	//距離条件チェック
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
		// カメラ前方との角度を計算
		float arcTangent = std::atan2(std::sqrtf(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);
		// 角度条件チェック
		if (std::fabs(arcTangent) <= angleRange) {
			//範囲外ではない
			return false;
		}
	}
	//範囲外
	return true;
}

Matrix4x4 LockOn::MakeViewProjectionViewPort(const ViewProjection& viewProjection) {
	Matrix4x4 viewProjectionMatrix = viewProjection.matView_ * viewProjection.matProjection_;
	Matrix4x4 matViewport = MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);
	Matrix4x4 viewProjectionViewPort = viewProjectionMatrix * matViewport;
	return viewProjectionViewPort;
}

Vector3 LockOn::GetTargetPosition() const {
	
	if (ExistTarget()) {
		return target_->GetCenterPosition();
	}		
	return Vector3();
}
