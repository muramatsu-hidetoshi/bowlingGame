/*==========================================================================
ビルボード描画の実装 [billBord.cpp]

概要：
    Direct3D 11を使用してビルボードを描画する。

主な処理：
    ・頂点バッファと深度ステンシルステートの生成
    ・カメラの回転を利用したビルボードの姿勢計算
    ・UV範囲、描画サイズ、色の設定
    ・描画前後のパイプライン状態の保存と復元

==========================================================================*/

#include "billBord.h"
#include "shaderBillBoard.h"
#include <DirectXMath.h>
#include "direct3d.h"
#include "texture.h"

using namespace DirectX;


//==========================================================================
// ビルボード描画用リソース
//==========================================================================

// ビルボードの頂点バッファ
static ID3D11Buffer* g_pVertexBuffer = nullptr;

// ビルボード用インデックスバッファ
// ※現在の描画処理では使用していない。
static ID3D11Buffer* g_pIndexBuffer = nullptr;

// 深度バッファへの書き込みを無効にするステート
static ID3D11DepthStencilState* g_pDepthNoWrite = nullptr;

// ビルボードを構成する頂点数
static constexpr int NUM_VERTEX = 4;

// カメラのビュー行列
static XMMATRIX g_mtxView{};


//==========================================================================
// 頂点データ
//==========================================================================
struct VertexBillboard
{
    XMFLOAT3 position; // 頂点のローカル座標
    XMFLOAT2 texcoord; // テクスチャ座標
};


// 板ポリゴンを構成する4頂点とUV座標
static VertexBillboard g_BillboardVertex[] =
{
    // 左上
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},

    // 右上
    {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},

    // 左下
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},

    // 右下
    {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
};


//==========================================================================
// 初期化
//==========================================================================
void Billboard_Initialize()
{
    //======================================================================
    // 頂点バッファの生成
    //======================================================================

    D3D11_BUFFER_DESC bd{};

    // CPUから更新可能な頂点バッファとして作成する。
    bd.Usage = D3D11_USAGE_DYNAMIC;

    // 頂点データ全体のサイズを指定する。
    bd.ByteWidth = sizeof(VertexBillboard) * NUM_VERTEX;

    // 頂点バッファとして使用することを指定する。
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // CPUからの書き込みを許可する。
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


    // 初期頂点データを指定する。
    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = g_BillboardVertex;

    // Direct3Dデバイスを使用して頂点バッファを生成する。
    Direct3D_GetDevice()->CreateBuffer(
        &bd,
        &sd,
        &g_pVertexBuffer
    );


    //======================================================================
    // 深度ステンシルステートの生成
    //======================================================================

    D3D11_DEPTH_STENCIL_DESC dsdesc{};

    // 深度テストは有効にする。
    dsdesc.DepthEnable = TRUE;

    // 深度バッファへの書き込みを無効にする。
    dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    // 既に描画された深度値と同じ深度も描画可能にする。
    dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // ステンシルテストは使用しない。
    dsdesc.StencilEnable = FALSE;

    // 深度ステンシルステートを生成する。
    Direct3D_GetDevice()->CreateDepthStencilState(
        &dsdesc,
        &g_pDepthNoWrite
    );
}


//==========================================================================
// 終了処理
//==========================================================================
void Billboard_Finalize()
{
    // 頂点バッファと深度ステンシルステートを解放する。
    SAFE_RELEASE(g_pVertexBuffer);
    SAFE_RELEASE(g_pDepthNoWrite);

    // 現在の実装ではインデックスバッファを生成していないため、
    // g_pIndexBufferも使用していない。
}


//==========================================================================
// 更新処理
//==========================================================================
void Billboard_Update(double elapsedTime)
{
    // 現在は時間経過に応じた更新処理を行わない。
}


//==========================================================================
// ビュー行列の設定
//==========================================================================
void Billboard_SetViewMatrix(const DirectX::XMMATRIX& mtxWorld)
{
    // 呼び出し側から渡された行列を保存する。
    // この行列は描画時のビルボードの向きの計算に使用する。
    g_mtxView = mtxWorld;
}


