
/*==========================================================================
トレイルの描画 [trail.h]

位置履歴を使ったトレイルの描画と、描画設定を変更する関数を宣言する。
==========================================================================*/

#pragma once
#include <DirectXMath.h>

// トレイル描画用の頂点バッファを初期化する
void Trail_Initialize();

// トレイル描画用のリソースを解放する
void Trail_Finalize();

// 保存された位置履歴を使ってトレイルを描画する
void Trail_Draw();

// 新しい位置を履歴へ追加する
void Trail_AddPosition(const DirectX::XMFLOAT3& position);

// 描画に使用するカメラ位置を設定する
void Trail_SetCameraPosition(const DirectX::XMFLOAT3& position);

// 保存された位置履歴を消去する
void Trail_Clear();

// トレイルを描画するかどうかを設定する
void Trail_SetEnabled(bool enabled);

// 色のグラデーションを有効・無効にする
void Trail_SetFadeEnabled(bool enabled);

// トレイルの開始色と終了色を設定する
void Trail_SetColor(
    const DirectX::XMFLOAT4& startColor,
    const DirectX::XMFLOAT4& endColor
);
