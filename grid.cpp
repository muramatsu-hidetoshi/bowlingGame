/*==========================================================================
グリッドの表示[gruid.cpp]



---------------------------------------------------------------------------






=========================================================================*/


#include "grid.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "shader3d.h"
#include "direct3d.h"


using namespace DirectX;

// ------------------------------------------------------
// グリッドの定義
// ------------------------------------------------------
static constexpr int GRID_SIZE = 10;          // 10x10
static constexpr float GRID_SPACING = 1.0f;   // 格子の間隔
static constexpr int NUM_LINES = (GRID_SIZE + 1) * 2;
//static constexpr int NUM_VERTEX = NUM_LINES * 2; // 1線2頂点
static ID3D11Buffer* g_pVertexBuffer = nullptr;
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;
static int g_NumVertex{0};
// 頂点構造体
struct Vertex3d
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;    // 頂点色
	//XMFLOAT2 texcoord; // テクスチャ座標（必要なら追加）

};
// ------------------------------------------------------
// 初期化
// ------------------------------------------------------
void Gulid_Initialize(int x_count,int z_count,float size)
{
    // 指定した分割数と間隔からグリッドの頂点を作成して登録する。

	g_pDevice = Direct3D_GetDevice();
	g_pContext = Direct3D_GetContext();

	const int X_V_COUNT = x_count +1;
	const int Z_V_COUNT = z_count + 1;
	g_NumVertex = X_V_COUNT*2 + Z_V_COUNT*2;


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(Vertex3d) * g_NumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	auto* pv = new Vertex3d[g_NumVertex];

	float start_x = x_count * size *-0.5f;
	float start_z = z_count * size * -0.5f;
	float end_x = x_count * size * 0.5f;
	float end_z = z_count * size * 0.5f;
	int Index = 0;
	for (int x = 0; x < X_V_COUNT; x++)
	{
		pv[Index].position = { start_x + size * x,0.0f,start_z };
		pv[Index].color = { 0.0f,1.0f,0.0f,1.0f };
		pv[Index+1].position = { start_x + size * x,0.0f,end_z };
		pv[Index+1].color = { 0.0f,1.0f,0.0f,1.0f };
		Index += 2;

	}
	for (int z = 0; z < Z_V_COUNT; z++) {
		pv[Index].position = { start_x,0.0f,start_z + size * z };
		pv[Index].color = { 0.5f,0.5f,0.5f,1.0f };
		pv[Index + 1].position = { end_x,0.0f,start_z + size * z };
		pv[Index + 1].color = { 0.5f,0.5f,0.5f,1.0f };

		Index += 2;
	}


	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem =pv;

	//頂点バッファへ流し込むデータの設計

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
	delete[] pv;

}

// ------------------------------------------------------
// 終了処理
// ------------------------------------------------------
void Gulid_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
}

// ------------------------------------------------------
// 更新（現状なし）
// ------------------------------------------------------
void Gulid_Update(double elapsedTime)
{
	// 今後グリッド移動などをしたい場合に使用
}

// ------------------------------------------------------
// 描画（デフォルト位置）
// ------------------------------------------------------
void Gulid_Draw()
{
	//Direct3D_SetDepthTest(true);
	Shader3d_Begin();
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	Shader3d_SetWorldMatrix(XMMatrixIdentity());


	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	g_pContext->Draw(g_NumVertex, 0);
}




// ------------------------------------------------------
// 描画（任意位置/回転/スケール）
// ------------------------------------------------------
