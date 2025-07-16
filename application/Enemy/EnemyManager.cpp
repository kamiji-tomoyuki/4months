#include "EnemyManager.h"
#include "CollisionTypeIdDef.h"
#include "GameScene.h"
#include "Boss.h"
#include "Soldier.h"

void EnemyManager::Initialize(){
	//敵
	Enemy::SetEnemyID(0);
}

void EnemyManager::Finalize(){

}

void EnemyManager::Update(){
	//今 敵処理
	UpdateEnemyPopCommands();
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();
		// Boss だけにこの処理を行う
		if (Boss* boss = dynamic_cast<Boss*>(enemy.get())) {
			gameScene_->GetUIManager()->SetBossHP(boss->GetHP()); // ボスのHPをUIマネージャに設定
		}
	}
}

void EnemyManager::DeathUpdate(){
	//死亡処理
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemy->GetCanDelate()) {
			gameScene_->GetLockOn()->ResetTarget();
			if (enemy->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kBoss)) {
				gameScene_->SetClear(true);
			}
		}
	}
	enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy) {
		if (enemy->GetCanDelate()) {
			return true;
		}
		return false;
		});
}

void EnemyManager::Draw(const ViewProjection& vp){
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(vp);
	}
}

void EnemyManager::DrawAnimation(const ViewProjection& vp){
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->DrawAnimation(vp);
	}
}

void EnemyManager::LoadEnemyPopData(){
	std::ifstream file;
	file.open("./resources/enemyPop.csv");
	assert(file.is_open());
	//
	enemyPopCommands << file.rdbuf();
	//
	file.close();
}
void EnemyManager::UpdateEnemyPopCommands() {
	//待機処理
	if (gameScene_->GetTimeManager()->GetTimer("enemyPop").isStart) {
		return;
	}
	//
	std::string line;
	while (getline(enemyPopCommands, line)) {
		std::stringstream line_stream(line);

		std::string word;
		//
		getline(line_stream, word, ',');
		if (word.find("//") == 0) {
			continue;
		}
		//POP
		if (word.find("POP") == 0) {
			//x
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			//y
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			//z
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());
			//
			AddEnemy(Vector3(x, y, z));
		}
		//WAIT
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');
			//
			int32_t waitTime = atoi(word.c_str());
			//待機開始
			gameScene_->GetTimeManager()->SetTimer("enemyPop", (float)waitTime);
			//
			break;
		}
	}

}

void EnemyManager::AddEnemy(const Vector3& position) {
	if (0 == Enemy::GetNextSerialNumber()) {
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();
		newEnemy->SetPlayer(gameScene_->GetPlayer());
		newEnemy->SetTimeManager(gameScene_->GetTimeManager());
		newEnemy->Init();
		newEnemy->SetTranslation({ 0,0,0 });
		enemies_.push_back(std::move(newEnemy));
	}
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {

		if (enemy->GetSerialNumber() == enemy->GetNextSerialNumber() - 1) {
			enemy->SetTranslation(position);
			enemy->Update();
		}
	}
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Soldier>();
	newEnemy->SetPlayer(gameScene_->GetPlayer());
	newEnemy->SetTimeManager(gameScene_->GetTimeManager());
	newEnemy->Init();
	newEnemy->SetTranslation({ 0,0,0 });
	enemies_.push_back(std::move(newEnemy));
}

void EnemyManager::AddBoss(const Vector3& position){
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Boss>();
	newEnemy->SetPlayer(gameScene_->GetPlayer());
	newEnemy->SetTimeManager(gameScene_->GetTimeManager());
	newEnemy->Init();
	newEnemy->SetTranslation(position);
	enemies_.push_back(std::move(newEnemy));
	std::unique_ptr<Enemy> newEnemy2 = std::make_unique<Soldier>();
	newEnemy2->SetPlayer(gameScene_->GetPlayer());
	newEnemy2->SetTimeManager(gameScene_->GetTimeManager());
	newEnemy2->Init();
	newEnemy2->SetTranslation({ 0,0,0 });
	enemies_.push_back(std::move(newEnemy2));
}
