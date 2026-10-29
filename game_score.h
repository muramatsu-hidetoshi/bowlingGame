/*==========================================================================

スコアヘッダー[game_score.h]


---------------------------------------------------------------------------






=========================================================================*/





#ifndef GAME_SCORE_H
#define GAME_SCORE_H

void Score_Initialize(float x, float y, int digit);
void Score_Update();
void Score_Draw();
void Score_Finalize();

void Score_AddScore(int score);



#endif // !GAME_SCORE_H
