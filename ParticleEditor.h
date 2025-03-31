#pragma once

#include "ParticleEmitter.h"

#include "vector"

#include "json.hpp"

///=====================================================/// 
/// パーティクルエディター
///=====================================================///
class ParticleEditor {

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="vp">ビュープロジェクション</param>
	void Initialize(ViewProjection* vp);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGui表示
	/// </summary>
	void ImGui();

	/// <summary>
	/// 新規エミッターの作成
	/// </summary>
	void CreateEmitter();

	/// <summary>
	/// ファイルからエミッターを読み込む
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	void LoadEmitter(const std::string& fileName);

private:

	// エミッター
	std::vector<std::unique_ptr<ParticleEmitter>> emitters_;

	// エミッターファイルリスト
	std::vector<std::string> emitterFiles_;

	// ディレクトリパス
	std::string kDirectoryPath_ = "resources/jsons/particles/";

	// ビュープロジェクション
	ViewProjection* viewProjection_;

	// タイマー
	float timer_;

	// 前回のタイマー
	float preTimer_;

	// 最大時間
	float maxTime_;

	// 開始フラグ
	bool isStart_;

	// ループフラグ
	bool isLoop_;
};