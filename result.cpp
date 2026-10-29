/*==============================================================================
    リザルト画面の管理 [result.cpp]

    ゲーム終了後の背景やスコア、キー入力案内を描画する。
    また、入力待ちやフェード処理を管理し、次のシーンへ移行する。
==============================================================================*/

#include "result.h"
#include "keylogger.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "Audio.h"
#include "direct3d.h"
#include "math.h"
#include <DirectXMath.h>
#include "scene.h"
#include "algorithm"
#include "scoreBoard.h"

static int g_WaitTime = 1.0;

using namespace DirectX;

// リザルト画面で使用するテクスチャID
static int g_ResultTex = -1;
static int g_TexPushAnyKeyId = -1;
static int g_TexID = -1;

// リザルト画面の経過時間と入力後の待機時間
static double g_AccumulatedTime = 0.0;
static double g_keyInputWaitTime = 0.0;

// スコア数字の表示設定
static int g_Digit = 6;
static int g_Dx = 600;
static int g_Dy = 400;

// 数字を描画する関数
void DrawNumber(int x, int y, int number);


// リザルト画面の状態
enum ResultState {
    RESULT_STATE_FADE_IN,       // フェードイン中
    RESULT_STATE_KEYINPUT_WAIT, // キー入力待ち
    RESULT_STATE_KEYINPUT_ACTION,// 入力後の待機
    RESULT_STATE_FADE_OUT,      // フェードアウト中
    RESULT_STATE_END            // 終了
};

// 現在のリザルト画面の状態
static ResultState g_ResultState = RESULT_STATE_FADE_IN;


// リザルト画面を初期化する
void Result_Initialize()
{
    // 背景・入力案内・スコア表示用のテクスチャを読み込む
    g_ResultTex = Texture_Load(L"rom\\dekoi.png");
    g_TexPushAnyKeyId = Texture_Load(L"rom\\push_Enter_key.png");
    g_TexID = Texture_Load(L"rom\\Resultscoa.png");

    // 初期状態を入力待ちにし、経過時間をリセットする
    g_ResultState = RESULT_STATE_KEYINPUT_WAIT;
    g_AccumulatedTime = 0.0;
}


// リザルト画面の状態と入力を更新する
void Result_Update(double elapsed_time)
{
    // 経過時間を加算する
    g_AccumulatedTime += elapsed_time;

    // 現在の状態に応じて処理を切り替える
    switch (g_ResultState)
    {
    case RESULT_STATE_FADE_IN:

        // フェードインが完了したら入力待ちに移行する
        if (Fade_GetState() == FADE_STATE_FADE_IN_FINISHED) {
            g_ResultState = RESULT_STATE_KEYINPUT_WAIT;
        }

        break;

    case RESULT_STATE_KEYINPUT_WAIT:

        // Enterキーが押されたら入力後の待機状態へ移行する
        if (KeyLogger_IsTrigger(KK_ENTER)) {
            g_ResultState = RESULT_STATE_KEYINPUT_ACTION;

            // 入力された時刻を記録する
            g_keyInputWaitTime = g_AccumulatedTime;

            // 必要に応じて決定音などを再生する
        }

        break;

    case RESULT_STATE_KEYINPUT_ACTION:

        // 一定時間経過したらフェードアウトを開始する
        if (g_AccumulatedTime - g_keyInputWaitTime > g_WaitTime) {
            g_ResultState = RESULT_STATE_FADE_OUT;
            Fade_StartIn(1.0, false);
        }

        break;

    case RESULT_STATE_FADE_OUT:

        // フェードアウトが完了したらタイトル画面への切り替えを予約する
        if (Fade_GetState() == FADE_STATE_FADE_OUT_FINISHED) {
            Scene_SetNextScene(SCENE_TITLE);
        }

        break;

    case RESULT_STATE_END:
        // 終了状態では追加処理を行わない
        break;
    }
}


// リザルト画面を描画する
void Result_Draw()
{
    // プレイヤーとスコア情報を取得する
    Score* s = GetScore();

    // 背景を画面全体に描画する
    Sprite_Draw(g_ResultTex, 0, 0, 1600, 900);

    // 合計スコアを取得する
    int temp = s->GetTotalScore();

    // 数字の描画間隔とサイズ
    int spacing = 64;
    int digitW = 32;
    int digitH = 40;

    // スコアの桁数を計算する
    int actualDigits = 1;
    int t = temp;

    while (t >= 10) {
        t /= 10;
        actualDigits++;
    }

    // 画面中央に配置するため、表示開始位置を計算する
    int startX = (Direct3D_GetBackBufferWidth() - actualDigits * spacing) / 2;
    int startY = 500;

    // 1桁ずつ取り出して描画する
    for (int i = 0; i < actualDigits; i++) {
        int n = temp % 10;

        // 下位の桁から取り出すが、描画位置は左から右になるようにする
        int drawX = startX + spacing * (actualDigits - 1 - i);

        // テクスチャから対応する数字を切り出して描画する
        Sprite_Draw(
            g_TexID,
            drawX,
            startY - 300,
            digitW * 3,
            digitH * 3,
            n * digitW,
            0,
            digitW,
            digitH
        );

        // 次の桁へ進む
        temp /= 10;
    }

    // フェードイン中以外は「キーを押してください」の案内を表示する
    if (g_ResultState != RESULT_STATE_FADE_IN) {

        // 入力待ち中はゆっくり、それ以外は速く点滅させる
        float speed =
            g_ResultState == RESULT_STATE_KEYINPUT_WAIT ? 4.0f : 32.0f;

        // sin関数を使い、透明度を0～1の範囲で変化させる
        float alpha = static_cast<float>(
            (sin(g_AccumulatedTime * speed) + 1.0f) * 0.5f
            );

        // 画面中央付近に案内を配置する
        float dx = (Direct3D_GetBackBufferWidth() - 350) * 0.5f;
        float dy = 700;

        // 算出した透明度を使って案内を描画する
        Sprite_Draw(
            g_TexPushAnyKeyId,
            dx,
            dy,
            400,
            100,
            { 1.0f, 1.0f, 1.0f, alpha }
        );
    }
}


// リザルト画面の終了処理
void Result_Finalize()
{
    // 現状では、この関数内で追加の終了処理は行っていない
}


// 指定した数字を描画する
void DrawNumber(int x, int y, int number)
{
    // 数字の画像領域を指定して描画する
    Sprite_Draw(
        g_TexID,
        x,
        y,
        15000.0,
        15000.0,
        number * 500,
        0,
        500,
        500
    );
}

