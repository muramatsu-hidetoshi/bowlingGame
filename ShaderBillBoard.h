/*==========================================================================

ビルボードシェーダ[shaderBillBoard.h]


---------------------------------------------------------------------------






=========================================================================*/


#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

bool ShaderBillboard_Initialize();
void ShaderBillboard_Finalize();
void ShaderBillboard_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void ShaderBillboard_SetColor(const DirectX::XMFLOAT4& Color);
void ShaderBillboard_Begin();
void ShaderBillboard_SetColor(const DirectX::XMFLOAT4& Color);
void ShaderBillboard_UV(const DirectX::XMFLOAT2& offset, const DirectX::XMFLOAT2& size);

