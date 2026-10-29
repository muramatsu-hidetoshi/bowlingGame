
/*==========================================================================
トレイルの描画や計算など [trail.cpp]

過去の位置情報を利用して、移動する物体の軌跡をポリゴンとして描画する。
==========================================================================*/

#include "trail.h"
#include <d3d11.h>
#include "direct3d.h"
#include "shaderBillBoard.h"

using namespace DirectX;

// トレイルに保存する位置情報の最大数
static constexpr int POSITION_COUNT_MAX = 50;

// トレイルの中心線から左右に広げる幅
static constexpr float POLIGON_WIDTH = 0.1f;

// トレイル描画用の頂点バッファ
static ID3D11Buffer* g_pVertexBuffer = NULL;

// 現在使用しているテクスチャID
static int g_Texture = -1;

// トレイルの位置履歴
static XMFLOAT3 g_PositionBuffer[POSITION_COUNT_MAX]{};

// 描画時に使用するカメラ位置
static XMFLOAT3 g_CameraPosition{};

// トレイルの色を徐々に変化させるかどうか
static bool g_FadeEnabled = true;

// トレイルの開始色と終了色
static XMFLOAT4 g_StartColor = { 1.0f, 0.5f, 0.0f, 1.0f };
static XMFLOAT4 g_EndColor = { 0.0f, 0.5f, 1.0f, 0.0f };

// トレイルを描画するかどうか
static bool g_IsEnabled = true;

// トレイル描画に使用する頂点データ
struct VertexTrail
{
    XMFLOAT3 position; // 頂点の位置
    XMFLOAT2 texcoord; // テクスチャ座標
    XMFLOAT4 color;    // 頂点カラー
};

// トレイル描画に必要な頂点バッファを作成する
void Trail_Initialize()
{
    D3D11_BUFFER_DESC bd = {};

    // CPUから毎フレーム更新する頂点バッファとして設定する
    bd.Usage = D3D11_USAGE_DYNAMIC;

    // 左右2列分の頂点データを格納できるサイズを確保する
    bd.ByteWidth = sizeof(VertexTrail) * POSITION_COUNT_MAX * 2;

    // 頂点バッファとして使用する
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // CPUから書き込み可能にする
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // 頂点バッファを作成する
    Direct3D_GetDevice()->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
}

// トレイル描画用の頂点バッファを解放する
void Trail_Finalize()
{
    SAFE_RELEASE(g_pVertexBuffer);
}

