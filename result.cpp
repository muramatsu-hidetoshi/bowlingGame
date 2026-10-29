
/*==========================================================================

リザルト画面[result.cpp]


---------------------------------------------------------------------------






=========================================================================*/
#include "result.h"
#include "keylogger.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "Audio.h"
#include "direct3d.h"
#include "math.h"
#include <DirectXMath.h>
#include "scene.h"
#include "algorithm"
#include "player.h"
/*==========================================================================
リザルト画面の描画や初期化など[result.cpp]





---------------------------------------------------------------------------



==========================================================================*/


#include "scoreBoard.h"


static int g_WaitTime = 1.0;
using namespace DirectX;
static int g_ResultTex = -1; // タイトルテクスチャ
static int g_TexPushAnyKeyId = -1; // キーを押してくださいテクスチャ
static double g_AccumulatedTime = 0.0; // 累積時間
static double g_keyInputWaitTime = 0.0; // キー入力待機時間
static int g_TexID = -1;
static int g_Digit = 6; // 表示桁数（例: 6桁まで表示）
static int g_Dx = 600;  // スコアのX座標（左下基準）
static int g_Dy = 400;  // スコアのY座標
static int g_DigitWidth = 32;  // score.png の 1 桁の幅
static int g_DigitHeight = 40; // score.png の 1 桁の高さ
void DrawNumber(int x, int y, int number);


enum ResultState {

	RESULT_STATE_FADE_IN, // フェードイン
	RESULT_STATE_KEYINPUT_WAIT,    // 待機
	RESULT_STATE_KEYINPUT_ACTION,  // キー入力アクション
	RESULT_STATE_FADE_OUT, // フェードアウト
	RESULT_STATE_END,  // 終了


};
static ResultState g_ResultState = RESULT_STATE_FADE_IN; // タイトルの状態




void Result_Initialize()
{
    // スコア表示や画面遷移に必要なリザルト画面の状態を初期化する。
	g_ResultTex = Texture_Load(L"rom\\dekoi.png"); // タイトルテクスチャの読み込み
	g_TexPushAnyKeyId = Texture_Load(L"rom\\push_Enter_key.png"); // キーを押してくださいテクスチャの読み込み
	g_TexID = Texture_Load(L"rom\\Resultscoa.png");

	g_ResultState = RESULT_STATE_KEYINPUT_WAIT;
	g_AccumulatedTime = 0.0;


}

void Result_Update(double elapsed_time)
{
    // リザルト画面の入力と表示状態を更新する。

	g_AccumulatedTime += elapsed_time;
	switch (g_ResultState)
	{


	case RESULT_STATE_FADE_IN:

		if (Fade_GetState() == FADE_STATE_FADE_IN_FINISHED) {
			g_ResultState = RESULT_STATE_KEYINPUT_WAIT;
		}

		break;


	case RESULT_STATE_KEYINPUT_WAIT:

		if (KeyLogger_IsTrigger(KK_ENTER)) {
			g_ResultState = RESULT_STATE_KEYINPUT_ACTION;
			g_keyInputWaitTime = g_AccumulatedTime;
			//サウンド
		}

		break;

	case RESULT_STATE_KEYINPUT_ACTION:

		if (g_AccumulatedTime - g_keyInputWaitTime > g_WaitTime) {
			g_ResultState = RESULT_STATE_FADE_OUT;
			Fade_StartIn(1.0, false);
		}

		break;

	case RESULT_STATE_FADE_OUT:

		if (Fade_GetState() == FADE_STATE_FADE_OUT_FINISHED) {
			//ゲームシーンへ移行
			Scene_SetNextScene(SCENE_TITLE);
		}

		break;

	case RESULT_STATE_END:


		break;



	}



}

void Result_Draw()
{
    // 確定したゲーム結果と画面操作案内を描画する。
	Player* p = GetPlayer();
	Score* s = GetScore();



	// --- 背景 ---
	Sprite_Draw(g_ResultTex, 0, 0, 1600, 900);

	// --- スコア表示 ---
	int temp = s->GetTotalScore();
	int spacing = 64;   // 数字の横間隔
	int digitW = 32;    // 数字の幅
	int digitH = 40;    // 数字の高さ

	// 桁数を計算
	int actualDigits = 1;
	int t = temp;
	while (t >= 10) {
		t /= 10;
		actualDigits++;
	}

	// スコアの開始位置（中央揃え）
	int startX = (Direct3D_GetBackBufferWidth() - actualDigits * spacing) / 2;
	int startY = 500; // 背景の下の方に固定表示

	for (int i = 0; i < actualDigits; i++) {
		int n = temp % 10;
		int drawX = startX + spacing * (actualDigits - 1 - i);
		// 数字を描画（score.png を 0～9 に分割して使う）
		Sprite_Draw(g_TexID, drawX, startY-300, digitW*3, digitH*3, n * digitW, 0, digitW, digitH);
		temp /= 10;
	}

	// --- "PUSH ANY KEY" 点滅 ---
	if (g_ResultState != RESULT_STATE_FADE_IN) {
		float speed = g_ResultState == RESULT_STATE_KEYINPUT_WAIT ? 4.0f : 32.0f;
		float alpha = static_cast<float>((sin(g_AccumulatedTime * speed) + 1.0f) * 0.5f);

		float dx = (Direct3D_GetBackBufferWidth() - 350) * 0.5f;
		float dy = 700; // スコアより下に配置
		Sprite_Draw(g_TexPushAnyKeyId, dx, dy, 400, 100, { 1.0f, 1.0f, 1.0f, alpha });
	}
}


void Result_Finalize()
{
    // リザルト画面が使用したリソースを解放する。
}
void DrawNumber(int x, int y, int number)
{
	Sprite_Draw(g_TexID, x, y, 15000.0, 15000.0, number * 500, 0, 500, 500);

}