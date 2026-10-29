
/*==============================================================================
    2D描画用シェーダーの管理 [shader.cpp]

    頂点シェーダー・ピクセルシェーダーの読み込みや作成を行う。
    また、描画に必要な定数バッファやサンプラーステートを管理する。
==============================================================================*/

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>
#include "debug_ostream.h"
#include "direct3d.h"

// シェーダー関連のDirect3Dリソース
static ID3D11VertexShader* g_pVertexShader = nullptr;
static ID3D11InputLayout* g_pInputLayout = nullptr;

// 頂点シェーダーに渡す定数バッファ
static ID3D11Buffer* g_pVSConstantBuffer0 = nullptr; // 投影行列
static ID3D11Buffer* g_pVSConstantBuffer1 = nullptr; // ワールド行列

// ピクセルシェーダーに渡す定数バッファ
static ID3D11Buffer* g_pPSConstantBuffer = nullptr;

static ID3D11PixelShader* g_pPixelShader = nullptr;

// テクスチャのサンプリング方法を設定するステート
static ID3D11SamplerState* g_pSamplerState = nullptr;

// 注意：これらは初期化時に外部から受け取るため、ここではReleaseしない
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


// シェーダー関連リソースを初期化する
bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    HRESULT hr; // Direct3D関数の実行結果

    // デバイスとデバイスコンテキストが有効か確認する
    if (!pDevice || !pContext) {
        hal::dout << "Shader_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
        return false;
    }

    // 描画処理で使用するため、受け取ったポインタを保存する
    g_pDevice = pDevice;
    g_pContext = pContext;


    // 事前コンパイル済み頂点シェーダーをバイナリファイルから読み込む
    std::ifstream ifs_vs("VertexShader2D.cso", std::ios::binary);

    if (!ifs_vs) {
        MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_2d.cso", "エラー", MB_OK);
        return false;
    }

    // ファイルサイズを取得するため、ファイルポインタを末尾へ移動する
    ifs_vs.seekg(0, std::ios::end);
    std::streamsize filesize = ifs_vs.tellg();

    // 読み込みを開始するため、ファイルポインタを先頭へ戻す
    ifs_vs.seekg(0, std::ios::beg);

    // シェーダーのバイナリデータを格納するメモリを確保する
    unsigned char* vsbinary_pointer = new unsigned char[filesize];

    // ファイルの内容をメモリへ読み込む
    ifs_vs.read((char*)vsbinary_pointer, filesize);
    ifs_vs.close();

    // 読み込んだバイナリデータから頂点シェーダーを作成する
    hr = g_pDevice->CreateVertexShader(
        vsbinary_pointer, filesize, nullptr, &g_pVertexShader);

    if (FAILED(hr)) {
        hal::dout << "Shader_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
        delete[] vsbinary_pointer;
        return false;
    }


    // 頂点データの各要素と、シェーダー側の入力項目を対応付ける
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        // 頂点位置
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
          D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

          // 頂点カラー
          { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            // テクスチャ座標
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
              D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // 入力レイアウトの要素数を取得する
    UINT num_elements = ARRAYSIZE(layout);

    // 頂点シェーダーと頂点データの形式を対応付ける入力レイアウトを作成する
    hr = g_pDevice->CreateInputLayout(
        layout, num_elements, vsbinary_pointer, filesize, &g_pInputLayout);

    // 頂点シェーダーの作成と入力レイアウトの作成に使ったデータを解放する
    delete[] vsbinary_pointer;

    if (FAILED(hr)) {
        hal::dout << "Shader_Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
        return false;
    }


    // 頂点シェーダー用の定数バッファを作成する
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(XMFLOAT4X4);
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // 投影行列とワールド行列をそれぞれ格納する
    g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer0);
    g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer1);

    // ピクセルシェーダー用の定数バッファも作成する
    g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer);


    // 事前コンパイル済みピクセルシェーダーを読み込む
    std::ifstream ifs_ps("PixelShader2D.cso", std::ios::binary);

    if (!ifs_ps) {
        MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました\n\nshader_pixel_2d.cso", "エラー", MB_OK);
        return false;
    }

    // ピクセルシェーダーのファイルサイズを取得する
    ifs_ps.seekg(0, std::ios::end);
    filesize = ifs_ps.tellg();
    ifs_ps.seekg(0, std::ios::beg);

    // ピクセルシェーダーのバイナリデータを読み込む
    unsigned char* psbinary_pointer = new unsigned char[filesize];
    ifs_ps.read((char*)psbinary_pointer, filesize);
    ifs_ps.close();

    // バイナリデータからピクセルシェーダーを作成する
    hr = g_pDevice->CreatePixelShader(
        psbinary_pointer, filesize, nullptr, &g_pPixelShader);

    delete[] psbinary_pointer;

    if (FAILED(hr)) {
        hal::dout << "Shader_Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
        return false;
    }


    // ピクセルシェーダーへ渡す色データ用の定数バッファを作成する
    buffer_desc.ByteWidth = sizeof(XMFLOAT4);
    g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer);


    // テクスチャのサンプリング方法を設定する
    D3D11_SAMPLER_DESC sampler_desc{};

    // 最も近いテクセルの色を取得するポイントサンプリングを使用する
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    // UV座標が範囲外になった場合、テクスチャを繰り返して表示する
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

    // W方向のアドレス指定方法
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    // 境界色を設定する
    sampler_desc.BorderColor[0] = 1.0f;
    sampler_desc.BorderColor[1] = 1.0f;
    sampler_desc.BorderColor[2] = 1.0f;
    sampler_desc.BorderColor[3] = 0.0f;

    sampler_desc.MipLODBias = 0;
    sampler_desc.MaxAnisotropy = 16;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    // 設定した内容からサンプラーステートを作成する
    g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);

    return true;
}


