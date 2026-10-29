/*==========================================================================
    フェード処理の関数と状態を宣言する [fade.h]

    フェードの開始・更新・描画・終了処理を外部から呼び出すための
    関数と、フェードの進行状態を定義する。
==========================================================================*/

#pragma once
#include <DirectXMath.h>

// フェード処理の初期化
void Fade_Initialize();

// 経過時間に応じてフェード状態を更新する
void Fade_Update(double elapsed_time);

// フェード中の画面を描画する
void Fade_Draw();

// フェード処理の終了
void Fade_Finalize();

// フェードインまたはフェードアウトを開始する
// fade_time：フェードにかける時間
// isFadeIn：trueならフェードイン、falseならフェードアウト
// FadeColor：フェードに使用する色
void Fade_StartIn(
    double fade_time,
    bool isFadeIn,
    DirectX::XMFLOAT4 FadeColor = { 0.0f, 0.0f, 0.0f, 1.0f });

// フェードの進行状態
enum FadeState
{
    FADE_STATE_NONE = 0,       // フェードしていない
    FADE_STATE_FADE_OUT,       // フェードアウト中
    FADE_STATE_FADE_OUT_FINISHED, // フェードアウト完了
    FADE_STATE_FADE_IN,        // フェードイン中
    FADE_STATE_FADE_IN_FINISHED,  // フェードイン完了
    FADE_STATE_MAX             // 状態数の上限
};

// 現在のフェード状態を取得する
FadeState Fade_GetState();
