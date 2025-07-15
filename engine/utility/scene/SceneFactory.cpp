#include "SceneFactory.h"
#include "TitleScene.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"
#include "TutorialScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
   // 次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "TUTORIAL") {
		newScene = new TutorialScene();
	}
	else if (sceneName == "GAME") {
		newScene = new GameScene();
		GameScene* gameScene = static_cast<GameScene*>(newScene);
		gameScene->SetStageNum(1); // ステージ番号を設定
	} else if (sceneName == "STAGE1") {
		newScene = new GameScene(); 
		GameScene* gameScene = static_cast<GameScene*>(newScene);
		gameScene->SetStageNum(1); // ステージ番号を設定
	} else if (sceneName == "STAGE2") {
		newScene = new GameScene(); 
		GameScene* gameScene = static_cast<GameScene*>(newScene);
		gameScene->SetStageNum(2); // ステージ番号を設定
	} else if (sceneName == "STAGE3") {
		newScene = new GameScene(); 
		GameScene* gameScene = static_cast<GameScene*>(newScene);
		gameScene->SetStageNum(3); // ステージ番号を設定
	} else if (sceneName == "STAGE4") {
		newScene = new GameScene(); 
		GameScene* gameScene = static_cast<GameScene*>(newScene);
		gameScene->SetStageNum(4); // ステージ番号を設定
	}
	else if (sceneName == "CLEAR") {
		newScene = new ClearScene();
	}
	else if (sceneName == "GAMEOVER") {
		newScene = new GameOverScene();
	}
	else if (sceneName == "TUTORIAL") {
		newScene = new TutorialScene();
	}
	return newScene;
}
