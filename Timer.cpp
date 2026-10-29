
/*==========================================================================
タイマー管理 [timer.cpp]

残り時間の更新、数字の描画、制限時間到達時のシーン遷移を管理する。
==========================================================================*/

#include "timer.h"
#include "sprite.h"
#include "texture.h"
#include "algorithm"
#include "scene.h"

static int g_TimerTexId = -1; // 数字表示用テクスチャのID
static int g_TimerTexId1 = -1; // タイマーの背景・ラベル用テクスチャのID

static double g_Timer = 0; // 残り時間
static int g_Digit = 0; // タイマーに表示する桁数

static float g_Dx = 0.0f, g_Dy = 0.0f; // タイマーの表示位置
static int g_ShowTimer = 0; // 画面に表示する整数の残り時間

// 指定された数字を描画する関数
void TimernumberDraw(float x, float y, int number);

static int g_CountStopValue = 1; // 表示できる最大値

// タイマーを初期化する
void Timer_Initilize(float x, float y, int digit)
{
    // 数字表示用とタイマー表示用のテクスチャを読み込む
    g_TimerTexId = Texture_Load(L"rom\\scoa.png");
    g_TimerTexId1 = Texture_Load(L"rom\\TimeLimit.png");

    // 制限時間を60秒に設定する
    g_Timer = 60;

    // 表示する桁数と座標を保存する
    g_Digit = digit;
    g_Dx = x;
    g_Dy = y;

    // 桁数に応じた最大表示値を計算する
    // 例：digitが3の場合、10の3乗から1を引いた999になる
    g_CountStopValue = 1;

    for (int i = 0; i < g_Digit; i++)
    {
        g_CountStopValue *= 10;
    }

    g_CountStopValue--;
}

// タイマーを更新する
void Timer_Update(double elapsed_time)
{
    // 経過時間を残り時間から減算する
    g_Timer -= elapsed_time;

    // 小数部分を切り捨て、表示用の整数に変換する
    g_ShowTimer = g_Timer;

    // 残り時間が0以下になった場合
    if (g_ShowTimer <= 0)
    {
        // 表示上の残り時間がマイナスにならないようにする
        g_ShowTimer = 0;

        // 結果画面へのシーン遷移を予約する
        Scene_SetNextScene(SCENE_RESULT);
    }
}

// タイマーを描画する
void Timer_Draw()
{
    // 表示できる最大値を超えないようにする
    int temp = std::min(g_ShowTimer, g_CountStopValue);

    // 桁数分だけ数字を取り出して描画する
    for (int i = 0; i < g_Digit; i++)
    {
        // 右端から順番に数字を取り出す
        int dd = g_Digit - 1 - i;
        int n = temp % 10;

        // 各桁の表示位置を計算して数字を描画する
        TimernumberDraw(g_Dx + 32 * dd, g_Dy, n);

        // 次の桁を取り出すために10で割る
        temp /= 10;
    }

    // タイマーの背景・ラベルを描画する
    Sprite_Draw(g_TimerTexId1, 0, 200, 250, 128);
}

// 数字テクスチャから指定された数字を切り出して描画する
void TimernumberDraw(float x, float y, int number)
{
    // 数字の画像が横方向に並んでいることを利用し、
    // numberに応じた領域を指定して描画する
    Sprite_Draw(
        g_TimerTexId,
        x + 250,
        y - 80,
        96.0,
        96.0,
        number * 32,
        32,
        32,
        32
    );
}

// タイマーの終了処理
void Timer_Finalize()
{
    // タイマー専用のリソース解放などを行う
}
