#pragma once
#include <numbers>
#include <array>
#include "Object3d.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "BaseObject.h"
#include "PlayerBaseState.h"
#include "PlayerSword.h"
#include "TimeManager.h"
#include "ParticleEmitter.h"

class FollowCamera;
class LockOn;
/// <summary>
/// 自キャラ
/// </summary>
class Player :public BaseObject {
public:
	enum class AttackType {
		kDownSwing,		// 振り下ろし(上入力攻撃)
		kThrust,		// 突き(下入力攻撃)
		kRightSlash,	// 右振り抜き(左入力攻撃)
		kLeftSlash,		// 左振り抜き(右入力攻撃)
		kNullType,		// 未入力
	};
	enum DirectionInput {
		TOP,		// 上
		DOWN,		// 下
		LEFT,		// 左
		RIGHT,		// 右
		Nothing,	// 無入力
	};
	struct Root {
		float floatingParameter = 0.0f;		//浮遊ギミックの媒介変数
		int32_t period = 60;				// 浮遊移動のサイクル<frame>
		float floatingAmplitude = 0.2f;		// 浮遊の振幅<m>
		float armAmplitude = 0.4f;			//アームの振幅
	};
	struct Attack {
		float kLimitTime = 0.4f;					// 攻撃の時間(モーション)
		Vector3 swordStartTransform = { 0.0f };		// 剣の始めの位置
		Vector3 swordEndTransform = { 0.0f };		// 剣の終わりの位置
		float time = 0;								// 現在の進行度(モーション)
		bool isAttack = false;						// 攻撃フラグ
	};
	struct Defence {
		float kLimitTime = 0.5f;			// 防御の時間(モーション)
		float time = 0;						// 現在の進行度(モーション)
		bool isDefence = false;				// 防御フラグ
	};
	struct WorkDash {
		float kDashTime_ = 0.6f;			// ダッシュの時間(モーション)
		float DashTime_ = 0;				// 現在の進行度(モーション)
		float kAttenuation_ = 0.250f;		// 減衰率
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

#ifdef _DEBUG
	// ImGui
	void ImGui();
#endif // _DEBUG

	// 移動
	void Move();

	// 入力方向
	/// <returns>入力方向 0:上 1:下 2:左 3:右 4:無入力</returns>
	int InputDirection();

	// 入力方向の設定
	void SetInputDirection();

	// ステートチェンジ
	void ChangeState(std::unique_ptr<PlayerBaseState> state);
	// ステート関係の入力
	bool IsDashInput();			// ダッシュ
	bool IsPreliminaryInput();	// 予備動作
	bool IsAttackInput();		// 攻撃
	bool IsGuard();				// 防御

	// 浮遊動作(無入力)
	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

public:	// getter
	// 中心座標を取得
	Vector3 GetCenterPosition() const override;
	// 中心回転を取得
	Vector3 GetCenterRotation() const override;
	// ワールド座標を取得
	Vector3 GetWorldPosition();
	// 回転を取得
	Vector3 GetRotation() { return transform_.rotation_; }
	// サイズを取得
	Vector3 GetScale() { return Vector3(size_, size_, size_); }
	// 方向を取得
	Vector2 InputDirectionGampad();
	// 速度を取得
	Vector3 GetVelocity() { return velocity_; }
	// 各動作に必要なデータを取得
	Root GetRootData() { return root_; }
	Attack GetAttackData() { return attack_; }
	Defence GetDefenceData() { return defence_; }
	WorkDash GetWorkDashData() { return workDash_; }

	// タイムマネージャを取得
	TimeManager* GetTimeManager() { return timeManager_; }

	// 狙う方向
	Vector3 GetAttackDirection() { return aimingDirection_; }
	Vector3 GetAimingDirection() { return aimingDirection_; }

	// IDを取得
	int GetID() { return id_; }
	// HPを取得
	int GetHP() { return hp_; }
	// 加速度を取得
	float GetAcceleration() { return kAcceleration_; }
	// 減衰速度を取得
	float GetAttenuation() { return kAttenuation_; }
	// 移動速度を取得
	float GetLimitRunSpeed() { return kLimitRunSpeed_; }
	// クリアフラグを取得
	bool IsClear() { return isClear_; }
	// ゲームオーバーフラグを取得
	bool IsGameOver() { return isGameOver_; }
	
