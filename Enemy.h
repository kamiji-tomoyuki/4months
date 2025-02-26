#pragma once
#include "BaseObject.h"

class Enemy : public BaseObject {
public:
	Enemy();
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

private:
	//シリアルナンバー
	uint32_t serialNumber_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNumber_;

	bool isAlive_ = true;

	int hp_ = 5;

public:
	void SetTranslation(const Vector3& translation);
	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
	uint32_t GetSerialNumber() const { return serialNumber_; }
	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	int GetHP() { return hp_; }
	void SetHP(int hp) { hp_ = hp; }
};
