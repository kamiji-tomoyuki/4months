#pragma once
#include "BaseObject.h"
#include "BaseEnemyState.h"

class TimeManager;
class Player;
class Enemy : public BaseObject {
public:
	enum class Behavior {
		kRoot,//通常状態
		kApproach,//接近
		kLeave,//離脱
		kAttack,//攻撃中
		kDefense,//防御中
	};
	struct BehaviorProbability
	{
		float kAttack;//攻撃確率
		float kDefense;//防御確率
	};
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

	//当たり判定
	virtual Vector3 GetCenterPosition() const override = 0;
	virtual Vector3 GetCenterRotation() const override = 0;
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
	//確率でtrueを返す
	bool GetProbabilities(float probabilities);
	//方向の回転
	void VectorRotation(const Vector3& direction);

protected:
	//ポインタ
	Player* player_ = nullptr;
	TimeManager* timeManager_ = nullptr;
	//ステート
	std::unique_ptr<BaseEnemyState> state_;
	//物理
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	//シリアルナンバー
	uint32_t serialNumber_ = 0;
	//次のシリアルナンバー
	static uint32_t nextSerialNumber_;
	//命
	bool isAlive_ = true;
	int kHp_ = 10000;
	int hp_ = 5;
	//行動距離
	float shortDistance_ = 10.0f;
	float middleDistance_ = 50.0f;
	//行動確率
	BehaviorProbability shortDistanceProbability_ = { 0.60f,0.10f };
	BehaviorProbability middleDistanceProbability_ = { 0.50f ,0.05f };
	BehaviorProbability longDistanceProbability_ = { 0.01f,0.01f };
	//行動クールタイム
	float kCoolTime_ = 0.3f;
public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetTimeManager(TimeManager* timeManager) { timeManager_ = timeManager; }
	void SetTranslation(const Vector3& translation);
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	void SetHP(int hp) { hp_ = hp; }
	void SetScale(const Vector3& scale) {
		transform_.scale_ = scale;  // **スケールを適用**
	}

	Player* GetPlayer() { return player_; }
	TimeManager* GetTimeManager() { return timeManager_; }
	Vector3 GetVelocity() { return velocity_; }
	uint32_t GetSerialNumber() const { return serialNumber_; }
	bool GetIsAlive() { return isAlive_; }
	int GetHP() { return hp_; }
	float GetShortDistance() { return shortDistance_; }
	float GetMiddleDistance() { return middleDistance_; }
	BehaviorProbability GetShortDistanceProbability() { return shortDistanceProbability_; }
	BehaviorProbability GetMiddleDistanceProbability() { return middleDistanceProbability_; }
	BehaviorProbability GetLongDistanceProbability() { return longDistanceProbability_; }
	float GetCoolTime() { return kCoolTime_; }
};
