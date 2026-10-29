/*==========================================================================

 [direct3d.h]


---------------------------------------------------------------------------






=========================================================================*/





#ifndef DIRECT3D_H
#define DIRECT3D_H

#include <windows.h>
#include <d3d11.h>
#define SAFE_RELEASE(o) if (o){(o)->Release(); o = NULL;}

bool Direct3D_Initialize(HWND hWnd);
void Direct3D_Finalize();

void Direct3D_Clear();
void Direct3D_Present();
unsigned int  Direct3D_GetBackBufferWidth();
unsigned int  Direct3D_GetBackBufferHeight();
ID3D11Device* Direct3D_GetDevice();
ID3D11DeviceContext* Direct3D_GetContext();

enum AlphaBlendMode
{
	BLEND_TRANSPARENT,
	BLEND_ADD,
	BLEND_MAX

};


void Direct3D_SetAlphaBlend(AlphaBlendMode blend);
void Direct3D_SetDepthTest(bool bEnable);
void Direct3D_SetDepthWriteDisable();
void Direct3D_SetDepthWriteEnable();
//void SetViewPort(int n);
#endif // !DIRECT3D_H