	// プレイヤーの剣を取得
	PlayerSword* GetSword() { return sword_.get(); }

	AttackType GetAttackType() { return attackType_; }

public:	// setter
	// ワールド座標をセット
	void SetTransform(const WorldTransform& transform) { transform_ = transform; }
	// 座標をセット
	void SetPosition(Vector3 position) {
		transform_.translation_ = position;
		transform_.UpdateMatrix();
	}
	// 大きさをセット
	void SetScale(const Vector3& scale) {
		size_ = (scale.x + scale.y + scale.z) / 3.0f;
		transform_.scale_ = scale;  // **スケールを適用**
	}
	// 向きをセット
	void VectorRotation(const Vector3& direction);
	// 速度をセット
	void SetVelocity(Vector3 velocity) { velocity_ = velocity; }
	// 各動作に必要なデータをセット
	void SetRootData(const Root& root) { root_ = root; }
	void SetAttackData(const Attack& attack) { attack_ = attack; }
	void SetDefenceData(const Defence& defence) { defence_ = defence; }
	void SetWorkDashData(const WorkDash& workDash) { workDash_ = workDash; }

	// 狙う方向をセット
	void SetAttackDirection(const Vector3& direction) { attackDirection_ = direction; }
	void SetAimingDirection(const Vector3& direction) { aimingDirection_ = direction; }

	// ロックオンをセット
	void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn; }
	// タイムマネージャをセット
	void SetTimeManager(TimeManager* timeManager) { timeManager_ = timeManager; }
	// フォローカメラをセット
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	// ビュープロジェクションをセット
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	
	// IDをセット
	static void SetPlayerID(int ID) { playerID_ = ID; }
	// HPをセット
	void SetHP(int hp) { hp_ = hp; }
	// 加速度をセット
	void SetAcceleration(float acceleration) { kAcceleration_ = acceleration; }
	// 減衰速度をセット
	void SetAttenuation(float attenuation) { kAttenuation_ = attenuation; }
	// 移動速度をセット
	void SetLimitRunSpeed(float limitRunSpeed) { kLimitRunSpeed_ = limitRunSpeed; }
	// 生存フラグをセット
	void SetIsAlive(bool flag) { isAlive_ = flag; }
	// ゲームオーバーフラグをセット
	void SetGameOver(bool gameOver) { isGameOver_ = gameOver; }

	void SetAttackType(AttackType type) { attackType_ = type; }

private:	// メンバ関数
	// 攻撃方向入力されたか
	bool IsAttackDirectionInput();

	// 死亡
	void Dead();

	// 調整項目の適用
	void ApplyGlobalVariables();

private:	// メンバ変数
	std::unique_ptr<Object3d> model_;

	// ポインタ
	TimeManager* timeManager_ = nullptr;
	FollowCamera* followCamera_ = nullptr;
	LockOn* lockOn_ = nullptr;
	const ViewProjection* viewProjection_ = nullptr;

	// ソード
	std::unique_ptr<PlayerSword> sword_;
	// ステート
	std::unique_ptr<PlayerBaseState> state_;
	// パーティクルエミッタ
	std::vector<std::unique_ptr<ParticleEmitter>> emitters_;

	AttackType attackType_ = AttackType::kNullType;

	// ステータス
	int kHp_ = 10000;
	int hp_ = kHp_;
	// 移動速度 減衰速度
	float kAcceleration_ = 0.1f;
	float kAttenuation_ = 0.1f;
	float kLimitRunSpeed_ = 5.0f;
	// サイズ
	float size_ = 1.0f;
	// 速度
	Vector3 velocity_{};
	Vector3 acceleration_{};
	// 各動作に必要なデータ
	Root root_;
	Attack attack_;
	Defence defence_;
	WorkDash workDash_;

	// 狙う方向
	Vector3 attackDirection_{};
	Vector3 aimingDirection_{};

	// フラグ
	bool isAlive_ = true;		// 生存
	bool isClear_ = false;		// クリア
	bool isGameOver_ = false;	// ゲームオーバー

	Vector3 deleteScale_;

	float deleteTimer_ = 0.0f;

	float deleteSpeed_ = 0.05f;

	// シリアルナンバー
	static inline int playerID_ = 0;
	int id_ = 0;
};