// 保存した位置履歴からトレイルを描画する
void Trail_Draw()
{
    // 描画が無効なら処理を終了する
    if (!g_IsEnabled)
        return;

    // 頂点バッファをCPUから書き換えられる状態にする
    D3D11_MAPPED_SUBRESOURCE msr;

    Direct3D_GetContext()->Map(
        g_pVertexBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &msr
    );

    // 頂点バッファのメモリを頂点構造体として扱う
    VertexTrail* v = (VertexTrail*)msr.pData;

    // 隣り合う位置を結び、トレイルの幅を持つ頂点を作成する
    for (int i = 0; i < POSITION_COUNT_MAX - 1; i++)
    {
        // トレイルの位置からカメラへ向かうベクトルを求める
        XMVECTOR vecEye =
            XMLoadFloat3(&g_PositionBuffer[i]) -
            XMLoadFloat3(&g_CameraPosition);

        // 隣り合う位置からトレイルの進行方向を求める
        XMVECTOR vecTrail =
            XMLoadFloat3(&g_PositionBuffer[i]) -
            XMLoadFloat3(&g_PositionBuffer[i + 1]);

        // 外積を使ってトレイルの幅方向を求める
        XMVECTOR vecCross = XMVector3Cross(vecEye, vecTrail);

        // 幅方向のベクトルを正規化する
        vecCross = XMVector3Normalize(vecCross);

        // 位置履歴に応じて色を変化させるための割合を求める
        // iが小さいほど先頭側、値が大きいほど末尾側になる
        float t = (float)i / (POSITION_COUNT_MAX - 1);

        XMFLOAT4 color;

        if (g_FadeEnabled)
        {
            // 開始色から終了色へ、割合tに応じて色を補間する
            color.x = g_StartColor.x * (1 - t) + g_EndColor.x * t;
            color.y = g_StartColor.y * (1 - t) + g_EndColor.y * t;
            color.z = g_StartColor.z * (1 - t) + g_EndColor.z * t;
            color.w = g_StartColor.w * (1 - t) + g_EndColor.w * t;
        }
        else
        {
            // フェードを使わない場合は開始色で統一する
            color = g_StartColor;
        }

        // 中心位置から幅方向へ移動させ、片側の頂点を作る
        XMStoreFloat3(
            &v[i * 2].position,
            XMLoadFloat3(&g_PositionBuffer[i]) + vecCross * POLIGON_WIDTH
        );

        v[i * 2].texcoord = XMFLOAT2(t, 0.0f);
        v[i * 2].color = color;

        // 中心位置から反対側へ移動させ、もう片側の頂点を作る
        XMStoreFloat3(
            &v[i * 2 + 1].position,
            XMLoadFloat3(&g_PositionBuffer[i]) - vecCross * POLIGON_WIDTH
        );

        v[i * 2 + 1].texcoord = XMFLOAT2(t, 1.0f);
        v[i * 2 + 1].color = color;
    }

    // 最後の位置に対応する頂点を設定する
    v[POSITION_COUNT_MAX * 2 - 2].position =
        g_PositionBuffer[POSITION_COUNT_MAX - 1];

    v[POSITION_COUNT_MAX * 2 - 2].texcoord = XMFLOAT2(1.0f, 0.0f);
    v[POSITION_COUNT_MAX * 2 - 2].color = g_EndColor;

    v[POSITION_COUNT_MAX * 2 - 1].position =
        g_PositionBuffer[POSITION_COUNT_MAX - 1];

    v[POSITION_COUNT_MAX * 2 - 1].texcoord = XMFLOAT2(1.0f, 1.0f);
    v[POSITION_COUNT_MAX * 2 - 1].color = g_EndColor;

    // CPUからの頂点データ更新を終了する
    Direct3D_GetContext()->Unmap(g_pVertexBuffer, 0);

    // ビルボード用シェーダーを開始し、UVを設定する
    ShaderBillboard_Begin();
    ShaderBillboard_UV({ 0.0f, 0.0f }, { 1.f, 1.f });

    // 頂点バッファを描画パイプラインへ設定する
    UINT stride = sizeof(VertexTrail);
    UINT offset = 0;

    Direct3D_GetContext()->IASetVertexBuffers(
        0,
        1,
        &g_pVertexBuffer,
        &stride,
        &offset
    );

    // ワールド行列を単位行列に設定する
    ShaderBillboard_SetWorldMatrix(XMMatrixIdentity());

    // 隣り合う頂点を帯状につないで描画する
    Direct3D_GetContext()->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
    );

    // 頂点カラーを使用するため、シェーダー側の色は白に設定する
    ShaderBillboard_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

    // 頂点データを使ってトレイルを描画する
    Direct3D_GetContext()->Draw(POSITION_COUNT_MAX * 2, 0);
}

// 新しい位置を履歴へ追加する
void Trail_AddPosition(const DirectX::XMFLOAT3& position)
{
    // 古い位置を前へずらして、最新の位置を追加できるようにする
    for (int i = 0; i < POSITION_COUNT_MAX - 1; i++)
    {
        g_PositionBuffer[i] = g_PositionBuffer[i + 1];
    }

    // 最新位置を配列の最後に保存する
    g_PositionBuffer[POSITION_COUNT_MAX - 1] = position;
}

// 描画に使用するカメラ位置を設定する
void Trail_SetCameraPosition(const DirectX::XMFLOAT3& position)
{
    g_CameraPosition = position;
}

// トレイルの色を徐々に変化させるかどうかを設定する
void Trail_SetFadeEnabled(bool enabled)
{
    g_FadeEnabled = enabled;
}

// トレイルの開始色と終了色を設定する
void Trail_SetColor(
    const DirectX::XMFLOAT4& startColor,
    const DirectX::XMFLOAT4& endColor
)
{
    g_StartColor = startColor;
    g_EndColor = endColor;
}

// トレイルを描画するかどうかを設定する
void Trail_SetEnabled(bool enabled)
{
    g_IsEnabled = enabled;
}

// 位置履歴を初期化する
void Trail_Clear()
{
    // すべての位置を原点に戻し、以前の軌跡を消去する
    for (int i = 0; i < POSITION_COUNT_MAX; i++)
    {
        g_PositionBuffer[i] = XMFLOAT3(0, 0, 0);
    }
}
