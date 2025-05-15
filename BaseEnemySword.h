#pragma once
#include "Collider.h"

class Enemy;
class TimeManager;
class BaseEnemySword : public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
	virtual void Initialize(std::string filePath);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(const ViewProjection& viewProjection);

	virtual void DrawAnimation(const ViewProjection& viewProjection);

	/// <summary>
	/// 当たってる間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollision([[maybe_unused]] Collider* other) override = 0;

	/// <summary>
	/// 当たった瞬間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollisionEnter([[maybe_unused]] Collider* other) override = 0;

	/// <summary>
	/// 当たり終わった瞬間
	/// </summary>
	/// <param name="other"></param>
	virtual void OnCollisionOut([[maybe_unused]] Collider* other) override = 0;

	// 中心座標を取得
	virtual Vector3 GetCenterPosition() const override = 0;
	virtual Vector3 GetCenterRotation() const override = 0;


protected:
	Enemy* enemy_ = nullptr;
	TimeManager* timeManager_ = nullptr;

	// モデル配列データ
	std::unique_ptr<Object3d> obj3d_;
	// ベースのワールド変換データ
	WorldTransform transform_;
	WorldTransform transformPalm_;
	//カラー
	ObjColor objColor_;

	bool isAttack_ = false;
	bool isDefense_ = false;

	int id_ = 0;
public:
	int GetID() { return id_; }
	Vector3 GetTranslation() { return transform_.translation_; }
	Vector3 GetRotate() { return transform_.rotation_; }
	bool GetIsAttack() { return isAttack_; }
	bool GetIsDefense() { return isDefense_; }
	Enemy* GetEnemy() { return enemy_; }

	void SetID(int id) { id_ = id; }
	void SetEnemy(Enemy* enemy);
	void SetTimeManager(TimeManager* timeManager) { timeManager_ = timeManager; }
	void SetObjColor(Vector4 c) { objColor_.SetColor(c); }
	void SetTranslation(Vector3 pos) { transform_.translation_ = pos; }
	void SetTranslationY(float pos) { transform_.translation_.y = pos; }
	void SetTranslationX(float pos) { transform_.translation_.x = pos; }
	void SetTranslationZ(float pos) { transform_.translation_.z = pos; }
	void SetRotation(Vector3 rotate) { transform_.rotation_ = rotate; }
	void SetRotationX(float rotate) { transform_.rotation_.x = rotate; }
	void SetRotationY(float rotate) { transform_.rotation_.y = rotate; }
	void SetRotationZ(float rotate) { transform_.rotation_.z = rotate; }
	void SetScale(Vector3 scale) { transform_.scale_ = scale; }
	void SetSize(float size) { Collider::SetRadius(size); }
	void SetIsAttack(bool isAttack) { isAttack_ = isAttack; }
	void SetIsDefense(bool isDefense) { isDefense_ = isDefense; }
};
