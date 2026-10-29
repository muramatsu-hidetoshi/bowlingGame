/*==========================================================================
    立方体の定義 [cube.h]

    【ファイルの役割】
    立方体の初期化・更新・描画・終了処理を宣言する。

    立方体の頂点バッファやインデックスバッファを使用し、
    指定されたワールド行列とテクスチャを使って立方体を描画する。
==========================================================================*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// 立方体の描画に使用する関数
// Direct3Dのデバイスとコンテキストを受け取り、描画に必要なリソースを初期化する
void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

// 立方体が使用するリソースを解放する
void Cube_Finalize();

// 立方体の状態を更新する

// 指定されたワールド行列とテクスチャIDを使用して立方体を描画する
void Cube_Draw(const DirectX::XMMATRIX& mtxWorld, int texid);