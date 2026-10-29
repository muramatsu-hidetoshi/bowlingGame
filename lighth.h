
/*==========================================================================
ƒ‰ƒCƒg‚Ì’è‹`[light.cpp]




---------------------------------------------------------------------------






=========================================================================*/


#pragma once
#include <DirectXMath.h>


struct DiffuseLight
{

	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 direction;


};

void Light_Initialize();
void Light_Finalize();
void Light_SetAmnient(const DirectX::XMFLOAT4& color);
void Light_SetDiffuse(const DirectX::XMFLOAT3& color,const DirectX::XMFLOAT3& direction);
void Light_SetDiffuse(const DiffuseLight& light);
