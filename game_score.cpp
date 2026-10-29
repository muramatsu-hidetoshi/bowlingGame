/*==========================================================================

スコアの管理と表示[game_score.cpp]


---------------------------------------------------------------------------


=========================================================================*/



#include "game_score.h"
#include "sprite.h"
#include "texture.h"
#include "algorithm"


static int g_ScoreTexId = -1; // スコアのテクスチャID
static int g_ScoreTexId1 = -1;
static int g_Score = 0; // スコア
static int g_Digit = 0; // 桁数
static float g_Dx = 0.0f, g_Dy = 0.0f; // スコアの表示間隔
static int g_ShowScore = 0; // 表示スコア
void numberDraw(float x, float y, int number);
static int g_CountStopValue = 1;
void Score_Initialize(float x, float y, int digit)
{


	g_ScoreTexId = Texture_Load(L"rom\\scoa.png"); // スコアテクスチャの読み込み
	g_ScoreTexId1 = Texture_Load(L"rom\\score.png");
	g_Score = 0; // スコアの初期化
	g_Digit = digit; // 桁数の設定
	g_Dx = x; // スコアの表示間隔Xの設定
	g_Dy = y; // スコアの表示間隔Yの設定
	g_CountStopValue = 1;
	g_ShowScore = 0;
	for (int i = 0; i < g_Digit; i++) {
		g_CountStopValue *= 10;
	}
	g_CountStopValue --;

}

void Score_Update()
{
	if (g_ShowScore < g_Score) {
		g_ShowScore++;
	}


}

void Score_Draw()
{
	int temp = std::min(g_ShowScore, g_CountStopValue);
	// temp = g_Score;
	//g_Score%10;
	for (int i = 0; i < g_Digit; i++)
	{

		int dd = g_Digit - 1 - i;
		int n = temp % 10;
		numberDraw( g_Dx + 32 * dd,g_Dy,n);
		temp /= 10;
	}
	Sprite_Draw(g_ScoreTexId1, 0, 350, 250, 128);


}

void Score_Finalize()
{
}

void Score_AddScore(int score)
{
	g_ShowScore += score;
	g_Score += score;
}

void numberDraw(float x, float y, int number)
{

	Sprite_Draw(g_ScoreTexId, x+250, y+300, 96.0, 96.0, number * 32, 0, 32 , 32);
}
