#pragma once
#include "Enemy.h"
#include "EnemySword.h"

class Soldier : public Enemy{
public:

public:
	Soldier();
	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

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

	void ApplyGlobalVariables();
private:
	const char* groupName = "Soldier";
	bool isMove_ = true;

	std::unique_ptr<EnemySword> sword_;
public:
	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
};

