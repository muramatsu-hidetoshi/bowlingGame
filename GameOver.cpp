/*==========================================================================
ゲームオーバ画面の実装[gameOver.cpp]





---------------------------------------------------------------------------



==========================================================================*/

#include "gameOver.h"
#include "keylogger.h"
#include "texture.h" 
#include "sprite.h"
#include "fade.h"
#include "audio.h"
#include "direct3d.h"
#include "math.h"
#include <DirectXMath.h>
#include "scene.h"


static int g_WaitTime = 1.0;
using namespace DirectX;
static int g_GameOverTex = -1; // タイトルテクスチャ
static int g_TexPushAnyKeyId = -1; // キーを押してくださいテクスチャ
static double g_AccumulatedTime = 0.0; // 累積時間
static double g_keyInputWaitTime = 0.0; // キー入力待機時間

enum GameOverState {

	GAMEOVER_STATE_FADE_IN, // フェードイン
	GAMEOVER_STATE_KEYINPUT_WAIT,    // 待機
	GAMEOVER_STATE_KEYINPUT_ACTION,  // キー入力アクション
	GAMEOVER_STATE_FADE_OUT, // フェードアウト
	GAMEOVER_STATE_END,  // 終了


};
static GameOverState g_TitleState = GAMEOVER_STATE_FADE_IN; // タイトルの状態

void GameOver_Initialize()
{
    // ゲームオーバー画面の画像や表示状態を初期化する。
	g_GameOverTex = Texture_Load(L"rom\\gameover.png"); // タイトルテクスチャの読み込み
	//g_TexPushAnyKeyId = Texture_Load(L"rom\\push_any_key.png"); // キーを押してくださいテクスチャの読み込み


	g_TitleState = GAMEOVER_STATE_KEYINPUT_WAIT;
	g_AccumulatedTime = 0.0;
}

void GameOver_Update(double elapsed_time)
{
    // 入力と経過時間を確認し、ゲームオーバー画面の状態を更新する。
	g_AccumulatedTime += elapsed_time;
	switch (g_TitleState)
	{


	case GAMEOVER_STATE_FADE_IN:

		if (Fade_GetState() == FADE_STATE_FADE_IN_FINISHED) {
			g_TitleState = GAMEOVER_STATE_KEYINPUT_WAIT;
		}

		break;


	case GAMEOVER_STATE_KEYINPUT_WAIT:

		if (KeyLogger_IsTrigger(KK_ENTER)) {
			g_TitleState = GAMEOVER_STATE_KEYINPUT_ACTION;
			g_keyInputWaitTime = g_AccumulatedTime;
			//サウンド
		}

		break;

	case GAMEOVER_STATE_KEYINPUT_ACTION:

		if (g_AccumulatedTime - g_keyInputWaitTime > g_WaitTime) {
			g_TitleState = GAMEOVER_STATE_FADE_OUT;
			Fade_StartIn(1.0, false);
		}

		break;

	case GAMEOVER_STATE_FADE_OUT:

		if (Fade_GetState() == FADE_STATE_FADE_OUT_FINISHED) {
			//ゲームシーンへ移行
			Scene_SetNextScene(SCENE_TITLE);
		}

		break;

	case GAMEOVER_STATE_END:


		break;



	}
}

void GameOver_Draw()
{
    // ゲームオーバー画面と操作案内を描画する。
	Sprite_Draw(g_GameOverTex, 0, 0, 1600, 900);
}

void GameOver_Finalize()
{
    // 画面で使用したテクスチャなどを解放する。

}
