
/*==========================================================================

テクスチャの管理[texture.cpp]


---------------------------------------------------------------------------






=========================================================================*/

#include "texture.h"
#include<string>
#include"direct3d.h"
#include "DirectXTex.h"
using namespace DirectX;
static constexpr int TEXTURE_MAX = 1024; // 最大テクスチャ数

struct Texture
{
	std::wstring fileName; // テクスチャファイル名
	unsigned int width{}; // テクスチャの幅
	unsigned int height{}; // テクスチャの高さ
	ID3D11ShaderResourceView* pTexture = nullptr; // テクスチャビュー（必要なら追加）


};

static Texture g_Textures[TEXTURE_MAX]; // テクスチャ管理配列

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;



void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // テクスチャ管理で使うDirect3Dデバイスとコンテキストを保持する。


	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;


}

void Texture_Finalize()
{
    // 登録されているテクスチャを解放して管理状態を終了する。
	Texture_AllRelease();
}

int Texture_Load(const wchar_t* pFileName, bool bMipMap)
{
    // 画像を読み込み、必要に応じてミップマップを生成して管理配列へ登録する。

	//すでに読みこんでいたファイル対応
	for (int i = 0; i < TEXTURE_MAX; i++) {

		if (!g_Textures[i].pTexture) continue;

		if (g_Textures[i].fileName == pFileName) {
			return i;
		}


	}




	//管理処理
	for (int i = 0; i < TEXTURE_MAX; i++) {

		//空いている場所を探す
		if (g_Textures[i].pTexture) continue;

		//テクスチャのファイルからの読み込み
		TexMetadata metadata;
		ScratchImage image;

		//画像ファイルのサイズを取得
		LoadFromWICFile(pFileName, WIC_FLAGS_NONE, &metadata, image);



		g_Textures[i].width = (unsigned int)metadata.width; // テクスチャの幅)
		g_Textures[i].height = (unsigned int)metadata.height; // テクスチャの高さ

		if (bMipMap) {

			//ミニマップを作成
			ScratchImage mipChain;
			GenerateMipMaps(
				image.GetImages(),
				image.GetImageCount(),
				image.GetMetadata(),
				TEX_FILTER_DEFAULT,
				0,
				mipChain
			);

			//TEX_FILTER_BOX | TEX_FILTER_FORCE_NON_WIC

			image = std::move(mipChain);
			metadata = image.GetMetadata();

		}



		//シェーダーリソースビューの作成
		HRESULT hr = CreateShaderResourceView(
			g_pDevice, image.GetImages(), image.GetImageCount(),
			metadata, &g_Textures[i].pTexture);

		if (FAILED(hr)) {
			MessageBox(nullptr, "テクスチャの読み込みに失敗しました", "error", MB_OK | MB_ICONERROR);
			break;
		}

		g_Textures[i].fileName = pFileName; // ファイル名を保存

		return i; // 成功した場合はインデックスを返す
	}



	return -1;
}

void Texture_AllRelease()
{
	for (Texture& t: g_Textures) {

		SAFE_RELEASE(t.pTexture);

	}

}

void Tecture_SetTexture(int texture_id)
{

	if (texture_id < 0) {

		return;
	}


	//頂点にはUV、サンプラーが必要
	// テクスチャをピクセルシェーダーに設定
	g_pContext->PSSetShaderResources(0, 1, &g_Textures[texture_id].pTexture); // テクスチャをピクセルシェーダーに設定



}

DirectX::XMUINT2 Texture_GetSize(int texture_id)
{

	return { g_Textures[texture_id].width,g_Textures[texture_id].height };

}


const unsigned int Texture_GetWidht(int texture_id)
{
	return g_Textures[texture_id].width;
}

const unsigned int Texture_GetHeight(int texture_id)
{
	return g_Textures[texture_id].height;
}
