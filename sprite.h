/*==========================================================================

スプライト描画[sprite.h]


---------------------------------------------------------------------------






=========================================================================*/

#ifndef SPRITE_H
#define SPRITE_H
#include <DirectXMath.h>
//#include <DirectXColors.h>
#include <d3d11.h>

// スプライト描画に必要なリソースを初期化する。
void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

// スプライト描画用のリソースを解放する。
void Sprite_Finalize(void);

// テクスチャの指定範囲を、指定位置・サイズ・色で描画する。
void Sprite_Draw(
	int texid,
	float x, float y,
	float width, float height,
	int tx, int ty,
	int tw, int th,
	DirectX::XMFLOAT4 color = {1.0f,1.0f,1.0f,1.0f}
);

void Sprite_Draw(
	int texid,
	float x, float y,
	float w, float h,
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }
);
// テクスチャ全体を指定位置・サイズで描画する。
void Sprite_Draw(int texid, float x, float y, float w, float h, float scrollX, float scrollY, DirectX::XMFLOAT4 color);
// 回転角度を指定してスプライトを描画する。
// angleの単位はラジアン。
void Sprite_Draw(
	int texid,
	float angle, // 回転角度（ラジアン単位）
	float x, float y,
	float width, float height,
	int tx, int ty,
	int tw, int th,
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }
);
// 中心座標を基準にスプライトを描画する。
void Sprite_DrawCenter(
	int texid,
	float cx, float cy,
	float w, float h,
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }
);






#endif