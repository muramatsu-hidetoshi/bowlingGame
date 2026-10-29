/*==============================================================================

   ポリゴン描画 [polygon.cpp]
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
#include"DirectXTex.h"
using namespace DirectX;
#include "shader.h"
#include "direct3d.h"
#include "debug_ostream.h"


static constexpr int NUM_VERTEX = 4; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
//2枚3枚使うときはこれを増やす
//一個につき一枚
static ID3D11ShaderResourceView* g_pTexture = nullptr; // テクスチャビュー（必要なら追加）


// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;


// 頂点構造体
struct Vertex
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT4 color;    // 頂点色
	XMFLOAT2 texcoord; // テクスチャ座標（必要なら追加）

};


void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Polygon_Initialize() : 与えられたデバイスかコンテキストが不正です";
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);


}

void Polygon_Finalize(void)
{
	// リソースの解放
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pVertexBuffer);

	// 注意！初期化で外部から設定されるもの。Release不要。

}

void Polygon_Draw(void)
{
	/*
	基本的にはCPUとGPUは互いに独立して動作する。
	v[0].positionはCPU側のメモリ上にある頂点データであり、GPUはそれを読み取って描画する。
	そのため、CPU側で頂点データを更新した後、GPUにそのデータを送る必要がある。



	非同期にCPUとGPUはうごいているためGPUが書いている時にCPUが書き換えると、GPUが読み取るデータが不正になる可能性がある。
	そのため、頂点バッファをロックして、CPUが書き込む間はGPUがそのバッファを読み取らないようにする必要がある。
	ロックを行うことで、CPUが頂点データを書き込んでいる間、GPUはそのバッファを読み取らないようにする。
	ロックを解除すると、GPUはそのバッファを読み取ることができるようになる。
	ロックを行うことで、CPUが頂点データを書き込んでいる間、GPUはそのバッファを読み取らないようにする。
	ロックをしている間にCPUが頂点データを書き込む
	お互いが非同期にうごけるのにロックして歩調を合わせているためこれは低速で効率が悪い




	*/


	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
	/*v[0].position = { 400.0f, 500.0f, 0.0f };
	v[1].position = { 300.0f, 500.0f, 0.0f };
	v[2].position = { 300.0f, 400.0f, 0.0f };
	v[3].position = { 200.0f,400.0f, 0.0f };*/

	static  float b = 0.0f; // 角度をラジアンで指定
	constexpr float W = 512.0f ;
	constexpr float H = 512.0f ;

	float x = 64.0f;
	float y = 64.0f;

	v[0].position = { x, y, 0.0f };
	v[1].position = { x+W, y, 0.0f };
	v[2].position = { x, y+H, 0.0f };
	v[3].position = { x+W, y+H, 0.0f };




	/*v[0].position = { 100.0f, 10.0f, 0.0f };
	v[1].position = { 1500.0f, 10.0f, 0.0f };
	v[2].position = { 100.0f,800.0f, 0.0f };
	v[3].position = { 1500.0f, 800.0f, 0.0f };*/




	//// 頂点色を設定
	//v[0].color = { 1.0f, 0.0f, 0.0f, 1.0f }; // 赤
	//v[1].color = { 0.0f, 1.0f, 0.0f, 1.0f }; // 緑
	//v[2].color = { 0.0f, 0.0f, 1.0f, 1.0f }; // 青
	//v[3].color = { 1.0f, 1.0f, 1.0f, 1.0f }; // white
	//


	// 頂点色を設定
	v[0].color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 赤
	v[1].color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 緑
	v[2].color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 青
	v[3].color = { 1.0f, 1.0f, 1.0f, 1.0f }; // white

	// テクスチャ座標を設定
	v[0].texcoord = { 0.0f+b, 0.0f+b }; // テクスチャ座標
	v[1].texcoord = { 4.0f+b, 0.0f+b }; // テクスチャ座標
	v[2].texcoord = { 0.0f+b, 4.0f+b }; // テクスチャ座標
	v[3].texcoord = { 4.0f+b, 4.0f+b }; // テクスチャ座標


	//b+= 0.001f;

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	/*

	一番左の座標を右の座標上の座標下の座標はどこか設定
	一番遠いZ座標はどこか一番近いZ座標はどこか設定

	下のやつを使えば画面を揺らせる
	一律に同じ値をかけたり引いたりすれば画面を揺らせる



	*/
	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	static float a = 0.5f;

	Shader_SetColor({1.0f,1.0f,0.0f,a}); // 色を白に設定
	a = std::min(a + 0.005f,1.0f);
	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);






	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);
}
