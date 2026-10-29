
/*==========================================================================

テクスチャの管理[texture.h]


---------------------------------------------------------------------------






=========================================================================*/
#ifndef TEXTURE_H
#define TEXTURE_H
#include <d3d11.h>
#include<DirectXMath.h>
//ID3D11Device* pDevice, ID3D11DeviceContext* pContext
//const char* filename, ID3D11ShaderResourceView** ppTexture

void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Texture_Finalize();

// テクスチャの読み込み
//
//引数
// pFileName : 読み込むテクスチャのファイル名
// bMipMap  : ミップマップを生成する時はtrue
// 戻り値 テクスチャ管理番号
// 0 : 成功
// -1 : 失敗
//
int Texture_Load(const wchar_t* pFileName,bool bMipMap=true );


void Texture_AllRelease();

void Tecture_SetTexture(int texture_id);

DirectX::XMUINT2 Texture_GetSize(int texture_id);

const unsigned int Texture_GetWidht(int texture_id);
const unsigned int Texture_GetHeight(int texture_id);

#endif // !TEXTURE_H


//#pragma once
