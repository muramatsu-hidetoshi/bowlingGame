
/*==============================================================================
    ポリゴン描画 [polygon.cpp]

    頂点バッファに頂点情報を書き込み、シェーダーを使用して
    画面上に四角形のポリゴンを描画する。
==============================================================================*/

#include <d3d11.h>
#include <DirectXMath.h>
#include "DirectXTex.h"
#include <algorithm> // std::min
using namespace DirectX;

#include "shader.h"
#include "direct3d.h"
#include "debug_ostream.h"

// 描画に使用する頂点数。四角形なので4頂点。
static constexpr int NUM_VERTEX = 4;

// ポリゴンの頂点情報を格納するバッファ。
static ID3D11Buffer* g_pVertexBuffer = nullptr;

// ポリゴンに使用するテクスチャビュー。
// 複数のテクスチャを扱う場合は、用途に応じて管理する。
static ID3D11ShaderResourceView* g_pTexture = nullptr;

// 初期化時に外部から受け取るDirect3Dのオブジェクト。
// 外部で管理されるため、このファイルでは解放しない。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// 頂点情報をまとめる構造体。
struct Vertex
{
    XMFLOAT3 position; // 頂点の座標
    XMFLOAT4 color;    // 頂点の色
    XMFLOAT2 texcoord; // テクスチャを参照するUV座標
};

// Direct3Dのデバイスとコンテキストを受け取り、描画に必要なリソースを作成する。
void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // デバイスまたはコンテキストが無効なら初期化を中断する。
    if (!pDevice || !pContext) {
        hal::dout << "Polygon_Initialize() : 与えられたデバイスかコンテキストが不正です";
        return;
    }

    // Direct3Dのオブジェクトを保存する。
    g_pDevice = pDevice;
    g_pContext = pContext;

    // 頂点バッファの設定を行う。
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;          // CPUから頂点データを書き換えられるようにする
    bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX; // 頂点4個分のメモリサイズ
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファとして使用する
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUからの書き込みを許可する

    // 頂点バッファを作成する。
    g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
}

// 使用し終わったDirect3Dリソースを解放する。
void Polygon_Finalize(void)
{
    SAFE_RELEASE(g_pTexture);
    SAFE_RELEASE(g_pVertexBuffer);

    // g_pDeviceとg_pContextは外部で管理されているため解放しない。
}

// 四角形のポリゴンを描画する。
void Polygon_Draw(void)
{
    /*
        CPUとGPUは基本的に独立して処理を進める。

        CPU側で頂点データを更新する場合は、GPUに渡す頂点バッファへ
        データを書き込む必要がある。

        Mapでバッファを書き込み可能な状態にし、頂点データを更新する。
        更新後にUnmapを呼び出すことで、バッファへの書き込みを終了する。
    */

    // 使用するシェーダーを描画パイプラインに設定する。
    Shader_Begin();

    // 頂点バッファをCPUから書き換えられる状態にする。
    // WRITE_DISCARDは、以前のデータを破棄して新しいデータを書き込む指定。
    D3D11_MAPPED_SUBRESOURCE msr;
    g_pContext->Map(
        g_pVertexBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &msr
    );

    // 頂点バッファのメモリをVertex型の配列として扱う。
    Vertex* v = (Vertex*)msr.pData;

    // バックバッファの幅と高さを取得する。
    const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
    const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

    // 四角形の大きさと描画位置を設定する。
    static float b = 0.0f; // テクスチャ座標に加える値
    constexpr float W = 512.0f;
    constexpr float H = 512.0f;

    float x = 64.0f;
    float y = 64.0f;

    // 四角形の4頂点を設定する。
    // TriangleStripで描画するため、頂点の並び順が重要になる。
    v[0].position = { x,     y,     0.0f };
    v[1].position = { x + W, y,     0.0f };
    v[2].position = { x,     y + H, 0.0f };
    v[3].position = { x + W, y + H, 0.0f };

    // すべての頂点を白色に設定する。
    // テクスチャの色をそのまま表示したい場合などに使用する。
    v[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    v[1].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    v[2].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    v[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 各頂点のテクスチャ座標を設定する。
    // 0～4の範囲を指定しているため、テクスチャの繰り返し表示が想定される。
    v[0].texcoord = { 0.0f + b, 0.0f + b };
    v[1].texcoord = { 4.0f + b, 0.0f + b };
    v[2].texcoord = { 0.0f + b, 4.0f + b };
    v[3].texcoord = { 4.0f + b, 4.0f + b };

    // bを増やすと、テクスチャ座標を少しずつ変化させられる。
    // b += 0.001f;

    // 頂点バッファへの書き込みを終了する。
    g_pContext->Unmap(g_pVertexBuffer, 0);

    // 頂点バッファを入力アセンブラに設定する。
    UINT stride = sizeof(Vertex); // 1頂点分のデータサイズ
    UINT offset = 0;              // バッファの読み込み開始位置
    g_pContext->IASetVertexBuffers(
        0, 1, &g_pVertexBuffer, &stride, &offset
    );

    // 画面座標を使って描画するため、正射影行列を設定する。
    // 左上を原点とし、画面右方向をX軸、下方向をY軸として扱う。
    Shader_SetProjectionMatrix(
        XMMatrixOrthographicOffCenterLH(
            0.0f, SCREEN_WIDTH,
            SCREEN_HEIGHT, 0.0f,
            0.0f, 1.0f
        )
    );

    // 描画に使用する色を設定する。
    // aはアルファ値で、0に近いほど透明、1に近いほど不透明になる。
    static float a = 0.5f;
    Shader_SetColor({ 1.0f, 1.0f, 0.0f, a });

    // アルファ値を少しずつ増加させ、最大1.0で止める。
    a = std::min(a + 0.005f, 1.0f);

    // 頂点を三角形ストリップとして解釈するように設定する。
    g_pContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
    );

    // 頂点バッファに設定した4頂点を使用してポリゴンを描画する。
    g_pContext->Draw(NUM_VERTEX, 0);
}
