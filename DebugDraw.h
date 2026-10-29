/*==========================================================================
    デバッグ用ライン描画ユーティリティの定義 [debugDraw.h]

    【ファイルの役割】
    デバッグ用の線分やAABBのワイヤーフレームを描画するための
    関数を宣言する。

    【主な機能】
    ・デバッグ描画に必要なリソースを初期化する
    ・任意の2点間に線分を登録する
    ・AABBの12本の辺を線分として登録する
    ・登録した線分をまとめて描画する
==========================================================================*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "collision.h" // AABBによる当たり判定範囲の描画に使用する

// デバッグ描画を初期化する。
// maxLinesは一度に登録できる線分の最大数を指定する。
void DebugDraw_Initialize(int maxLines = 8192);

// デバッグ描画に使用したリソースを解放する。
void DebugDraw_Finalize();

// 指定した2点を結ぶ線分を登録する。
// colorには線分の色を指定する。省略した場合は白色になる。
void DebugDraw_AddLine(
    const DirectX::XMFLOAT3& a,
    const DirectX::XMFLOAT3& b,
    const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f }
);

// AABBの12本の辺を線分として登録する。
// colorにはワイヤーフレームの色を指定する。省略した場合は赤色になる。
void DebugDraw_AddAABB(
    const AABB& aabb,
    const DirectX::XMFLOAT4& color = { 1.0f, 0.0f, 0.0f, 1.0f }
);

// 登録されている線分をまとめて描画する。
void DebugDraw_Draw();