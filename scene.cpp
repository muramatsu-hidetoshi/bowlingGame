
/*==============================================================================
    シーン管理 [scene.cpp]

    現在のシーンに応じて初期化・更新・描画・終了処理を切り替える。
    シーンの切り替えは、Scene_Change()でまとめて行う。
==============================================================================*/

#include "scene.h"
#include "titile.h"
#include "game.h"
#include "result.h"
#include "gameOver.h"

// ゲーム開始時のシーン
// SCENE_GAMEに変更すると、ゲーム画面から開始できる
static Scene g_Scene = SCENE_TITLE;

// 次に切り替えるシーン
static Scene g_SceneNext = g_Scene;

// 各シーンの処理関数を扱うための関数ポインタ型
typedef void (*FunctionVoid)(void);
typedef void (*FunctionUpdate)(double);

// 各シーンの初期化関数
// Scene列挙型の値と配列の添字を対応させている
static FunctionVoid g_SceneInitialize[]{
    Title_Initialize,
    Game_Initialize,
    Result_Initialize,
    GameOver_Initialize
};

// 各シーンの更新関数
static FunctionUpdate g_SceneUpdate[]{
    Title_Update,
    Game_Update,
    Result_Update,
    GameOver_Update
};

// 各シーンの描画関数
static FunctionVoid g_SceneDraw[]{
    Title_Draw,
    Game_Draw,
    Result_Draw,
    GameOver_Draw
};

// 各シーンの終了処理関数
static FunctionVoid g_SceneFinalize[]{
    Title_Finalize,
    Game_Finalize,
    Result_Finalize,
    GameOver_Finalize
};


// 現在のシーンの初期化処理を呼び出す
void Scene_Initialize()
{
    g_SceneInitialize[g_Scene]();
}


// 現在のシーンを更新する
void Scene_Update(double elapsed_time)
{
    // 前フレームからの経過時間をシーン側へ渡す
    g_SceneUpdate[g_Scene](elapsed_time);
}


// 現在のシーンを描画する
void Scene_Draw()
{
    g_SceneDraw[g_Scene]();
}


// 現在のシーンの終了処理を呼び出す
void Scene_Finalize()
{
    g_SceneFinalize[g_Scene]();
}


// 次のシーンへの切り替えを実行する
void Scene_Change()
{
    // シーン変更が要求されている場合だけ処理する
    if (g_Scene != g_SceneNext) {

        // 現在のシーンが使用しているリソースなどを解放する
        Scene_Finalize();

        // 次のシーンを現在のシーンとして設定する
        g_Scene = g_SceneNext;

        // 新しいシーンの初期化処理を実行する
        Scene_Initialize();
    }
}


// 次に切り替えるシーンを設定する
void Scene_SetNextScene(Scene next_scene)
{
    // ここでは予約のみ行い、実際の切り替えはScene_Change()で行う
    g_SceneNext = next_scene;
}


