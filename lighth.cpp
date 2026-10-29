/*==========================================================================
ライトの実装[light.cpp]



---------------------------------------------------------------------------






=========================================================================*/

#include "lighth.h"
#include "shader3d.h"
#include <DirectXMath.h>

using namespace DirectX;

// 初期化：デフォルトの ambient / diffuse をシェーダへ設定
void Light_Initialize()
{
    // 3Dシェーダーで使うライト定数バッファを作成する。
    // 適度なアンビエント（暗い場合はここを上げる）
    Light_SetAmnient(XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));

    // ディフューズ方向光のデフォルト（上方から斜め下）
    Light_SetDiffuse(XMFLOAT3(1.0f, 1.0f, 0.95f), XMFLOAT3(-0.5f, -1.0f, 0.5f));
}

void Light_Finalize()
{
    // 今はリソース無し。将来必要なら解放処理を追加。
}

// アンビエント設定（シェーダの b0 に相当）
void Light_SetAmnient(const DirectX::XMFLOAT4& color)
{
    // Shader3d_SetColor は PixelShader の b0（ambient_color）へ書き込む実装に合わせる
    Shader3d_SetColor(color);
}

// ディフューズ設定（色 + 方向）
// direction はワールド空間のベクトル（正規化して渡す）
void Light_SetDiffuse(const DirectX::XMFLOAT3& color, const DirectX::XMFLOAT3& direction)
{
    // 正規化
    XMVECTOR dirv = XMLoadFloat3(&direction);
    dirv = XMVector3Normalize(dirv);
    XMFLOAT3 d3;
    XMStoreFloat3(&d3, dirv);
    XMFLOAT4 worldDir(d3.x, d3.y, d3.z, 0.0f); // w は未使用なので 0

    XMFLOAT4 diffuseColor(color.x, color.y, color.z, 1.0f);
    Shader3d_SetLightDiffuse(diffuseColor, worldDir);
}

void Light_SetDiffuse(const DiffuseLight& light)
{
    Shader3d_SetLightDiffuse(light.color, light.direction);
}
