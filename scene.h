/*==========================================================================
シーン管理[scene.h]]



---------------------------------------------------------------------------






=========================================================================*/


#pragma once


void Scene_Initialize();
void Scene_Update(double elapsed_time);
void Scene_Draw();
void Scene_Finalize();

enum Scene {
	SCENE_TITLE, // タイトル
	SCENE_GAME,  // ゲーム
	SCENE_RESULT, // リザルト
	SCENE_GAMEOVER
};
void Scene_Change(); // シーン変更
void Scene_SetNextScene(Scene next_scene); // 次のシーン設定