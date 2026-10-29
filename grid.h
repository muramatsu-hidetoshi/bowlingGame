/*==========================================================================
グリッドの管理 [grid.h]

グリッドの初期化・更新・描画・終了処理を宣言する。
==========================================================================*/

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

// グリッドを初期化する。x_count、z_countは各方向の分割数、sizeは間隔
void Gulid_Initialize(int x_count, int z_count, float size);

// グリッドのリソースを解放する
void Gulid_Finalize();

// グリッドを更新する
void Gulid_Update(double elapsedTime);

// グリッドを描画する
void Gulid_Draw();

// 任意の位置・回転・スケールで描画する場合の拡張用
// void Gulid_Draw(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale);


