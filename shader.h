/*==============================================================================
    2D描画用シェーダーの関数宣言 [shader.h]

    シェーダーの初期化・終了処理と、
    描画に必要な行列や色の設定を行う関数を宣言する。
==============================================================================*/

#ifndef SHADER_H
#define SHADER_H

#include <d3d11.h>
#include <DirectXMath.h>

// シェーダー関連リソースを初期化・解放する
bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader_Finalize();

// 描画に使用する行列と色を設定する
void Shader_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader_SetColor(const DirectX::XMFLOAT4& Color);

// 描画に使用するシェーダーや設定をパイプラインへ登録する
void Shader_Begin();

#endif // SHADER_H