// シェーダー関連リソースを解放する
void Shader_Finalize()
{
    SAFE_RELEASE(g_pSamplerState);
    SAFE_RELEASE(g_pPSConstantBuffer);
    SAFE_RELEASE(g_pPixelShader);
    SAFE_RELEASE(g_pVSConstantBuffer0);
    SAFE_RELEASE(g_pVSConstantBuffer1);
    SAFE_RELEASE(g_pInputLayout);
    SAFE_RELEASE(g_pVertexShader);
}


// 投影行列を頂点シェーダー用定数バッファへ設定する
void Shader_SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
    XMFLOAT4X4 transpose;

    // 行列を転置して、定数バッファへ渡す形式に変換する
    XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

    // 頂点シェーダーの定数バッファ0へ行列を転送する
    g_pContext->UpdateSubresource(
        g_pVSConstantBuffer0, 0, nullptr, &transpose, 0, 0);
}


// ワールド行列を頂点シェーダー用定数バッファへ設定する
void Shader_SetWorldMatrix(const DirectX::XMMATRIX& matrix)
{
    XMFLOAT4X4 transpose;

    // 行列を転置して、定数バッファへ渡す形式に変換する
    XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

    // 頂点シェーダーの定数バッファ1へ行列を転送する
    g_pContext->UpdateSubresource(
        g_pVSConstantBuffer1, 0, nullptr, &transpose, 0, 0);
}


// ピクセルシェーダーへ渡す色を設定する
void Shader_SetColor(const XMFLOAT4& Color)
{
    g_pContext->UpdateSubresource(
        g_pPSConstantBuffer, 0, nullptr, &Color, 0, 0);
}


// 描画に使用するシェーダーや各種設定をパイプラインへ登録する
void Shader_Begin()
{
    // 頂点シェーダーとピクセルシェーダーを設定する
    g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);

    // 頂点データの入力形式を設定する
    g_pContext->IASetInputLayout(g_pInputLayout);

    // 頂点シェーダーへ投影行列とワールド行列を渡す
    g_pContext->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer0);
    g_pContext->VSSetConstantBuffers(1, 1, &g_pVSConstantBuffer1);

    // ピクセルシェーダーへ色データを渡す
    g_pContext->PSSetConstantBuffers(0, 1, &g_pPSConstantBuffer);

    // テクスチャのサンプリング設定を登録する
    g_pContext->PSSetSamplers(0, 1, &g_pSamplerState);
}


