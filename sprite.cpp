/*==========================================================================

スプライト描画[sprite.cpp]

---------------------------------------------------------------------------






=========================================================================*/

#include <d3d11.h>
#include <DirectXMath.h>
#include"DirectXTex.h"
using namespace DirectX;
#include "sprite.h"
#include "direct3d.h"
#include "debug_ostream.h"
#include "shader.h"
#include "texture.h"
static constexpr int NUM_VERTEX = 4; // 頂点数


static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
//2枚3枚使うときはこれを増やす
//一個につき一枚
//static ID3D11ShaderResourceView* g_pTexture = nullptr; // テクスチャビュー（必要なら追加）


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


void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // スプライト描画に使う頂点・インデックスバッファを作成する。
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

void Sprite_Finalize(void)
{
    // スプライト描画用のDirect3Dリソースを解放する。
	// リソースの解放

	SAFE_RELEASE(g_pVertexBuffer);

}

void Sprite_Draw(int texid, float x, float y, float w, float h, float scrollX, float scrollY, DirectX::XMFLOAT4 color)
{
    // 表示位置とサイズ、UV範囲を指定してスプライトを描画する。
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


	static  float b = 0.0f; // 角度をラジアンで指定


	x += w / 2;
	y += h / 2;


	v[0].position = { -w / 2,  -h / 2, 0.0f };
	v[1].position = { w / 2, -h / 2 ,0.0f, };
	v[2].position = { - w/ 2,h/ 2,  0.0f };
	v[3].position = { w/ 2, h/ 2,  0.0f };


	// 頂点色を設定
	v[0].color = color; // 赤
	v[1].color = color; // 緑
	v[2].color = color; // 青
	v[3].color = color; // white

	float tsw = (float)Texture_GetWidht(texid); // テクスチャの幅を取得
	//float tsh = (float)Texture_GetHeight(texid); // テクスチャの高さを取得


	float u0 = scrollX;
	float v0 = scrollY;
	float u1 = scrollX + (w / tsw);  // 横サイズ分
	float v1 = 1.0; // 縦サイズ分

	v[0].texcoord = { u0,v0 };
	v[1].texcoord = { u1,v0 };
	v[2].texcoord = { u0,v1 };
	v[3].texcoord = { u1,v1 };




	//b += 0.001f;

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// 頂点シェーダーに変換行列を設定
	Shader_SetWorldMatrix(XMMatrixIdentity());


	/*

	一番左の座標を右の座標上の座標下の座標はどこか設定
	一番遠いZ座標はどこか一番近いZ座標はどこか設定

	下のやつを使えば画面を揺らせる
	一律に同じ値をかけたり引いたりすれば画面を揺らせる



	*/

	Shader_SetWorldMatrix(XMMatrixIdentity());
	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	static float a = 0.5f;

	Shader_SetColor({ 1.0f,1.0f,1.0f,1.0f }); // 色を白に設定
	a = std::min(a + 0.005f, 1.0f);
	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	Tecture_SetTexture(texid); // テクスチャの設定


	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);







}

void Sprite_Draw(int texid, float x, float y, float width, float height, int tx, int ty, int tw, int th, DirectX::XMFLOAT4 color)
{


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


	/*float x = 64.0f;
	float y = 64.0f;*/

	v[0].position = { x, y, 0.0f };
	v[1].position = { x + width, y, 0.0f };
	v[2].position = { x, y + height, 0.0f };
	v[3].position = { x + width, y + height, 0.0f };





	// 頂点色を設定
	v[0].color = color; // 赤
	v[1].color = color; // 緑
	v[2].color = color; // 青
	v[3].color = color; // white

	float tsw = (float)Texture_GetWidht(texid); // テクスチャの幅を取得
	float tsh = (float)Texture_GetHeight(texid); // テクスチャの高さを取得


	float u0 = tx / tsw;
	float v0 = ty / tsh;
	float u1 = (tx + tw) / tsw;
	float v1 = (ty + th) / tsh;

	v[0].texcoord = { u0,v0 };
	v[1].texcoord = { u1,v0 };
	v[2].texcoord = { u0,v1 };
	v[3].texcoord = { u1,v1 };

	//// テクスチャ座標を設定
	//v[0].texcoord = { 0.0f + b, 0.189f + b }; // テクスチャ座標
	//v[1].texcoord = { 0.065f + b, 0.189f + b }; // テクスチャ座標
	//v[2].texcoord = { 0.0f + b, 0.25f + b }; // テクスチャ座標
	//v[3].texcoord = { 0.065f + b, 0.25f + b }; // テクスチャ座標


	//// テクスチャ座標を設定
	//v[0].texcoord = { 0.0f + b, 0.0f + b }; // テクスチャ座標
	//v[1].texcoord = { 1.0f + b, 0.0f + b }; // テクスチャ座標
	//v[2].texcoord = { 0.0f + b, 1.0f + b }; // テクスチャ座標
	//v[3].texcoord = { 1.0f + b, 1.0f + b }; // テクスチャ座標


	//b += 0.001f;

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// 頂点シェーダーに変換行列を設定
	Shader_SetWorldMatrix(XMMatrixIdentity());


	/*

	一番左の座標を右の座標上の座標下の座標はどこか設定
	一番遠いZ座標はどこか一番近いZ座標はどこか設定

	下のやつを使えば画面を揺らせる
	一律に同じ値をかけたり引いたりすれば画面を揺らせる



	*/

	Shader_SetWorldMatrix(XMMatrixIdentity());
	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	static float a = 0.5f;

	Shader_SetColor(color);

	a = std::min(a + 0.005f, 1.0f);
	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	Tecture_SetTexture(texid); // テクスチャの設定


	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);





}

