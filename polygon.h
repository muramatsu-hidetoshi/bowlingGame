/*==============================================================================
    ポリゴン描画 [polygon.h]

    ポリゴン描画機能の初期化・終了処理・描画処理を公開する。
==============================================================================*/

#ifndef POLYGON_H
#define POLYGON_H

#include <d3d11.h>

// Direct3Dのデバイスとコンテキストを受け取り、描画リソースを初期化する。
void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

// ポリゴン描画で使用したリソースを解放する。
void Polygon_Finalize(void);

// ポリゴンを描画する。
void Polygon_Draw(void);

#endif // POLYGON_H