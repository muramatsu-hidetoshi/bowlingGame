
/*==========================================================================
    スコアの管理と表示を行う [game_score.cpp]

    スコアの保持と更新を行い、数字のテクスチャを使用して
    画面上にスコアを描画する。
==========================================================================*/

#include "game_score.h"
#include "sprite.h"
#include "texture.h"
#include "algorithm"

// スコア表示に使用するテクスチャID
static int g_ScoreTexId = -1;
static int g_ScoreTexId1 = -1;

// 現在のスコアと、表示に使用する桁数
static int g_Score = 0;
static int g_Digit = 0;

// スコアの表示位置
static float g_Dx = 0.0f, g_Dy = 0.0f;

// 画面上に表示するスコア
static int g_ShowScore = 0;

// 指定した位置に数字を描画する関数
void numberDraw(float x, float y, int number);

// 表示可能なスコアの最大値
static int g_CountStopValue = 1;

// スコア表示に必要な情報を初期化する
void Score_Initialize(float x, float y, int digit)
{
    // スコア表示に使用するテクスチャを読み込む
    g_ScoreTexId = Texture_Load(L"rom\\scoa.png");
    g_ScoreTexId1 = Texture_Load(L"rom\\score.png");

    // スコアと表示スコアを初期化する
    g_Score = 0;
    g_ShowScore = 0;

    // 表示する桁数と座標を設定する
    g_Digit = digit;
    g_Dx = x;
    g_Dy = y;

    // 指定された桁数から、表示可能な最大値を計算する
    // 例：3桁の場合、999が最大値となる
    g_CountStopValue = 1;

    for (int i = 0; i < g_Digit; i++)
    {
        g_CountStopValue *= 10;
    }

    g_CountStopValue--;
}

// 表示スコアを実際のスコアに近づける
void Score_Update()
{
    // 表示スコアが実際のスコアより小さい場合、1ずつ増加させる
    if (g_ShowScore < g_Score)
    {
        g_ShowScore++;
    }
}

// スコアを数字のテクスチャに分解して描画する
void Score_Draw()
{
    // 表示可能な最大値を超えないようにする
    int temp = std::min(g_ShowScore, g_CountStopValue);

    // 桁数分だけ繰り返し、1桁ずつ数字を描画する
    for (int i = 0; i < g_Digit; i++)
    {
        // 描画する桁の位置を求める
        int dd = g_Digit - 1 - i;

        // 10で割った余りから、右端の数字を取得する
        int n = temp % 10;

        // 数字を描画し、表示位置を桁に応じてずらす
        numberDraw(g_Dx + 32 * dd, g_Dy, n);

        // 次の桁を取得するために10で割る
        temp /= 10;
    }

    // スコア用の背景や装飾を描画する
    Sprite_Draw(g_ScoreTexId1, 0, 350, 250, 128);
}

// スコア処理の終了処理
void Score_Finalize()
{
    // この実装では、終了時に追加で解放するリソースはない
}

// 指定された値をスコアに加算する
void Score_AddScore(int score)
{
    // 実際のスコアと表示スコアに加算する
    g_ShowScore += score;
    g_Score += score;
}

// 指定された数字をテクスチャから切り出して描画する
void numberDraw(float x, float y, int number)
{
    // 数字の番号に応じてテクスチャの切り出し位置を変更する
    // 1桁あたり32×32の領域を使用する
    Sprite_Draw(
        g_ScoreTexId,
        x + 250, y + 300,
        96.0, 96.0,
        number * 32, 0,
        32, 32);
}
