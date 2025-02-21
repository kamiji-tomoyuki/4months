#pragma once
#include "application/Base/BaseObject.h"

class Skydome : public BaseObject {
public:
	void Init() override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	Vector3 GetCenterPosition() const override;
	Vector3 GetCenterRotation() const override;
	void SetViewProjection(ViewProjection* viewProjection);
	WorldTransform& GetTransform() { return transform_; }
private:
	ViewProjection* viewProjection_ = nullptr; // 新規追加

	WorldTransform transform_;
};
