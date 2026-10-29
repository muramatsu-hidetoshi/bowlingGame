/*==========================================================================
    立方体の実装 [cube.cpp]

    【ファイルの役割】
    立方体の頂点データとインデックスデータを定義し、
    Direct3Dのバッファを使用して立方体を描画する。

    【主な処理】
    ・頂点バッファとインデックスバッファの作成
    ・頂点データとインデックスデータの登録
    ・シェーダーやテクスチャの設定
    ・インデックスを使用した立方体の描画
==========================================================================*/

#include <d3d11.h>

#include "cube.h"
#include <DirectXMath.h>
#include "DirectXTex.h"
#include "debug_ostream.h"
#include "shader3d.h"
#include "direct3d.h"
#include "keyboard.h"
#include "keylogger.h"
#include "texture.h"

using namespace DirectX;

// 立方体の頂点数。
// 1面につき4頂点、6面で合計24頂点を使用する。
static constexpr int NUM_VERTEX = 4 * 6;

// 立方体のインデックス数。
// 1面につき2つの三角形を使用し、合計12三角形を描画する。
static constexpr int NUM_INDEX = 36;

// 立方体の描画に使用するインデックスバッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr;

// 立方体の描画に使用する頂点バッファ
static ID3D11Buffer* g_pVertexBuffer = nullptr;

// 初期化時に外部から受け取るDirect3Dのデバイスとコンテキスト。
// 外部で管理されているため、このファイルでは解放しない。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


// 頂点データの構造体
struct Vertex3d
{
    DirectX::XMFLOAT3 position; // 頂点の座標
    DirectX::XMFLOAT3 normal;   // ライティング計算に使用する法線
    DirectX::XMFLOAT4 color;    // 頂点の色
    DirectX::XMFLOAT2 texcoord; // テクスチャを貼り付ける位置を示すUV座標
};


// アニメーションなどに使用する時間。
// 現在のコードでは値を更新していない。
float g_time = 0.0f;


// 立方体の頂点データ。
// 各面に4頂点を用意し、面ごとに法線とテクスチャ座標を設定する。
// 立方体の中心を原点とし、各軸方向に-0.5～0.5の範囲で定義する。
static Vertex3d g_CubeVertex[] =
{
    // 前面（Z+）
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.000f, 0.000f}},
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.000f, 0.500f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.333f, 0.500f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1,1,1,1}, {0.333f, 0.000f}},

    // 背面（Z-）
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1,1,1,1}, {1.000f, 0.500f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1,1,1,1}, {1.000f, 1.000f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1,1,1,1}, {0.667f, 1.000f}},
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1,1,1,1}, {0.667f, 0.500f}},

    // 左面（X-）
    {{-0.5f,  0.5f, -0.5f}, {-1.0f,0.0f,0.0f}, {1,1,1,1}, {0.000f, 0.500f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f,0.0f,0.0f}, {1,1,1,1}, {0.000f, 1.000f}},
    {{-0.5f, -0.5f,  0.5f}, {-1.0f,0.0f,0.0f}, {1,1,1,1}, {0.333f, 1.000f}},
    {{-0.5f,  0.5f,  0.5f}, {-1.0f,0.0f,0.0f}, {1,1,1,1}, {0.333f, 0.500f}},

    // 右面（X+）
    {{ 0.5f,  0.5f,  0.5f}, {1.0f,0.0f,0.0f}, {1,1,1,1}, {0.667f, 0.000f}},
    {{ 0.5f, -0.5f,  0.5f}, {1.0f,0.0f,0.0f}, {1,1,1,1}, {0.667f, 0.500f}},
    {{ 0.5f, -0.5f, -0.5f}, {1.0f,0.0f,0.0f}, {1,1,1,1}, {1.000f, 0.500f}},
    {{ 0.5f,  0.5f, -0.5f}, {1.0f,0.0f,0.0f}, {1,1,1,1}, {1.000f, 0.000f}},

    // 上面（Y+）
    {{-0.5f,  0.5f, -0.5f}, {0.0f,1.0f,0.0f}, {1,1,1,1}, {0.333f, 0.000f}},
    {{-0.5f,  0.5f,  0.5f}, {0.0f,1.0f,0.0f}, {1,1,1,1}, {0.333f, 0.500f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f,1.0f,0.0f}, {1,1,1,1}, {0.667f, 0.500f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f,1.0f,0.0f}, {1,1,1,1}, {0.667f, 0.000f}},

    // 下面（Y-）
    {{-0.5f, -0.5f,  0.5f}, {0.0f,-1.0f,0.0f}, {1,1,1,1}, {0.333f, 0.500f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f,-1.0f,0.0f}, {1,1,1,1}, {0.333f, 1.000f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f,-1.0f,0.0f}, {1,1,1,1}, {0.667f, 1.000f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f,-1.0f,0.0f}, {1,1,1,1}, {0.667f, 0.500f}},
};