//==========================================================================
// 描画状態の保存
//==========================================================================
static void SaveCommonPipelineState(
    ID3D11DeviceContext* ctx,
    ID3D11VertexShader** outVS,
    ID3D11PixelShader** outPS,
    ID3D11InputLayout** outIL,
    ID3D11Buffer* outVSCB[3],
    ID3D11Buffer* outPSCB[3],
    ID3D11ShaderResourceView* outSRV[1],
    ID3D11SamplerState* outSamp[1])
{
    // 現在の頂点シェーダーとピクセルシェーダーを取得する。
    *outVS = nullptr;
    *outPS = nullptr;

    ctx->VSGetShader(outVS, nullptr, nullptr);
    ctx->PSGetShader(outPS, nullptr, nullptr);


    // 現在の入力レイアウトを取得する。
    *outIL = nullptr;
    ctx->IAGetInputLayout(outIL);


    // 定数バッファの保存先を初期化する。
    for (int i = 0; i < 3; ++i)
    {
        outVSCB[i] = nullptr;
        outPSCB[i] = nullptr;
    }

    // 頂点シェーダーとピクセルシェーダーの定数バッファを保存する。
    ctx->VSGetConstantBuffers(0, 3, outVSCB);
    ctx->PSGetConstantBuffers(0, 3, outPSCB);


    // ピクセルシェーダーに設定されているテクスチャを保存する。
    outSRV[0] = nullptr;
    ctx->PSGetShaderResources(0, 1, outSRV);


    // サンプラーステートを保存する。
    outSamp[0] = nullptr;
    ctx->PSGetSamplers(0, 1, outSamp);
}


//==========================================================================
// 描画状態の復元
//==========================================================================
static void RestoreCommonPipelineState(
    ID3D11DeviceContext* ctx,
    ID3D11VertexShader* prevVS,
    ID3D11PixelShader* prevPS,
    ID3D11InputLayout* prevIL,
    ID3D11Buffer* prevVSCB[3],
    ID3D11Buffer* prevPSCB[3],
    ID3D11ShaderResourceView* prevSRV[1],
    ID3D11SamplerState* prevSamp[1])
{
    // 描画前のシェーダーと入力レイアウトを復元する。
    ctx->VSSetShader(prevVS, nullptr, 0);
    ctx->PSSetShader(prevPS, nullptr, 0);
    ctx->IASetInputLayout(prevIL);


    // 描画前の定数バッファを復元する。
    ctx->VSSetConstantBuffers(0, 3, prevVSCB);
    ctx->PSSetConstantBuffers(0, 3, prevPSCB);


    // 描画前のテクスチャとサンプラーステートを復元する。
    ctx->PSSetShaderResources(0, 1, prevSRV);
    ctx->PSSetSamplers(0, 1, prevSamp);


    // Get系APIで取得したインターフェースの参照を解放する。
    if (prevVS) prevVS->Release();
    if (prevPS) prevPS->Release();
    if (prevIL) prevIL->Release();

    for (int i = 0; i < 3; ++i)
    {
        if (prevVSCB[i]) prevVSCB[i]->Release();
        if (prevPSCB[i]) prevPSCB[i]->Release();
    }

    if (prevSRV[0]) prevSRV[0]->Release();
    if (prevSamp[0]) prevSamp[0]->Release();
}


//==========================================================================
// 描画処理（テクスチャ全体を使用）
//==========================================================================
void Billboard_Draw(
    int texid,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT2& pivot,
    const DirectX::XMFLOAT4& color)
{
    // テクスチャ全体をUV範囲として指定し、共通の描画処理に渡す。
    Billboard_Draw(
        texid,
        mtxWorld,
        { 0.0f, 0.0f },
        {
            (float)Texture_GetWidht(texid),
            (float)Texture_GetHeight(texid)
        },
        scale,
        color,
        pivot
    );
}


