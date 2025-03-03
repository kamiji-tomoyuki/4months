#pragma once
#include "BaseObject.h"

/// <summary>
/// 地面
/// </summary>
class Ground : public BaseObject {
public:
	void Init() override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;

	WorldTransform& GetTransform() { return transform_; }
};