// 頂点を参照するインデックスデータ。
// 立方体の各面を2つの三角形に分割して描画する。
// 1面につき6インデックス、6面で合計36インデックスを使用する。
static unsigned short g_CubeIndex[] =
{
    // 前面
    0, 1, 2, 0, 2, 3,

    // 背面
    4, 5, 6, 4, 6, 7,

    // 左面
    8, 9, 10, 8, 10, 11,

    // 右面
    12, 13, 14, 12, 14, 15,

    // 上面
    16, 17, 18, 16, 18, 19,

    // 下面
    20, 21, 22, 20, 22, 23,
};


// 立方体の描画に必要なリソースを初期化する
void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // デバイスとデバイスコンテキストが有効か確認する
    if (!pDevice || !pContext)
    {
        hal::dout << "Cube_Initialize() : 与えられたデバイスかコンテキストが不正です";
        return;
    }

    // 描画に使用するデバイスとコンテキストを保存する
    // これらのリソース自体は外部で管理されている
    g_pDevice = pDevice;
    g_pContext = pContext;

    // 頂点バッファの設定情報を初期化する
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;                 // CPUから書き込み可能なバッファ
    bd.ByteWidth = sizeof(Vertex3d) * NUM_VERTEX;   // 頂点データ全体のサイズ
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;        // 頂点バッファとして使用する
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // CPUからの書き込みを許可する

    // 初期データとして立方体の頂点データを指定する
    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = g_CubeVertex;

    // 頂点データを格納する頂点バッファを作成する
    g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

    // インデックスバッファ用に設定を変更する
    bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    // 初期データとしてインデックスデータを指定する
    sd.pSysMem = g_CubeIndex;

    // 頂点を参照するインデックスバッファを作成する
    g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);
}


// 立方体が使用するDirect3Dリソースを解放する
void Cube_Finalize()
{
    // インデックスバッファと頂点バッファを解放する
    SAFE_RELEASE(g_pIndexBuffer);
    SAFE_RELEASE(g_pVertexBuffer);

    // 外部で管理されているデバイスとコンテキストは解放しない
    g_pDevice = nullptr;
    g_pContext = nullptr;
}


// 立方体の更新処理
void Cube_Update(double elapsedTime)
{
    // 現在は更新処理を行っていない
    // 回転やアニメーションなどを追加する場合はここに記述する
}


// 立方体を描画する
void Cube_Draw(const XMMATRIX& mtxWorld, int texid)
{
    // 立方体を描画するためのシェーダーを設定する
    Shader3d_Begin();

    // 頂点バッファの1頂点あたりのサイズと開始位置を指定する
    UINT stride = sizeof(Vertex3d);
    UINT offset = 0;

    // 描画パイプラインに頂点バッファを設定する
    g_pContext->IASetVertexBuffers(
        0,
        1,
        &g_pVertexBuffer,
        &stride,
        &offset
    );

    // 描画パイプラインにインデックスバッファを設定する
    // DXGI_FORMAT_R16_UINTは16ビット符号なし整数のインデックスを表す
    g_pContext->IASetIndexBuffer(
        g_pIndexBuffer,
        DXGI_FORMAT_R16_UINT,
        0
    );

    // オブジェクトの位置・回転・拡大縮小を反映するワールド行列を設定する
    // 実際に設定する行列は、呼び出し元から受け取ったmtxWorld
    Shader3d_SetWorldMatrix(mtxWorld);

    // 頂点を三角形として描画する設定にする
    g_pContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    // 指定されたテクスチャを描画に使用する
    Tecture_SetTexture(texid);

    // マテリアルの拡散反射色を白に設定する
    // テクスチャ本来の色を反映させるための設定
    Shader3d_SetMaterialDiffuse({ 1.0f, 1.0f, 1.0f, 1.0f });

    // インデックスバッファを使用して立方体を描画する
    // 36個のインデックスから12個の三角形を描画する
    g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
}