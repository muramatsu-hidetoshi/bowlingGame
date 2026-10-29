/*==============================================================================
    シーン管理の関数宣言 [scene.h]

    ゲーム内のシーンを表す列挙型と、
    シーンの初期化・更新・描画・終了・切り替えに使用する関数を宣言する。
==============================================================================*/

#pragma once

// シーン管理の基本処理
void Scene_Initialize();
void Scene_Update(double elapsed_time);
void Scene_Draw();
void Scene_Finalize();

// ゲーム内のシーン種別
enum Scene {
    SCENE_TITLE,    // タイトル画面
    SCENE_GAME,     // ゲーム画面
    SCENE_RESULT,   // リザルト画面
    SCENE_GAMEOVER  // ゲームオーバー画面
};

// シーンの切り替え処理
void Scene_Change();

// 次に表示するシーンを設定する
void Scene_SetNextScene(Scene next_scene);
