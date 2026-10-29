/*==========================================================================
グリッドの実装 [gruid.cpp]

指定された分割数と間隔から頂点を生成し、Direct3Dの頂点バッファに登録する。
描画時はラインリストを使用して格子状の線を表示する。
==========================================================================*/

#include "grid.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "shader3d.h"
#include "direct3d.h"

using namespace DirectX;

// グリッドの基本設定
static constexpr int GRID_SIZE = 10;        // 標準の分割数
static constexpr float GRID_SPACING = 1.0f; // 標準の格子間隔
static constexpr int NUM_LINES = (GRID_SIZE + 1) * 2;

// グリッド描画に使用する頂点バッファ
static ID3D11Buffer* g_pVertexBuffer = nullptr;

// Direct3Dのデバイスとコンテキスト
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

// 描画する頂点数
static int g_NumVertex{ 0 };

// グリッドの頂点情報
struct Vertex3d
{
    XMFLOAT3 position; // 頂点の座標
    XMFLOAT4 color;    // 頂点の色
    // XMFLOAT2 texcoord; // テクスチャ座標（必要な場合に使用）
};

// 初期化処理
void Gulid_Initialize(int x_count, int z_count, float size)
{
    // Direct3Dのデバイスとコンテキストを取得する
    g_pDevice = Direct3D_GetDevice();
    g_pContext = Direct3D_GetContext();

    // 各方向の分割数に対して、両端の頂点を用意する
    const int X_V_COUNT = x_count + 1;
    const int Z_V_COUNT = z_count + 1;

    // 1本の線につき2頂点を使用するため、必要な頂点数を計算する
    g_NumVertex = X_V_COUNT * 2 + Z_V_COUNT * 2;

    // 頂点バッファの設定
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex3d) * g_NumVertex;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // 頂点データを一時的に格納する配列
    auto* pv = new Vertex3d[g_NumVertex];

    // グリッドの開始位置と終了位置を計算する
    float start_x = x_count * size * -0.5f;
    float start_z = z_count * size * -0.5f;
    float end_x = x_count * size * 0.5f;
    float end_z = z_count * size * 0.5f;

    // 頂点配列に格納する位置
    int Index = 0;

    // X方向に伸びる縦線の頂点を作成する
    for (int x = 0; x < X_V_COUNT; x++)
    {
        // 線の始点
        pv[Index].position = { start_x + size * x, 0.0f, start_z };
        pv[Index].color = { 0.0f, 1.0f, 0.0f, 1.0f };

        // 線の終点
        pv[Index + 1].position = { start_x + size * x, 0.0f, end_z };
        pv[Index + 1].color = { 0.0f, 1.0f, 0.0f, 1.0f };

        // 次の線の格納位置へ進める
        Index += 2;
    }

    // Z方向に伸びる横線の頂点を作成する
    for (int z = 0; z < Z_V_COUNT; z++)
    {
        // 線の始点
        pv[Index].position = { start_x, 0.0f, start_z + size * z };
        pv[Index].color = { 0.5f, 0.5f, 0.5f, 1.0f };

        // 線の終点
        pv[Index + 1].position = { end_x, 0.0f, start_z + size * z };
        pv[Index + 1].color = { 0.5f, 0.5f, 0.5f, 1.0f };

        // 次の線の格納位置へ進める
        Index += 2;
    }

    // 頂点バッファへ登録するデータを指定する
    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = pv;

    // 作成した頂点データをDirect3Dの頂点バッファへ登録する
    g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

    // 一時的に確保した頂点配列を解放する
    delete[] pv;
}

// 終了処理
void Gulid_Finalize()
{
    // 頂点バッファを解放する
    SAFE_RELEASE(g_pVertexBuffer);
}

// 更新処理
void Gulid_Update(double elapsedTime)
{
    // 現状では更新処理はない
    // 将来的にグリッドを移動させる場合などに使用する
}

// 描画処理
void Gulid_Draw()
{
    // 3Dシェーダーを描画用に設定する
    Shader3d_Begin();

    // 頂点バッファの1頂点あたりのサイズと開始位置を設定する
    UINT stride = sizeof(Vertex3d);
    UINT offset = 0;

    // グリッドの頂点バッファを入力アセンブラに設定する
    g_pContext->IASetVertexBuffers(
        0, 1, &g_pVertexBuffer, &stride, &offset);

    // グリッドをワールド座標の原点に配置する
    Shader3d_SetWorldMatrix(XMMatrixIdentity());

    // 頂点を2つずつ組み合わせて線として描画する
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // 登録されている全頂点を描画する
    g_pContext->Draw(g_NumVertex, 0);
}

// 任意の位置・回転・スケールで描画する処理を追加する場合は、
// ワールド行列を設定する処理などを実装する
