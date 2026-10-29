/*==============================================================================
    スコア情報とスコアボードの関数宣言 [scoreBoard.h]

    投球結果や合計得点を保持する構造体と、
    スコアの記録・表示に使用する関数を宣言する。
==============================================================================*/

#pragma once

#include "debug_text.h"
#include "debug_ostream.h"

// 投球結果の表示種別
enum class ThrowMark
{
    None,   // 記録なし
    Number, // 倒した本数
    Spare,  // スペア
    Strike  // ストライク
};

// 合計得点を管理する構造体
struct Score
{
public:
    int Totalscore;

    // 合計得点を取得する
    int GetTotalScore() { return Totalscore; }

    // 合計得点を設定する
    void SetTotalScore(int a) { Totalscore = a; }
};

// スコア情報の取得・更新
Score* GetScore();
void ScoreSet(int);
void Score_AddThrow(int pins);
void Score_Recalculate();

// スコアボードの初期化と描画
void ScoreBoard_Draw();
void ScoreBoard_Initialize();

// 数字画像内の描画位置や高さを取得する
float GetSymbolSrcY(int symbol);
float GetSymbolHeight(int symbol);

// 指定した数字・記号を描画する
void DrawSymbol(
    int texId,
    int symbol,
    float x,
    float y,
    float scale = 1.0f
);
