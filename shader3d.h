/*==============================================================================

   シェーダー [Shader3d.h]
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER3D_H
#define	SHADER3D_H

#include <d3d11.h>
#include <DirectXMath.h>

bool Shader3d_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader3d_Finalize();

void Shader3d_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader3d_SetColor(const DirectX::XMFLOAT4& Color);
void Shader3d_Begin();
void Shader3d_SetMaterialDiffuse(const DirectX::XMFLOAT4& Color);

// 追加：ピクセルシェーダーのディフューズ光を設定する関数
void Shader3d_SetLightDiffuse(const DirectX::XMFLOAT4& diffuseColor, const DirectX::XMFLOAT4& worldDirection);

#endif // SHADER_H
