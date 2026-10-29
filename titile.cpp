/*==========================================================================
タイトルの表示 [title.cpp]

タイトル画面の表示、入力受付、画面遷移を管理する。
==========================================================================*/

#include "titile.h"
#include "keylogger.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "audio.h"
#include "direct3d.h"
#include "math.h"
#include <DirectXMath.h>
#include "scene.h"

static int g_WaitTime = 1.0; // キー入力後、次の処理へ移るまでの待機時間

using namespace DirectX;

static int g_TitleTex = -1; // タイトル画像のテクスチャID
static int g_TexPushAnyKeyId = -1; // キー入力を促す画像のテクスチャID

static double g_AccumulatedTime = 0.0; // タイトル画面開始からの累積時間
static double g_keyInputWaitTime = 0.0; // キー入力が行われた時刻

static int PushCount = 0; // 表示する説明画面を管理するカウンター
static int setumeiTexId[4] = { -1,-1,-1,-1 }; // 説明画面のテクスチャID

// タイトル画面の進行状態
enum TitleState
{
    TITLE_STATE_FADE_IN,         // フェードイン中
    TITLE_STATE_KEYINPUT_WAIT,   // キー入力待ち
    TITLE_STATE_KEYINPUT_ACTION, // キー入力後の待機中
    TITLE_STATE_FADE_OUT,        // フェードアウト中
    TITLE_STATE_END              // タイトル画面の終了
};

static TitleState g_TitleState = TITLE_STATE_FADE_IN; // 現在のタイトル画面の状態

// タイトル画面を初期化する
void Title_Initialize()
{
    // 必要なテクスチャを読み込む
    // Fade_StartIn(100, false); // フェードイン開始処理（現在は無効）

    g_TitleTex = Texture_Load(L"rom\\Texture\\title.png");
    g_TexPushAnyKeyId = Texture_Load(L"rom\\Push_Enter_Key.png");

    // 説明画面のテクスチャを読み込む場合は、ここで設定する
    // setumeiTexId[3] = Texture_Load(L"rom\\Push_Enter_Key.png");

    // カウンターと時間を初期化する
    PushCount = 0;
    g_TitleState = TITLE_STATE_KEYINPUT_WAIT;
    g_AccumulatedTime = 0.0;
}

// タイトル画面の状態と入力を更新する
void Title_Update(double elapsed_time)
{
    // 前回の更新からの経過時間を累積する
    g_AccumulatedTime += elapsed_time;

    // 現在の状態に応じて処理を切り替える
    switch (g_TitleState)
    {
    case TITLE_STATE_FADE_IN:

        // フェードインが完了したらキー入力待ちへ移行する
        if (Fade_GetState() == FADE_STATE_FADE_IN_FINISHED)
        {
            g_TitleState = TITLE_STATE_KEYINPUT_WAIT;
        }

        break;

    case TITLE_STATE_KEYINPUT_WAIT:

        // Enterキーが押されたら、キー入力後の待機状態へ移行する
        if (KeyLogger_IsTrigger(KK_ENTER) && PushCount == 0)
        {
            g_TitleState = TITLE_STATE_KEYINPUT_ACTION;

            // キー入力が行われた時刻を保存する
            g_keyInputWaitTime = g_AccumulatedTime;

            // PushCountを増やす処理やサウンド再生を追加する場合はここで行う
            // PushCount++;
        }

        break;

    case TITLE_STATE_KEYINPUT_ACTION:

        // キー入力後、指定時間が経過したらフェードアウトを開始する
        if (g_AccumulatedTime - g_keyInputWaitTime > g_WaitTime)
        {
            g_TitleState = TITLE_STATE_FADE_OUT;

            // フェード処理を開始する
            Fade_StartIn(1.0, false);
        }

        break;

    case TITLE_STATE_FADE_OUT:

        // フェードアウトが完了したらゲームシーンへ移行する
        if (Fade_GetState() == FADE_STATE_FADE_OUT_FINISHED)
        {
            Scene_SetNextScene(SCENE_GAME);
        }

        break;

    case TITLE_STATE_END:

        // タイトル画面終了後の処理
        break;
    }
}

// タイトル画面を描画する
void Title_Draw()
{
    // 説明画面を表示していない場合はタイトル画面を描画する
    if (PushCount == 0)
    {
        // タイトル画像を画面全体に表示する
        Sprite_Draw(g_TitleTex, 0, 0, 1600, 900);

        // フェードイン中はキー入力案内を表示しない
        if (g_TitleState != TITLE_STATE_FADE_IN)
        {
            // 待機中とキー入力後で点滅速度を切り替える
            float speed = g_TitleState == TITLE_STATE_KEYINPUT_WAIT ? 4.0 : 32.0f;

            // sin関数を使って透明度を0～1の範囲で変化させる
            float alpha = static_cast<float>(
                (sin(g_AccumulatedTime * speed) + 1.0f) * 0.5f
                );

            // 画面中央にキー入力案内を表示するための座標を計算する
            float dx = (Direct3D_GetBackBufferWidth() - 400) * 0.5f;

            // キー入力案内を半透明で描画する
            Sprite_Draw(
                g_TexPushAnyKeyId,
                dx,
                Direct3D_GetBackBufferHeight() * 0.5f,
                400,
                100,
                { 1.0f, 1.0f, 1.0f, alpha }
            );
        }
    }
    // PushCountに応じて説明画面を描画する
    else if (PushCount == 1)
    {
        Sprite_Draw(setumeiTexId[0], 0, 0, 1600, 900);
    }
    else if (PushCount == 2)
    {
        Sprite_Draw(setumeiTexId[1], 0, 0, 1600, 900);
    }
    else if (PushCount == 3)
    {
        Sprite_Draw(setumeiTexId[2], 0, 0, 1600, 900);
    }

    /*
    else if (PushCount == 4)
    {
        Sprite_Draw(setumeiTexId[3], 0, 0, 1600, 900);
    }
    */
}

// タイトル画面の終了処理
void Title_Finalize()
{
    // タイトル画面専用のリソース解放などを行う
}
