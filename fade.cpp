
/*==========================================================================
    画面のフェード処理を管理する [fade.cpp]

    フェードの状態と経過時間に応じて透明度を変化させ、
    画面全体にフェード用のテクスチャを重ねて描画する。
==========================================================================*/

#include "fade.h"
#include "sprite.h"
#include "texture.h"
#include "direct3d.h"

using namespace DirectX;

// 現在のフェード状態
static FadeState g_fadeState = FADE_STATE_NONE;

// フェードにかける時間と、処理開始からの累積時間
static double g_fadeTime = 0.0;
static double g_AccumulatedTime = 0.0;

// フェードを開始した時点の累積時間
static double g_FadeStartTime = 0.0;

// フェードに使用する色。w成分が透明度（アルファ値）を表す
static XMFLOAT4 g_FadeColor = { 0.0f, 0.0f, 0.0f, 0.0f };

// フェード用テクスチャのID
static int g_TexId;

// フェード処理の初期化
void Fade_Initialize()
{
    // 状態と累積時間を初期化する
    g_fadeState = FADE_STATE_NONE;
    g_AccumulatedTime = 0.0;

    // 白いテクスチャを読み込む
    // 描画時に色と透明度を指定することで、画面全体を覆う
    g_TexId = Texture_Load(L"rom\\white_8x8.png");
}

// 経過時間に応じてフェードの透明度と状態を更新する
void Fade_Update(double elapsed_time)
{
    // フェード処理が不要な状態なら更新しない
    if (g_fadeState == FADE_STATE_NONE ||
        g_fadeState == FADE_STATE_FADE_IN_FINISHED ||
        g_fadeState == FADE_STATE_FADE_OUT_FINISHED)
    {
        return;
    }

    // 累積時間を更新し、フェード開始からの経過時間を求める
    g_AccumulatedTime += elapsed_time;
    double LifeTime = g_AccumulatedTime - g_FadeStartTime;

    // 経過時間をフェード時間で割り、進行度を0～1で求める
    float alpha = static_cast<float>(LifeTime / g_fadeTime);

    // フェードインでは透明から不透明へ、フェードアウトでは不透明へ近づける
    g_FadeColor.w =
        (g_fadeState == FADE_STATE_FADE_IN) ? (1.0f - alpha) : alpha;

    // 設定時間に到達したら、対応する終了状態へ切り替える
    if (g_fadeTime <= LifeTime)
    {
        g_fadeState =
            (g_fadeState == FADE_STATE_FADE_IN)
            ? FADE_STATE_FADE_IN_FINISHED
            : FADE_STATE_FADE_OUT_FINISHED;

        return;
    }
}

// フェードが進行中の場合、画面全体にフェード用スプライトを描画する
void Fade_Draw()
{
    // フェード中でなければ描画しない
    if (g_fadeState == FADE_STATE_NONE ||
        g_fadeState == FADE_STATE_FADE_IN_FINISHED ||
        g_fadeState == FADE_STATE_FADE_OUT_FINISHED)
    {
        return;
    }

    // バックバッファ全体を覆うサイズで描画する
    Sprite_Draw(
        g_TexId,
        0.0f, 0.0f,
        (float)Direct3D_GetBackBufferWidth(),
        (float)Direct3D_GetBackBufferHeight(),
        0, 0, 8, 8,
        g_FadeColor);
}

// フェード処理の終了処理
void Fade_Finalize()
{
    // この実装では、終了時に追加で解放するリソースはない
}

// 指定した時間・方向・色でフェードを開始する
void Fade_StartIn(double fade_time, bool isFadeIn, DirectX::XMFLOAT4 FadeColor)
{
    // フェードにかける時間を設定する
    g_fadeTime = fade_time;

    // 引数に応じてフェードインまたはフェードアウトを開始する
    g_fadeState = isFadeIn ? FADE_STATE_FADE_IN : FADE_STATE_FADE_OUT;

    // フェードに使用する色を設定する
    g_FadeColor = FadeColor;

    // 開始時点の累積時間を保存する
    g_FadeStartTime = g_AccumulatedTime;
}

// 現在のフェード状態を取得する
FadeState Fade_GetState()
{
    return g_fadeState;
}





