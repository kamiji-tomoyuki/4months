#pragma once
#include "BaseEnemySword.h"

class BossSword : public BaseEnemySword{
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string filePath) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	 void Draw(const ViewProjection& viewProjection)override;

	 void DrawAnimation(const ViewProjection& viewProjection)override;

	/// <summary>
	/// 当たってる間
	/// </summary>
	/// <param name="other"></param>
	 void OnCollision([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 当たった瞬間
	/// </summary>
	/// <param name="other"></param>
	 void OnCollisionEnter([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 当たり終わった瞬間
	/// </summary>
	/// <param name="other"></param>
	 void OnCollisionOut([[maybe_unused]] Collider* other) override;

	// 中心座標を取得
	 Vector3 GetCenterPosition() const override;
	 Vector3 GetCenterRotation() const override;
private:
	
};

