/*==========================================================================

	デバッグ用のライン描画ユーティリティ。[ debugDraw.h]



---------------------------------------------------------------------------


==========================================================================*/


#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "collision.h" // AABB が定義されているヘッダ

void DebugDraw_Initialize(int maxLines = 8192);
void DebugDraw_Finalize();
void DebugDraw_AddLine(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
void DebugDraw_AddAABB(const AABB& aabb, const DirectX::XMFLOAT4& color = { 1.0f,0.0f,0.0f,1.0f });
void DebugDraw_Draw();