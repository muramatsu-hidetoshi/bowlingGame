/*==========================================================================
ライトの実装 [light.cpp]

環境光とディフューズライトの設定値を3Dシェーダーへ渡す。
==========================================================================*/

#include "lighth.h"
#include "shader3d.h"
#include <DirectXMath.h>

using namespace DirectX;

// 初期化処理
void Light_Initialize()
{
    // 環境光を設定する。値を大きくすると、全体が明るくなる
    Light_SetAmnient(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

    // ディフューズライトを設定する
    // 白に近い色を使用し、斜め上から照らすような方向を指定する
    Light_SetDiffuse(
        XMFLOAT3(1.0f, 1.0f, 0.95f),
        XMFLOAT3(-0.5f, -1.0f, 0.5f));
}

// 終了処理
void Light_Finalize()
{
    // 現状では解放が必要なリソースはない
}

// 環境光の設定
void Light_SetAmnient(const DirectX::XMFLOAT4& color)
{
    // 環境光の色を3Dシェーダーへ渡す
    Shader3d_SetColor(color);
}

// ディフューズライトの設定
// colorはライトの色、directionはワールド空間でのライトの方向を表す
void Light_SetDiffuse(
    const DirectX::XMFLOAT3& color,
    const DirectX::XMFLOAT3& direction)
{
    // ライトの方向ベクトルをDirectXMathのベクトル形式に変換する
    XMVECTOR dirv = XMLoadFloat3(&direction);

    // 方向ベクトルを正規化し、長さを1にする
    dirv = XMVector3Normalize(dirv);

    // 正規化したベクトルをXMFLOAT3へ戻す
    XMFLOAT3 d3;
    XMStoreFloat3(&d3, dirv);

    // ライトの方向をXMFLOAT4に変換する
    // wには方向ベクトルであることを示す0を設定する
    XMFLOAT4 worldDir(d3.x, d3.y, d3.z, 0.0f);

    // ライトの色をXMFLOAT4に変換する
    XMFLOAT4 diffuseColor(color.x, color.y, color.z, 1.0f);

    // ライトの色と方向を3Dシェーダーへ渡す
    Shader3d_SetLightDiffuse(diffuseColor, worldDir);
}

// 構造体を使ったディフューズライトの設定
void Light_SetDiffuse(const DiffuseLight& light)
{
    // 構造体に格納されている色と方向を3Dシェーダーへ渡す
    Shader3d_SetLightDiffuse(light.color, light.direction);
}
