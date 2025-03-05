#pragma once
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "memory"

///=====================================================/// 
/// HPバークラス
///=====================================================///
class HPBer {

	///-------------------------------------------/// 
	/// メンバ関数
	///-------------------------------------------///
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	HPBer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="parent">追従元</param>
	/// <param name="color">HPバーの色</param>
	/// <param name="distance">追従元との距離</param>
	void Init(const WorldTransform* parent, Vector3 color, float distance);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="maxNum">最大値</param>
	/// <param name="currentNum">現在地</param>
	void Update(int maxNum, int currentNum);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	///-------------------------------------------/// 
	/// メンバ変数
	///-------------------------------------------///
private:

	//背景スプライト
	std::unique_ptr<Sprite> backGroundSprite_;

	//HPバースプライト
	std::unique_ptr<Sprite> berSprite_;

	//ワールド座標
	WorldTransform transform_;

	//高さ
	float height_;

	//幅
	float width_;

	//距離
	float distance_;
};