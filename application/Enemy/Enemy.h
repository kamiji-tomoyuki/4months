#pragma once
#include "BaseObject.h"
#include "BaseEnemyState.h"

class Player;
class Enemy : public BaseObject {
public:
	Enemy();
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(const ViewProjection& viewProjection)override;

	/// <summary>
	/// 当たってる間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollision([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 当たった瞬間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollisionEnter([[maybe_unused]] Collider* other) override;

	/// <summary>
	/// 当たり終わった瞬間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollisionOut([[maybe_unused]] Collider* other) override;

	//Updateのステートチェンジ
	void ChangeState(std::unique_ptr<BaseEnemyState> state);

private:
	//ポインタ
	Player* player_ = nullptr;
	//ステート
	std::unique_ptr<BaseEnemyState> state_;
	//シリアルナンバー
	uint32_t serialNumber_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNumber_;

	bool isAlive_ = true;

	int hp_ = 5;

public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetTranslation(const Vector3& translation);
	virtual Vector3 GetCenterPosition() const override = 0;
	virtual Vector3 GetCenterRotation() const override = 0;
	uint32_t GetSerialNumber() const { return serialNumber_; }
	bool GetIsAlive() { return isAlive_; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	int GetHP() { return hp_; }
	void SetHP(int hp) { hp_ = hp; }
};
