#pragma once
#include "BaseObject.h"
#include "string"

class Coliseum : public BaseObject
{
public:

	void Init() override;

	void Init(const std::string& fileName);

	void Update() override;

	void Draw(const ViewProjection& viewProjection) override;

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

	Vector3 GetCenterPosition() const override;

	Vector3 GetCenterRotation() const override;

	void SetViewProjection(ViewProjection* viewProjection);

	WorldTransform& GetTransform() { return transform_; }

private:

	ViewProjection* viewProjection_ = nullptr; // 新規追加

	WorldTransform transform_;
};

