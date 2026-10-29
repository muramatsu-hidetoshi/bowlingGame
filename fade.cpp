/*==========================================================================
  フェード処理 [fade.cpp]



---------------------------------------------------------------------------






=========================================================================*/


#include "fade.h"
#include "sprite.h"
#include "texture.h"
#include "direct3d.h"

using namespace DirectX;

static FadeState g_fadeState = FADE_STATE_NONE; // フェードの状態
static double g_fadeTime = 0.0; // フェード時間
static double g_AccumulatedTime = 0.0; // 累積時間
static double g_FadeStartTime = 0.0; // フェード開始時間

static XMFLOAT4 g_FadeColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // フェードの色
static int g_TexId ; // フェード用テクスチャ

void Fade_Initialize()
{
	g_fadeState = FADE_STATE_NONE;
	g_AccumulatedTime = 0.0;

	//g_TexId = Texture_Load(L"rom\\white_8x8.png");
	g_TexId = Texture_Load(L"rom\\white_8x8.png");

}

void Fade_Update(double elapsed_time)
{
    // 経過時間に応じて不透明度を進め、フェード状態を更新する。
	//じかんけいそくとステートの管理
	if (g_fadeState == FADE_STATE_NONE ||
		g_fadeState == FADE_STATE_FADE_IN_FINISHED ||
		g_fadeState == FADE_STATE_FADE_OUT_FINISHED) {
		return;
	}

	g_AccumulatedTime += elapsed_time;
	double LifeTime = g_AccumulatedTime - g_FadeStartTime;
	// 透明度の計算
	float alpha = static_cast<float>(LifeTime/g_fadeTime);


	g_FadeColor.w = (g_fadeState == FADE_STATE_FADE_IN) ? (1.0f - alpha) : alpha;


	if (g_fadeTime <= LifeTime) {

		g_fadeState = (g_fadeState == FADE_STATE_FADE_IN) ? FADE_STATE_FADE_IN_FINISHED : FADE_STATE_FADE_OUT_FINISHED;
		return;
	}

}

void Fade_Draw()
{
	if (g_fadeState == FADE_STATE_NONE ||
		g_fadeState == FADE_STATE_FADE_IN_FINISHED ||
		g_fadeState == FADE_STATE_FADE_OUT_FINISHED) {
		return;
	}

	Sprite_Draw(g_TexId, 0.0f, 0.0f,
		(float)Direct3D_GetBackBufferWidth(),
		(float)Direct3D_GetBackBufferHeight(),
		0,0,8,8,
		g_FadeColor);
}

void Fade_Finalize()
{


}

void Fade_StartIn(double fade_time, bool isFadeIn, DirectX::XMFLOAT4 FadeColor)
{
	g_fadeTime = fade_time;
	g_fadeState = isFadeIn ? FADE_STATE_FADE_IN : FADE_STATE_FADE_OUT;
	g_FadeColor = FadeColor;
	g_FadeStartTime = g_AccumulatedTime;
}	

FadeState Fade_GetState()
{
	return g_fadeState;
}
