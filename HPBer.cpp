#include "HPBer.h"
#include "myMath.h"

HPBer::HPBer() {
}

void HPBer::Init(const WorldTransform* parent, Vector3 color, float distance) {

	//ワールド座標初期化
	transform_.Initialize();

	//親設定
	transform_.parent_ = parent;

	//高さ
	height_ = 10.0f;

	//幅
	width_ = 100.0f;

	//距離
	distance_ = distance;

	/// === HPバーの初期化 === ///

	//スプライト作成
	berSprite_ = std::make_unique<Sprite>();
	//初期化
	berSprite_->Initialize("ber.png", Vector2(0.0f, 0.0f));
	//サイズ設定
	berSprite_->SetSize(Vector2(width_, height_));
	//アンカーポイント設定
	berSprite_->SetAnchorPoint({ 0.0f,0.5f });
	//色設定
	berSprite_->SetColor(color);

	/// === 背景の初期化 === ///

	//スプライト作成
	backGroundSprite_ = std::make_unique<Sprite>();
	//初期化
	backGroundSprite_->Initialize("berBackGround.png", Vector2(0.0f, 0.0f));
	//サイズ設定
	backGroundSprite_->SetSize(Vector2(width_, height_));
	//アンカーポイント設定
	backGroundSprite_->SetAnchorPoint({ 0.5f,0.5f });

}

void HPBer::Update(int maxNum, int currentNum) {

	//ワールド座標の更新
	transform_.UpdateMatrix();

	//HP値の割合
	float hpRatio = static_cast<float>(currentNum) / maxNum;

	//HPバーの幅
	float newWidth = width_ * hpRatio;

	//HPバーのサイズ更新
	berSprite_->SetSize(Vector2(newWidth, height_));
}

void HPBer::Draw(const ViewProjection& viewProjection) {

	//ワールド座標取得
	Vector3 worldPos = Vector3(transform_.matWorld_.m[3][0], transform_.matWorld_.m[3][1], transform_.matWorld_.m[3][2]);

	//ビューポート行列作成
	Matrix4x4 viewPort = MakeViewPortMatrix(0, 0, WinApp::kClientWidth, WinApp::kClientHeight, 0, 1);

	//ビュープロジェクションビューポート行列作成
	Matrix4x4 viewProjectionViewPortMatrix = viewProjection.matView_ * viewProjection.matProjection_ * viewPort;

	//スクリーン座標に変換
	worldPos = Transformation(worldPos, viewProjectionViewPortMatrix);

	//位置設定
	backGroundSprite_->SetPosition(Vector2(worldPos.x, worldPos.y - distance_));
	berSprite_->SetPosition(Vector2(worldPos.x - (width_ / 2.0f), worldPos.y - distance_));

	//描画
	backGroundSprite_->Draw();
	berSprite_->Draw();
}