
/*==========================================================================
テクスチャの管理 [texture.h]

テクスチャの読み込み、設定、サイズ取得、解放に関する関数を宣言する。
==========================================================================*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <DirectXMath.h>

// テクスチャ管理を初期化する
// pDevice  : Direct3Dデバイス
// pContext : Direct3Dデバイスコンテキスト
void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

// 登録されているテクスチャをすべて解放する
void Texture_Finalize();

// テクスチャを読み込む
// pFileName : 読み込む画像ファイルのパス
// bMipMap   : trueの場合、ミップマップを生成する
// 戻り値    : テクスチャの管理番号。失敗した場合は-1
int Texture_Load(const wchar_t* pFileName, bool bMipMap = true);

// 登録されているテクスチャをすべて解放する
void Texture_AllRelease();

// 指定されたテクスチャをピクセルシェーダーに設定する
void Tecture_SetTexture(int texture_id);

// テクスチャの幅と高さを取得する
DirectX::XMUINT2 Texture_GetSize(int texture_id);

// テクスチャの幅を取得する
const unsigned int Texture_GetWidht(int texture_id);

// テクスチャの高さを取得する
const unsigned int Texture_GetHeight(int texture_id);

#endif // TEXTURE_H
