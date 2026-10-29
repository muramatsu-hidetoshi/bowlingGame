/*==========================================================================

ƒgƒŒƒCƒ‹(ƒ|ƒŠƒSƒ“)‚Ì•\¦[trail.h]


---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <DirectXMath.h>

void Trail_Initialize();
void Trail_Finalize();
void Trail_Draw();
void Trail_AddPosition(const DirectX::XMFLOAT3& position);
void Trail_SetCameraPosition(const DirectX::XMFLOAT3& position);
// trail.h ‚É’Ç‰Á
void Trail_Clear();
void Trail_SetEnabled(bool enabled);
void Trail_SetFadeEnabled(bool enabled);
void Trail_SetColor(const DirectX::XMFLOAT4& startColor, const DirectX::XMFLOAT4& endColor);
/*Tail‚ğ•`‰æ‚·‚é‚µ‚È‚¢‚ÌŠÖ”‚ğ’Ç‰Á‚µ‚Ä”»’f*/

