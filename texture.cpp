
/*==========================================================================
テクスチャの管理 [texture.cpp]

画像の読み込み、管理、シェーダーへの設定、リソースの解放を行う。
==========================================================================*/

#include "texture.h"
#include <string>
#include "direct3d.h"
#include "DirectXTex.h"

using namespace DirectX;

static constexpr int TEXTURE_MAX = 1024; // 管理できるテクスチャの最大数

// テクスチャ1枚分の情報
struct Texture
{
    std::wstring fileName; // テクスチャのファイル名
    unsigned int width{}; // テクスチャの幅
    unsigned int height{}; // テクスチャの高さ
    ID3D11ShaderResourceView* pTexture = nullptr; // シェーダーから参照するテクスチャ
};

// 読み込んだテクスチャを管理する配列
static Texture g_Textures[TEXTURE_MAX];

// 初期化時に外部から渡されるDirect3D関連のポインタ
// ここでは所有権を持たないため、この関数内ではReleaseしない
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// テクスチャ管理を初期化する
void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // テクスチャの作成と描画に必要なポインタを保存する
    g_pDevice = pDevice;
    g_pContext = pContext;
}

// 読み込んだテクスチャをすべて解放する
void Texture_Finalize()
{
    // 管理配列内のテクスチャを解放する
    Texture_AllRelease();
}

// 画像ファイルを読み込み、テクスチャとして登録する
int Texture_Load(const wchar_t* pFileName, bool bMipMap)
{
    // 同じファイルがすでに読み込まれているか確認する
    // 読み込み済みであれば、既存の管理番号を返す
    for (int i = 0; i < TEXTURE_MAX; i++)
    {
        // テクスチャが登録されていない要素は無視する
        if (!g_Textures[i].pTexture)
            continue;

        // ファイル名が一致した場合は、既存のテクスチャを再利用する
        if (g_Textures[i].fileName == pFileName)
        {
            return i;
        }
    }

    // 未登録のテクスチャを格納できる空き領域を探す
    for (int i = 0; i < TEXTURE_MAX; i++)
    {
        // すでに使用中の領域はスキップする
        if (g_Textures[i].pTexture)
            continue;

        // 画像ファイルの情報と読み込んだ画像データを保持する変数
        TexMetadata metadata;
        ScratchImage image;

        // 画像ファイルを読み込む
        // metadataには画像サイズなどの情報が格納される
        LoadFromWICFile(pFileName, WIC_FLAGS_NONE, &metadata, image);

        // 読み込んだ画像の幅と高さを保存する
        g_Textures[i].width = (unsigned int)metadata.width;
        g_Textures[i].height = (unsigned int)metadata.height;

        // 必要に応じてミップマップを生成する
        if (bMipMap)
        {
            ScratchImage mipChain;

            // 元画像から複数の解像度の画像データを生成する
            GenerateMipMaps(
                image.GetImages(),
                image.GetImageCount(),
                image.GetMetadata(),
                TEX_FILTER_DEFAULT,
                0,
                mipChain
            );

            // 生成したミップマップを読み込み画像として使用する
            image = std::move(mipChain);
            metadata = image.GetMetadata();
        }

        // 読み込んだ画像データからシェーダーリソースビューを作成する
        HRESULT hr = CreateShaderResourceView(
            g_pDevice,
            image.GetImages(),
            image.GetImageCount(),
            metadata,
            &g_Textures[i].pTexture
        );

        // 作成に失敗した場合はエラーメッセージを表示する
        if (FAILED(hr))
        {
            MessageBox(
                nullptr,
                "テクスチャの読み込みに失敗しました",
                "error",
                MB_OK | MB_ICONERROR
            );

            break;
        }

        // テクスチャのファイル名を保存し、管理番号を返す
        g_Textures[i].fileName = pFileName;

        return i;
    }

    // 空き領域がない場合や読み込みに失敗した場合は-1を返す
    return -1;
}

// 登録されているテクスチャをすべて解放する
void Texture_AllRelease()
{
    for (Texture& t : g_Textures)
    {
        // シェーダーリソースビューを解放する
        SAFE_RELEASE(t.pTexture);
    }
}

// 指定されたテクスチャをピクセルシェーダーに設定する
void Tecture_SetTexture(int texture_id)
{
    // 無効なIDが渡された場合は何もしない
    if (texture_id < 0)
    {
        return;
    }

    // スロット0にテクスチャを設定する
    // ピクセルシェーダーから、このテクスチャを参照できるようにする
    g_pContext->PSSetShaderResources(
        0,
        1,
        &g_Textures[texture_id].pTexture
    );
}

// 指定されたテクスチャの幅と高さをまとめて取得する
DirectX::XMUINT2 Texture_GetSize(int texture_id)
{
    return {
        g_Textures[texture_id].width,
        g_Textures[texture_id].height
    };
}

// 指定されたテクスチャの幅を取得する
const unsigned int Texture_GetWidht(int texture_id)
{
    return g_Textures[texture_id].width;
}

// 指定されたテクスチャの高さを取得する
const unsigned int Texture_GetHeight(int texture_id)
{
    return g_Textures[texture_id].height;
}
