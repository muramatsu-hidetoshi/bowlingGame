
/*==========================================================================
ボール制御機能の宣言 [ball.h]

概要：
    ボールの初期化、更新、描画、ショット操作などの関数を宣言する。

主な機能：
    ・ボールの初期化と終了処理
    ・重力および速度に基づく移動
    ・マップとの衝突判定
    ・ショットによる速度の設定
    ・ボールの位置と当たり判定領域の取得

==========================================================================*/

#pragma once

#include "DirectXMath.h"
#include "collision.h"


// ボールを指定した位置に初期化する。
void Ball_Initialize(const DirectX::XMFLOAT3& position);

// ボールの使用終了時に、読み込んだリソースを解放する。
void Ball_Finalize();

// 経過時間に応じてボールの位置・速度・衝突判定を更新する。
void Ball_Update(double et);

// ボールを描画する。
void Ball_Draw();

// ボールが停止しているかを判定する。
bool Ball_IsStationary();

// ボールの現在位置を取得する。
DirectX::XMFLOAT3 Ball_GetPosition();

// 指定された速度をボールに加え、ショットを行う。
void BallShot(const DirectX::XMFLOAT3& velocity);

// ボールの当たり判定領域を取得する。
const AABB& Ball_GetAABB();

