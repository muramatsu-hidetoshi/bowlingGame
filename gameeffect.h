
/*==========================================================================
    エフェクト制御機能の関数を宣言する [gameeffect.h]

    エフェクトの初期化・更新・描画・終了処理と、
    エフェクト生成処理を外部から利用できるようにする。
==========================================================================*/

#pragma once
#include <DirectXMath.h>

// エフェクト管理を初期化する
void Effect_Initialize();

// 経過時間に応じてエフェクトを更新する
void Effect_Update(double elapsed_time);

// エフェクトを描画する
void Effect_Draw();

// エフェクト管理を終了する
void Effect_Finalize();

// 指定された位置・サイズ・寿命・色でエフェクトを生成する
void Effect_Create(
    const DirectX::XMFLOAT2& position,
    float size,
    double lifeTime,
    const DirectX::XMFLOAT4& color);
