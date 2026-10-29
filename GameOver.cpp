
/*==========================================================================
ゲームオーバー画面の実装 [gameOver.cpp]

ゲームオーバー画面を表示し、Enterキーの入力を受け付けて
フェード処理を行った後、タイトル画面へ遷移する。
==========================================================================*/

#include "gameOver.h"
#include "keylogger.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "audio.h"
#include "direct3d.h"
#include "math.h"
#include <DirectXMath.h>
#include "scene.h"

using namespace DirectX;

// キー入力後、画面遷移を開始するまでの待機時間
static int g_WaitTime = 1.0;

// ゲームオーバー画面のテクスチャID
static int g_GameOverTex = -1;

// キー入力案内用のテクスチャID
static int g_TexPushAnyKeyId = -1;

// ゲーム開始からの累積経過時間
static double g_AccumulatedTime = 0.0;

// キー入力を受け付けた時点の経過時間
static double g_keyInputWaitTime = 0.0;

// ゲームオーバー画面の状態
enum GameOverState
{
    GAMEOVER_STATE_FADE_IN,        // フェードイン中
    GAMEOVER_STATE_KEYINPUT_WAIT,  // キー入力待ち
    GAMEOVER_STATE_KEYINPUT_ACTION,// 入力後の待機
    GAMEOVER_STATE_FADE_OUT,       // フェードアウト中
    GAMEOVER_STATE_END             // 終了
};

// 現在のゲームオーバー画面の状態
static GameOverState g_TitleState = GAMEOVER_STATE_FADE_IN;

// 初期化処理
void GameOver_Initialize()
{
    // ゲームオーバー画面のテクスチャを読み込む
    g_GameOverTex = Texture_Load(L"rom\\gameover.png");

    // キー入力案内用テクスチャを読み込む場合に使用する
    // g_TexPushAnyKeyId = Texture_Load(L"rom\\push_any_key.png");

    // 画面の状態と経過時間を初期化する
    g_TitleState = GAMEOVER_STATE_KEYINPUT_WAIT;
    g_AccumulatedTime = 0.0;
}

// 更新処理
void GameOver_Update(double elapsed_time)
{
    // 前回の更新からの経過時間を累積する
    g_AccumulatedTime += elapsed_time;

    // 現在の状態に応じて処理を切り替える
    switch (g_TitleState)
    {
    case GAMEOVER_STATE_FADE_IN:

        // フェードインが完了したらキー入力待ちに移行する
        if (Fade_GetState() == FADE_STATE_FADE_IN_FINISHED)
        {
            g_TitleState = GAMEOVER_STATE_KEYINPUT_WAIT;
        }

        break;

    case GAMEOVER_STATE_KEYINPUT_WAIT:

        // Enterキーが押されたら入力後の待機状態に移行する
        if (KeyLogger_IsTrigger(KK_ENTER))
        {
            g_TitleState = GAMEOVER_STATE_KEYINPUT_ACTION;

            // 入力を受け付けた時刻を記録する
            g_keyInputWaitTime = g_AccumulatedTime;

            // 必要に応じて決定音などを再生する
        }

        break;

    case GAMEOVER_STATE_KEYINPUT_ACTION:

        // 一定時間経過したらフェードアウトを開始する
        if (g_AccumulatedTime - g_keyInputWaitTime > g_WaitTime)
        {
            g_TitleState = GAMEOVER_STATE_FADE_OUT;

            // フェードアウトを1秒かけて開始する
            Fade_StartIn(1.0, false);
        }

        break;

    case GAMEOVER_STATE_FADE_OUT:

        // フェードアウトが完了したらタイトル画面へ遷移する
        if (Fade_GetState() == FADE_STATE_FADE_OUT_FINISHED)
        {
            Scene_SetNextScene(SCENE_TITLE);
        }

        break;

    case GAMEOVER_STATE_END:

        // 終了状態では追加の処理を行わない
        break;
    }
}

// 描画処理
void GameOver_Draw()
{
    // ゲームオーバー画面の画像を画面全体に表示する
    Sprite_Draw(g_GameOverTex, 0, 0, 1600, 900);
}

// 終了処理
void GameOver_Finalize()
{
    // 現在は追加の終了処理を行わない
}
