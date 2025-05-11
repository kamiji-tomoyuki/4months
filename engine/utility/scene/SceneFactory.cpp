#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"
#include "EditorScene.h"
#include "TutorialScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
   // 次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GAME") {
		newScene = new GameScene();
	}
	else if (sceneName == "CLEAR") {
		newScene = new ClearScene();
	}
	else if (sceneName == "GAMEOVER") {
		newScene = new GameOverScene();
	}
	else if (sceneName == "EDITOR") {
		newScene = new EditorScene();
	}
	else if (sceneName == "TUTORIAL") {
		newScene = new TutorialScene();
	}
	return newScene;
}
