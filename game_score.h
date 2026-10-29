
/*==========================================================================
    スコア管理機能の関数を宣言する [game_score.h]

    スコアの初期化・更新・描画・終了処理と、
    スコア加算処理を外部から呼び出せるようにする。
==========================================================================*/

#ifndef GAME_SCORE_H
#define GAME_SCORE_H

// スコア表示の初期化
// x・y：スコアの表示位置、digit：表示する桁数
void Score_Initialize(float x, float y, int digit);

// 表示スコアを更新する
void Score_Update();

// スコアを画面に描画する
void Score_Draw();

// スコア処理の終了
void Score_Finalize();

// 指定された値をスコアに加算する
void Score_AddScore(int score);

#endif // GAME_SCORE_H
