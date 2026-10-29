/*==========================================================================
スコアの表示[scoreBoard.h]





---------------------------------------------------------------------------



==========================================================================*/

#pragma once
#include "debug_text.h"
#include "debug_ostream.h"
enum class ThrowMark
{
    None,
    Number,
    Spare,
    Strike
};

struct Score
{
public:
    int Totalscore;
    int  GetTotalScore() { return Totalscore; }
    void SetTotalScore(int a) { Totalscore = a; }
};

Score *GetScore();
void ScoreSet(int);
void Score_AddThrow(int pins);
void Score_Recalculate();
void ScoreBoard_Draw();
void ScoreBoard_Initialize();
float GetSymbolSrcY(int symbol);
float GetSymbolHeight(int symbol);
void DrawSymbol(
    int texId,
    int symbol,
    float x,
    float y,
    float scale = 1.0f
);