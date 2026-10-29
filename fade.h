


/*==========================================================================

フェード処理[fade.h]


---------------------------------------------------------------------------






=========================================================================*/
#pragma once
#include <DirectXMath.h>
void Fade_Initialize();
void Fade_Update(double elapsed_time);
void Fade_Draw();
void Fade_Finalize();

void Fade_StartIn(double fade_time, bool isFadeIn, DirectX::XMFLOAT4 FadeColor = {0.0f,0.0f,0.0f,1.0f}); // フェードインを開始
enum FadeState {
	FADE_STATE_NONE = 0, // フェードなし
	FADE_STATE_FADE_OUT,  // フェードイン中
	FADE_STATE_FADE_OUT_FINISHED, // フェードアウト中
	FADE_STATE_FADE_IN,  // フェードイン中
	FADE_STATE_FADE_IN_FINISHED, // フェードアウト中
	FADE_STATE_MAX
};
FadeState Fade_GetState(); // フェードの状態を取得