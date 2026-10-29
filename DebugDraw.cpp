/*==========================================================================
    デバッグ用ライン描画ユーティリティの実装 [DebugDraw.cpp]

    【ファイルの役割】
    デバッグ用の線分を登録し、Direct3Dを使用して描画する。

    AABBの最小座標と最大座標から8つの頂点を求め、
    それらを結ぶ12本の線分を描画することで、
    当たり判定範囲をワイヤーフレームとして可視化する。

    【主な処理】
    ・描画用頂点バッファの作成
    ・線分の頂点データの登録
    ・AABBの頂点と辺の計算
    ・GPUへの頂点データ転送
    ・ラインリスト形式での描画
==========================================================================*/

#include "debugDraw.h"
#include "direct3d.h"
#include "shader3d.h"
#include <cstring>

using namespace DirectX;


// デバッグ描画に使用する頂点データ。
// 各頂点は座標と色を持つ。
struct DDVertex
{
    XMFLOAT3 position; // 頂点の座標
    XMFLOAT4 color;    // 頂点の色
};


// デバッグ描画用の頂点バッファ
static ID3D11Buffer* g_pVB = nullptr;

// Direct3Dのデバイス
// Direct3D側で管理されているオブジェクトを参照する
static ID3D11Device* g_pDevice = nullptr;

// Direct3Dのデバイスコンテキスト
// 描画命令やGPUリソースへの操作に使用する
static ID3D11DeviceContext* g_pContext = nullptr;

// 描画する線分の頂点データを一時的に保持する配列
// 1本の線分につき始点と終点の2頂点を使用する
static std::vector<DDVertex> g_vertices;

// 登録できる頂点数の上限
// maxLinesで指定された線分数の2倍を保持する
static int g_maxVertices = 0;


// デバッグ描画に必要なリソースを初期化する
void DebugDraw_Initialize(int maxLines)
{
    // Direct3Dが管理するデバイスとコンテキストを取得する
    g_pDevice = Direct3D_GetDevice();
    g_pContext = Direct3D_GetContext();

    // 1本の線分につき2頂点を使用するため、
    // 最大線分数から必要な頂点数を計算する
    g_maxVertices = maxLines * 2;

    // 頂点データを初期化する
    g_vertices.clear();

    // 最大頂点数分のメモリをあらかじめ確保する
    // 描画のたびにメモリを確保する処理を減らす
    g_vertices.reserve(g_maxVertices);

    // 頂点バッファの作成に必要な設定を初期化する
    D3D11_BUFFER_DESC bd{};

    // CPUから書き込み可能な動的バッファとして作成する
    bd.Usage = D3D11_USAGE_DYNAMIC;

    // 頂点データ全体を格納できるバッファサイズを指定する
    bd.ByteWidth = sizeof(DDVertex) * g_maxVertices;

    // 頂点バッファとして使用する
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // CPUからの書き込みを許可する
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // すでにバッファが存在する場合は解放する
    if (g_pVB)
    {
        g_pVB->Release();
        g_pVB = nullptr;
    }

    // 頂点データを転送するためのバッファを作成する
    // 初期データは後から設定するため、ここではnullptrを指定する
    g_pDevice->CreateBuffer(&bd, nullptr, &g_pVB);
}


// デバッグ描画に使用したリソースを解放する
void DebugDraw_Finalize()
{
    // 頂点バッファを解放する
    if (g_pVB)
    {
        g_pVB->Release();
        g_pVB = nullptr;
    }

    // 登録されている頂点データを破棄する
    g_vertices.clear();

    // 頂点数の上限を初期状態に戻す
    g_maxVertices = 0;

    // デバイスとコンテキスト自体は外部で管理されているため、
    // この関数では解放しない
    g_pDevice = nullptr;
    g_pContext = nullptr;
}


