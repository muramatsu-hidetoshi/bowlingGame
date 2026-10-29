/*==========================================================================

シーン管理[scne.cpp]


---------------------------------------------------------------------------






=========================================================================*/

#include "scene.h"
#include "titile.h"
#include "game.h"
#include "result.h"
#include "gameOver.h"
static Scene g_Scene = SCENE_TITLE;//ここを変更すればすぐにゲームに行ける
static Scene g_SceneNext = g_Scene;

typedef void(*FunctionVoid)(void);
typedef void(*FunctionUpdate)(double);

static FunctionVoid g_SceneInitialize[]{

	Title_Initialize,
    Game_Initialize,
	Result_Initialize,
	GameOver_Initialize

};

static FunctionUpdate g_SceneUpdate[]{

	Title_Update,
	Game_Update,
	Result_Update,
	GameOver_Update
};

static FunctionVoid g_SceneDraw[]{

	Title_Draw,
	Game_Draw,
	Result_Draw,
	GameOver_Draw
};

static FunctionVoid g_SceneFinalize[]{

	Title_Finalize,
	Game_Finalize,
	Result_Finalize,
	GameOver_Finalize
};

void Scene_Initialize()
{

	g_SceneInitialize[g_Scene]();
}

void Scene_Update(double elapsed_time)
{
	g_SceneUpdate[g_Scene](elapsed_time);
}

void Scene_Draw()
{

	g_SceneDraw[g_Scene]();
}

void Scene_Finalize()
{

	g_SceneFinalize[g_Scene]();
}

void Scene_Change()
{
    // 現在のシーンを終了し、予約されている次のシーンを初期化する。
	if (g_Scene != g_SceneNext) {
		//現在のシーンの後かたずけ
		Scene_Finalize();
		g_Scene = g_SceneNext;
		Scene_Initialize();
	}

}

void Scene_SetNextScene(Scene next_scene)
{
	g_SceneNext = next_scene;

}
