#pragma once
#include <list>
#include <memory>
#include "Enemy.h"
#include "LockOn.h"
#include "ViewProjection.h"
#include <fstream>

class GameScene;
class EnemyManager{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	void DeathUpdate();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& vp);
	void DrawAnimation(const ViewProjection& vp);
	/// <summary>
	/// 敵のポップデータを読み込む
	/// </summary>
	void LoadEnemyPopData();
	void UpdateEnemyPopCommands();
	void AddEnemy(const Vector3& position);
	void AddBoss(const Vector3& position);
private:
	LockOn* lockOn_ = nullptr; // ロックオンシステムへのポインタ
	GameScene* gameScene_ = nullptr; // ゲームシーンへのポインタ
	//Enemy
	std::list<std::unique_ptr<Enemy>> enemies_;
	//敵発生コマンド
	std::stringstream enemyPopCommands;
public:
	const std::list<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

	void SetLockOn(LockOn* lockOn) { lockOn_ = lockOn;}
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
};

