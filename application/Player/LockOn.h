#pragma once
#include <memory>
#include <list>
#include "Sprite.h"
#include "ViewProjection.h"

class Enemy;
class LockOn {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
private:
	void Search(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);
	bool IsOutRange(const ViewProjection& viewProjection);
	Matrix4x4 MakeViewProjectionViewPort(const ViewProjection& viewProjection);
	//ロックオンマーク用スプライト
	//std::unique_ptr<Sprite> lockOnMark_;
	//ロックオン対象
	Enemy* target_ = nullptr;
	// 最小距離
	float minDistance_ = 10.0f;
	// 最大距離
	float maxDistance_ = 3000.0f;

	float kDegreeToRadian = 1.0f;
	// 角度範囲
	float angleRange = 20.0f * kDegreeToRadian;

	bool preLockOnButton_ = false;
public:
	Vector3 GetTargetPosition() const;
	bool ExistTarget() const { return target_ ? true : false; }
	void ResetTarget() { target_ = nullptr; }
};
