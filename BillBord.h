
/*==========================================================================
ビルボード機能の宣言 [billBord.h]

概要：
    テクスチャを貼り付けた板ポリゴンを、カメラの向きに合わせて
    描画するための関数を宣言する。

主な機能：
    ・ビルボード用リソースの初期化と解放
    ・カメラのビュー行列の設定
    ・テクスチャ、UV範囲、サイズ、色を指定した描画

==========================================================================*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


// ビルボード描画に必要なリソースを初期化する。
void Billboard_Initialize();

// ビルボード描画で使用したリソースを解放する。
void Billboard_Finalize();

// ビルボードの更新処理。
void Billboard_Update(double elapsedTime);

// カメラのビュー行列を設定する。
void Billboard_SetViewMatrix(const DirectX::XMMATRIX& mtxWorld);


//==========================================================================
// ビルボード描画
//==========================================================================

// テクスチャ全体を使用してビルボードを描画する。
// scale：描画サイズ
// pivot：回転・配置基準点
// color：描画色
void Billboard_Draw(
    int texId,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT2& pivot,
    const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
);


// テクスチャの一部分を指定してビルボードを描画する。
// uv_offset：テクスチャ上の開始位置
// uv_size：使用するUV領域のサイズ
// scale：描画サイズ
// color：描画色
// pivot：回転・配置基準点
void Billboard_Draw(
    int texId,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT2& uv_offset,
    const DirectX::XMFLOAT2& uv_size,
    const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT4& color,
    const DirectX::XMFLOAT2& pivot
);


// ピボットを原点として、指定したサイズと色で描画する。
void Billboard_Draw(
    int texid,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT4& color
);