// 指定した2点を結ぶ線分を登録する
void DebugDraw_AddLine(
    const XMFLOAT3& a,
    const XMFLOAT3& b,
    const XMFLOAT4& color)
{
    // 始点と終点の2頂点を追加すると容量を超える場合は登録しない
    if (static_cast<int>(g_vertices.size()) + 2 > g_maxVertices)
    {
        return;
    }

    // 線分の始点を登録する
    g_vertices.push_back(DDVertex{ a, color });

    // 線分の終点を登録する
    g_vertices.push_back(DDVertex{ b, color });
}


// AABBのワイヤーフレームを線分として登録する
void DebugDraw_AddAABB(const AABB& aabb, const XMFLOAT4& color)
{
    // AABBの最小座標と最大座標を取得する
    const XMFLOAT3& mn = aabb.GetMin();
    const XMFLOAT3& mx = aabb.GetMax();

    // 最小座標と最大座標から、箱の8つの頂点を計算する
    XMFLOAT3 v[8] = {
        { mn.x, mn.y, mn.z }, // 0：手前側の左下
        { mx.x, mn.y, mn.z }, // 1：手前側の右下
        { mx.x, mx.y, mn.z }, // 2：手前側の右上
        { mn.x, mx.y, mn.z }, // 3：手前側の左上
        { mn.x, mn.y, mx.z }, // 4：奥側の左下
        { mx.x, mn.y, mx.z }, // 5：奥側の右下
        { mx.x, mx.y, mx.z }, // 6：奥側の右上
        { mn.x, mx.y, mx.z }  // 7：奥側の左上
    };

    // 8つの頂点を結ぶ12本の辺を定義する
    // 各要素は、線分の始点と終点に対応する頂点番号を表す
    const int edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, // 手前側の面
        {4,5}, {5,6}, {6,7}, {7,4}, // 奥側の面
        {0,4}, {1,5}, {2,6}, {3,7}  // 手前側と奥側を結ぶ辺
    };

    // 12本の辺を線分として登録する
    for (int i = 0; i < 12; ++i)
    {
        DebugDraw_AddLine(
            v[edges[i][0]],
            v[edges[i][1]],
            color
        );
    }
}


// 登録されている線分をまとめて描画する
void DebugDraw_Draw()
{
    // 描画する頂点がない場合や、描画に必要なリソースが
    // 初期化されていない場合は処理を終了する
    if (g_vertices.empty() || !g_pVB || !g_pContext)
    {
        return;
    }

    // GPUの頂点バッファをCPUから書き換えられる状態にする
    D3D11_MAPPED_SUBRESOURCE ms{};

    if (SUCCEEDED(g_pContext->Map(
        g_pVB,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &ms)))
    {
        // 登録した頂点データをGPUの頂点バッファにコピーする
        // WRITE_DISCARDにより、以前のバッファ内容を破棄して書き込む
        std::memcpy(
            ms.pData,
            g_vertices.data(),
            sizeof(DDVertex) * g_vertices.size()
        );

        // 頂点バッファへの書き込みを終了する
        g_pContext->Unmap(g_pVB, 0);
    }

    // 描画に使用するシェーダーを設定する
    Shader3d_Begin();

    // 頂点データ1個分のサイズと開始位置を設定する
    UINT stride = sizeof(DDVertex);
    UINT offset = 0;

    // 描画パイプラインに頂点バッファを設定する
    g_pContext->IASetVertexBuffers(
        0,
        1,
        &g_pVB,
        &stride,
        &offset
    );

    // デバッグ線分はワールド変換を行わず、
    // 登録された座標をそのまま使用する
    Shader3d_SetWorldMatrix(XMMatrixIdentity());

    // 頂点を2個ずつ組み合わせて線分として描画する
    g_pContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST
    );

    // 登録されている頂点数分の描画命令を発行する
    g_pContext->Draw(
        static_cast<UINT>(g_vertices.size()),
        0
    );

    // 描画済みの頂点データを消去する
    // 次のフレームで登録された線分を描画できるようにする
    g_vertices.clear();
}