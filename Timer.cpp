/*==========================================================================
  タイマー管理[timer.cpp]




---------------------------------------------------------------------------



==========================================================================*/



#include "timer.h"
#include "sprite.h"
#include "texture.h"
#include "algorithm"
#include "scene.h"
static int g_TimerTexId = -1; // タイマーのテクスチャID
static int g_TimerTexId1 = -1;
static double g_Timer = 0; // タイマー
static int g_Digit = 0; // 桁数
static float g_Dx = 0.0f, g_Dy = 0.0f; // タイマーの表示間隔
static int g_ShowTimer = 0; // 表示タイマー
void TimernumberDraw(float x, float y, int number);
static int g_CountStopValue = 1;
void Timer_Initilize(float x, float y, int digit)
{

	g_TimerTexId = Texture_Load(L"rom\\scoa.png"); // タイマーテクスチャの読み込み
	g_TimerTexId1 = Texture_Load(L"rom\\TimeLimit.png");
	g_Timer = 60;                // タイマーの初期化
	g_Digit = digit;            // 桁数の設定
	g_Dx = x;                   // タイマーの表示間隔Xの設定
	g_Dy = y;                   // タイマーの表示間隔Yの設定
	g_CountStopValue = 1;
	for (int i = 0; i < g_Digit; i++) {
		g_CountStopValue *= 10;
	}
	g_CountStopValue--;
}

void Timer_Update(double elapsed_time)
{
    // 経過時間を加算し、設定された時間への到達状態を更新する。

	g_Timer -= elapsed_time;
	g_ShowTimer = g_Timer;
	if (g_ShowTimer <= 0) {

		g_ShowTimer = 0;

		Scene_SetNextScene(SCENE_RESULT);
	}


}

void Timer_Draw()
{

	int temp = std::min(g_ShowTimer, g_CountStopValue);
	// temp = g_Score;
	//g_Score%10;
	for (int i = 0; i < g_Digit; i++)
	{

		int dd = g_Digit - 1 - i;
		int n = temp % 10;
		TimernumberDraw(g_Dx + 32 * dd, g_Dy, n);
		temp /= 10;
	}
	Sprite_Draw(g_TimerTexId1,0,200,250,128);

}
void TimernumberDraw(float x, float y, int number)
{

	Sprite_Draw(g_TimerTexId, x+250, y-80, 96.0, 96.0, number * 32, 32, 32, 32);

}

void Timer_Finalize()
{



}
