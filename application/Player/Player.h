#pragma once
#include <numbers>
#include <array>
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "BaseObject.h"
#include "optional"
#include "PlayerArm.h"
#include "TimeManager.h"
#include "ParticleEmitter.h"

class FollowCamera;
class LockOn;
/// <summary>
/// 自キャラ
/// </summary>
class Player :public BaseObject {
public:
	enum ModelState {
		kLArm,
		kRArm,
		kModelNum,
	};
	enum class Behavior {
		kRoot,//通常状態
		kDash,//ダッシュ中
		kAttack,//攻撃中
		kGrab,//掴み中
		kCelebrate, // 喜び動作
	};
	struct Root {
		float floatingParameter = 0.0f;//浮遊ギミックの媒介変数
		int32_t period = 60;// 浮遊移動のサイクル<frame>
		float floatingAmplitude = 0.2f;// 浮遊の振幅<m>
		float armAmplitude = 0.4f;//アームの振幅
	};
	struct Attack {
		float kLimitTime = 0.4f;
		float armStart = 0.0f;
		float armEnd = 3.0f;
		float time = 0;
		bool isAttack = false;
		bool isLeft = false;
	};
	struct Grab {
		float kLimitTime = 0.5f;
		float armStartL = 0.0f;
		float armStartR = 0.0f;
		float armEnd = 3.0f;
		float time = 0;
		bool isGrab = false;
	};
	struct WorkDash {
		float kDashTime_ = 0.3f;
		float DashTime_ = 0;
		float kAttenuation_ = 1.50f;
	};

	Player();

	~Player() {}
	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	void UpdateParticle(const ViewProjection& viewProjection);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection) override;
	void DrawParticle(const ViewProjection& viewProjection);
	void DrawAnimation(const ViewProjection& viewProjection);

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

	// ImGui
	void ImGui();

	// 向きをセット
	void VectorRotation(const Vector3& direction);
	// 中心座標を取得
	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
private:	// 動作パターン
	// 浮遊動作(無入力)
	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

	// 通常動作
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	// ダッシュ動作
	void BehaviorDashInitialize();
	void BehaviorDashUpdate();

	// 攻撃動作
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	// 掴む動作
	void BehaviorGrabInitialize();
	void BehaviorGrabUpdate();

	// 勝利(喜ぶ)動作
	void BehaviorCelebrateInitialize();
	void BehaviorCelebrateUpdate();

	// 調整項目の適用
	void ApplyGlobalVariables();

	// 移動
	void Move();

	// メンバ変数
	// 動作パターン
	Behavior behavior_ = Behavior::kRoot;
	static void(Player::* BehaviorInitFuncTable[])();
	static void(Player::* BehaviorUpdateFuncTable[])();
	// 動作パターンのリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	const ViewProjection* viewProjection_ = nullptr;

	// 生存フラグ
	bool isAlive_ = true;

	// 各動作に必要なデータ
	Root root_;
	Attack attack_;
	Grab grab_;
	WorkDash workDash_;

	// 移動速度 減衰速度
	float kAcceleration_ = 0.1f;
	float kAttenuation = 0.1f;
	float kLimitRunSpeed = 5.0f;

	// 攻撃速度 攻撃距離
	//Vector3 attackVelocity_ = { 0.0f,0.0f,5.0f };
	//float velocityLength = 0.0f;
	//float velocityLengthW = 0.0f;

	// サイズ
	float size_ = 1.0f;
	// 速度
	Vector3 velocity_{};
	Vector3 acceleration_{};

	// 狙う方向
	Vector3 aimingDirection_{};

	// HP
	int kHp_ = 10000;
	int hp_ = kHp_;
	//float attackPower_ = 10;
	//float powerMagnification_ = 1.0f;
	bool isClear_ = false;
	bool isGameOver_ = false;

	std::array<std::unique_ptr<PlayerArm>, kModelNum> arms_;
	// パーティクルエミッタ
	//std::vector<std::unique_ptr<ParticleEmitter>> emitters_;
	// ポインタ
	TimeManager* timeManager_ = nullptr;
	FollowCamera* followCamera_ = nullptr;
	LockOn* lockOn_ = nullptr;

	static inline int playerID_ = 0;
	int id_ = 0;

	// 喜び動作用の時間
	//float celebrateTime_ = 0.0f;

public:
	int GetID() { return id_; }
	Vector3 GetWorldPosition();
	Vector3 GetVelocity() { return velocity_; }
	Vector3 GetRotation() { return transform_.rotation_; }
	//bool IsAttack() { return attack_.isAttack; }
	int GetHP() { return hp_; }
	//float GetAttackPower() { return attackPower_; }
	//float GetPowerMagnification() { return powerMagnification_; }
	bool IsClear() { return isClear_; }
	bool IsGameOver() { return isGameOver_; }

	Vector3 GetScale() { return Vector3(size_, size_, size_); }
	void SetScale(const Vector3& scale) {
		size_ = (scale.x + scale.y + scale.z) / 3.0f;
		transform_.scale_ = scale;  // **スケールを適用**
	}

	// プレイヤーの腕を取得
	std::array<std::unique_ptr<PlayerArm>, kModelNum>& GetArms() { return arms_; }

	void SetBehavior(Behavior newBehavior);

	static void SetPlayerID(int ID) { playerID_ = ID; }
	void SetVelocity(Vector3 velocity) { velocity_ = velocity; }
	void SetPosition(Vector3 position) {
		transform_.translation_ = position;
		transform_.UpdateMatrix();
	}
	void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }
	void SetTimeManager(TimeManager* timeManager) { timeManager_ = timeManager; }
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetHP(int hp) { hp_ = hp; }
	//void SetAttackPower(float attackPower) { attackPower_ = attackPower; }
	//void SetPowerMagnification(float powerMagnification) { powerMagnification_ = powerMagnification; }
	void SetGameOver(bool gameOver) { isGameOver_ = gameOver; }

};