//==========================================================================
// 描画処理（UV範囲指定）
//==========================================================================
void Billboard_Draw(
    int texId,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT2& uv_offset,
    const DirectX::XMFLOAT2& uv_size,
    const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT4& color,
    const DirectX::XMFLOAT2& pivot)
{
    // Direct3Dの描画コンテキストを取得する。
    ID3D11DeviceContext* ctx = Direct3D_GetContext();


    //======================================================================
    // 現在のパイプライン状態を保存
    //======================================================================

    ID3D11VertexShader* prevVS = nullptr;
    ID3D11PixelShader* prevPS = nullptr;
    ID3D11InputLayout* prevIL = nullptr;

    ID3D11Buffer* prevVSCB[3] = { nullptr, nullptr, nullptr };
    ID3D11Buffer* prevPSCB[3] = { nullptr, nullptr, nullptr };

    ID3D11ShaderResourceView* prevSRV[1] = { nullptr };
    ID3D11SamplerState* prevSamp[1] = { nullptr };

    // ビルボード描画によって変更する状態を退避する。
    SaveCommonPipelineState(
        ctx,
        &prevVS,
        &prevPS,
        &prevIL,
        prevVSCB,
        prevPSCB,
        prevSRV,
        prevSamp
    );


    // 深度ステンシルステートとステンシル参照値を保存する。
    ID3D11DepthStencilState* prevDSS = nullptr;
    UINT prevRef = 0;

    ctx->OMGetDepthStencilState(&prevDSS, &prevRef);


    //======================================================================
    // ビルボード用シェーダーの設定
    //======================================================================

    // ビルボード用の描画シェーダーを設定する。
    ShaderBillboard_Begin();


    // ピクセル単位のUV範囲を、テクスチャ全体に対する割合へ変換する。
    float x = uv_offset.x / Texture_GetWidht(texId);
    float y = uv_offset.y / Texture_GetHeight(texId);

    float w = uv_size.x / Texture_GetWidht(texId);
    float h = uv_size.y / Texture_GetHeight(texId);

    // 使用するUV範囲をシェーダーに設定する。
    ShaderBillboard_UV({ x, y }, { w, h });


    // 頂点バッファを入力アセンブラに設定する。
    UINT stride = sizeof(VertexBillboard);
    UINT offset = 0;

    ctx->IASetVertexBuffers(
        0,
        1,
        &g_pVertexBuffer,
        &stride,
        &offset
    );


    //======================================================================
    // ビルボードのワールド変換行列を作成
    //======================================================================

    XMFLOAT4X4 mtxView{};

    // ビュー行列を扱いやすい形式に変換する。
    XMStoreFloat4x4(&mtxView, g_mtxView);

    // 平行移動成分を除去し、カメラの回転成分のみを残す。
    mtxView._41 = mtxView._42 = mtxView._43 = 0.0f;

    XMMATRIX mtxViewRotationOnly = XMLoadFloat4x4(&mtxView);

    // カメラの回転を打ち消すため、逆行列を求める。
    XMMATRIX mtxViewInverse =
        XMMatrixInverse(nullptr, mtxViewRotationOnly);


    // 描画サイズを設定するスケール行列を作成する。
    XMMATRIX mtxScale =
        XMMatrixScaling(scale.x, scale.y, 1.0f);

    // ピボット位置を考慮するための平行移動行列を作成する。
    XMMATRIX mtxPivot =
        XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);


    // サイズ、ピボット、カメラの向き、ワールド位置を組み合わせる。
    XMMATRIX mtxWorldFinal =
        mtxScale * mtxPivot * mtxViewInverse * mtxWorld;

    // 最終的なワールド変換行列をシェーダーに設定する。
    ShaderBillboard_SetWorldMatrix(mtxWorldFinal);


    //======================================================================
    // ビルボードの描画
    //======================================================================

    // 深度テストを維持しながら、深度バッファへの書き込みを無効にする。
    // 透明オブジェクトなどが後続の描画に与える影響を抑える。
    if (g_pDepthNoWrite)
    {
        ctx->OMSetDepthStencilState(g_pDepthNoWrite, prevRef);
    }

    // 4頂点を三角形ストリップとして描画する。
    ctx->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
    );

    // 描画するテクスチャと色を設定する。
    Tecture_SetTexture(texId);
    ShaderBillboard_SetColor(color);

    // ビルボードを描画する。
    ctx->Draw(NUM_VERTEX, 0);


    //======================================================================
    // 描画状態の復元
    //======================================================================

    // 深度ステンシルステートを描画前の状態に戻す。
    ctx->OMSetDepthStencilState(prevDSS, prevRef);

    // 取得した深度ステンシルステートの参照を解放する。
    if (prevDSS)
        prevDSS->Release();

    // シェーダーなどのパイプライン状態を復元する。
    RestoreCommonPipelineState(
        ctx,
        prevVS,
        prevPS,
        prevIL,
        prevVSCB,
        prevPSCB,
        prevSRV,
        prevSamp
    );
}


//==========================================================================
// 描画処理（サイズ・色指定）
//==========================================================================
void Billboard_Draw(
    int texid,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT2& scal,
    const DirectX::XMFLOAT4& color)
{
    // UV範囲をテクスチャ全体に設定し、ピボットを原点として描画する。
    Billboard_Draw(
        texid,
        mtxWorld,
        DirectX::XMFLOAT2(1.0f, 1.0f),
        DirectX::XMFLOAT2(0.0f, 0.0f),
        scal,
        color,
        DirectX::XMFLOAT2(0.0f, 0.0f)
    );
}


//==========================================================================
// 描画処理（色指定）
//==========================================================================
void Billboard_Draw(
    int texid,
    const DirectX::XMMATRIX& mtxWorld,
    const DirectX::XMFLOAT4& color)
{
    // サイズを1に設定し、ピボットを原点として描画する。
    Billboard_Draw(
        texid,
        mtxWorld,
        DirectX::XMFLOAT2(1.0f, 1.0f),
        DirectX::XMFLOAT2(0.0f, 0.0f),
        DirectX::XMFLOAT2(1.0f, 1.0f),
        color,
        DirectX::XMFLOAT2(0.0f, 0.0f)
    );
}
