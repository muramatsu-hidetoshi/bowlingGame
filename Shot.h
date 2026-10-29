
/*==========================================================================
ショットの処理[shot.h]



---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include "DirectXMath.h"
#include "mouse.h"




enum class ShotState
{
    Charge,     // 連打でパワー決定
    Aim,        // 矢印が左右に揺れる
    Fired
};
bool Shot_IsFired();
DirectX::XMFLOAT3 Shot_GetShotVelocity();
static ShotState g_State = ShotState::Charge;

void Shot_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3&   front);
void Shot_Finalize();
void Shot_Update(double et);
void Shot_Draw();

const DirectX::XMFLOAT3& Shot_GetVelocity();
void Shot_SetPosition(const DirectX::XMFLOAT3& position);
float Shot_GetPower();
void Shot_ResetPower();
float Clamp(float v, float minV, float maxV);
float Shot_GetChargeRatio();
bool  Shot_IsCharging();
void Shot_DrawUI();
bool Shot_ConsumeFire();
