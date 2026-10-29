
/*==========================================================================
ライトの管理 [light.h]

3D描画で使用する環境光とディフューズライトの設定関数を宣言する。
==========================================================================*/

#pragma once
#include <DirectXMath.h>

// ディフューズライトの色と方向
struct DiffuseLight
{
    DirectX::XMFLOAT4 color;     // ライトの色
    DirectX::XMFLOAT4 direction; // ライトの方向
};

// ライトの初期化
void Light_Initialize();

// ライトの終了処理
void Light_Finalize();

// 環境光の色を設定する
void Light_SetAmnient(const DirectX::XMFLOAT4& color);

// ディフューズライトの色と方向を設定する
void Light_SetDiffuse(
    const DirectX::XMFLOAT3& color,
    const DirectX::XMFLOAT3& direction);

// ディフューズライトの構造体を使って設定する
void Light_SetDiffuse(const DiffuseLight& light);