void Sprite_Draw(int texid, float angle, float x, float y, float width, float height, int tx, int ty, int tw, int th, DirectX::XMFLOAT4 color)
{



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


	static  float b = 0.0f; // 角度をラジアンで指定


	/*float x = 64.0f;
	float y = 64.0f;*/

	v[0].position = { -0.5f, -0.5f, 0.0f };
	v[1].position = { +0.5f, -0.5f, 0.0f };
	v[2].position = { -0.5f, +0.5f, 0.0f };
	v[3].position = { +0.5f, +0.5f, 0.0f };


	/*v[0].position = { x, y, 0.0f };
	v[1].position = { x + width, y, 0.0f };
	v[2].position = { x, y + height, 0.0f };
	v[3].position = { x + width, y + height, 0.0f };*/




	// 頂点色を設定
	v[0].color = color; // 赤
	v[1].color = color; // 緑
	v[2].color = color; // 青
	v[3].color = color; // white

	float tsw = (float)Texture_GetWidht(texid); // テクスチャの幅を取得
	float tsh = (float)Texture_GetHeight(texid); // テクスチャの高さを取得


	float u0 = tx / tsw;
	float v0 = ty / tsh;
	float u1 = (tx + tw) / tsw;
	float v1 = (ty + th) / tsh;

	v[0].texcoord = { u0,v0 };
	v[1].texcoord = { u1,v0 };
	v[2].texcoord = { u0,v1 };
	v[3].texcoord = { u1,v1 };



	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);


	// 頂点シェーダーにworld変換行列を設定

	//演算するなら
	XMMATRIX s = XMMatrixScaling(width, height, 1.0f); // スケーリング行列を生成
	XMMATRIX r = XMMatrixRotationZ(angle); // Z軸周りの回転行列を生成
	XMMATRIX t = XMMatrixTranslation(x, y, 0.0f); // 平行移動行列を生成

	Shader_SetWorldMatrix(s* r *t);



	//演算しないならXMMATRIX4x4
	//Shader_SetWorldMatrix(XMMatrixIdentity());




	//Shader_SetWorldMatrix(XMMatrixIdentity());
	// 頂点シェーダーに変換行列を設定
	Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

	static float a = 0.5f;

	Shader_SetColor(color);

	a = std::min(a + 0.005f, 1.0f);
	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	Tecture_SetTexture(texid); // テクスチャの設定


	// ポリゴン描画命令発行
	g_pContext->Draw(NUM_VERTEX, 0);






}




	void Sprite_Draw(int texid, float x, float y, float w, float h, DirectX::XMFLOAT4 color)
	{





		Shader_Begin();

		D3D11_MAPPED_SUBRESOURCE msr;
		g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		Vertex* v = (Vertex*)msr.pData;

		const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
		const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

		// 頂点ポジション（そのままのサイズで描画）
		v[0].position = { x,     y,     0.0f };
		v[1].position = { x + w, y,     0.0f };
		v[2].position = { x,     y + h, 0.0f };
		v[3].position = { x + w, y + h, 0.0f };

		// 色設定
		v[0].color = color;
		v[1].color = color;
		v[2].color = color;
		v[3].color = color;

		// テクスチャ全体を使う（スクロールなし）
		v[0].texcoord = { 0.0f, 0.0f };
		v[1].texcoord = { 1.0f, 0.0f };
		v[2].texcoord = { 0.0f, 1.0f };
		v[3].texcoord = { 1.0f, 1.0f };

		g_pContext->Unmap(g_pVertexBuffer, 0);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

		// 行列設定
		Shader_SetWorldMatrix(XMMatrixIdentity());
		Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(
			0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

		Shader_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // α変更するならここ

		g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		Tecture_SetTexture(texid);
		g_pContext->Draw(NUM_VERTEX, 0);
	}
	void Sprite_DrawCenter(int texid, float cx, float cy, float w, float h, DirectX::XMFLOAT4 color)
	{
		Shader_Begin();

		D3D11_MAPPED_SUBRESOURCE msr;
		g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		Vertex* v = (Vertex*)msr.pData;

		float halfW = w * 0.5f;
		float halfH = h * 0.5f;

		v[0].position = { cx - halfW, cy - halfH, 0.0f };
		v[1].position = { cx + halfW, cy - halfH, 0.0f };
		v[2].position = { cx - halfW, cy + halfH, 0.0f };
		v[3].position = { cx + halfW, cy + halfH, 0.0f };

		v[0].color = color;
		v[1].color = color;
		v[2].color = color;
		v[3].color = color;

		v[0].texcoord = { 0.0f, 0.0f };
		v[1].texcoord = { 1.0f, 0.0f };
		v[2].texcoord = { 0.0f, 1.0f };
		v[3].texcoord = { 1.0f, 1.0f };

		g_pContext->Unmap(g_pVertexBuffer, 0);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

		const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
		const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
		Shader_SetWorldMatrix(XMMatrixIdentity());
		Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(
			0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

		Shader_SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		Tecture_SetTexture(texid);
		g_pContext->Draw(NUM_VERTEX, 0);
	}






