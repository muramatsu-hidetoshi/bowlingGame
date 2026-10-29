/*==========================================================================

エフェクト制御[game_effect.h]


---------------------------------------------------------------------------






=========================================================================*/


#pragma once
#include <DirectXMath.h>

void Effect_Initialize(); // タイトルの初期化
void Effect_Update(double elapsed_time); // タイトルの更新
void Effect_Draw(); // タイトルの描画
void Effect_Finalize(); // タイトルの終了


void Effect_Create(const DirectX::XMFLOAT2& position, float size, double lifeTime, const DirectX::XMFLOAT4& color);
