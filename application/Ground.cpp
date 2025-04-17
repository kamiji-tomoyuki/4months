#include "Ground.h"
#include <cassert>
#include "myMath.h"

void Ground::Init(){
	BaseObject::Init();
	CreateModel("debug/ground.obj");
	transform_.scale_ = { 100.0f,100.0f,100.0f };
	transform_.UpdateMatrix();
}

void Ground::Init(const std::string& fileName) {

	const std::string kDirectoryPath = "map/"; // ディレクトリパス

	std::string filePath = kDirectoryPath + fileName; // ファイルパス

	BaseObject::Init();

	CreateModel(filePath);

	transform_.scale_ = { 100.0f,100.0f,100.0f };

	transform_.UpdateMatrix();
}

void Ground::Update() {}

void Ground::Draw(const ViewProjection& viewProjection){
	BaseObject::Draw(viewProjection);
}

Vector3 Ground::GetCenterPosition() const{
	return Vector3();
}

Vector3 Ground::GetCenterRotation() const{
	return transform_.rotation_;
}
