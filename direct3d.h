
/*==========================================================================
    Direct3D 11の機能を宣言する [direct3d.h]

    Direct3Dの初期化・終了処理、画面のクリアと表示、
    描画設定の変更などを行う関数を宣言する。
==========================================================================*/

#ifndef DIRECT3D_H
#define DIRECT3D_H

#include <windows.h>
#include <d3d11.h>

// Direct3DのCOMオブジェクトを安全に解放するためのマクロ
// オブジェクトが存在する場合にRelease()を呼び出し、NULLを設定する
#define SAFE_RELEASE(o) if (o){(o)->Release(); o = NULL;}

// Direct3Dデバイスやスワップチェーンなどを初期化する
bool Direct3D_Initialize(HWND hWnd);

// Direct3Dで確保したリソースを解放する
void Direct3D_Finalize();

// 描画先の画面と深度バッファをクリアする
void Direct3D_Clear();

// 描画結果を画面に表示する
void Direct3D_Present();

// バックバッファの幅を取得する
unsigned int Direct3D_GetBackBufferWidth();

// バックバッファの高さを取得する
unsigned int Direct3D_GetBackBufferHeight();

// Direct3Dデバイスを取得する
ID3D11Device* Direct3D_GetDevice();

// Direct3Dデバイスコンテキストを取得する
ID3D11DeviceContext* Direct3D_GetContext();

// アルファブレンドの動作を指定する列挙型
enum AlphaBlendMode
{
    BLEND_TRANSPARENT, // 半透明合成
    BLEND_ADD,         // 加算合成
    BLEND_MAX          // 列挙値の上限として使用
};

// 描画時のブレンドモードを切り替える
void Direct3D_SetAlphaBlend(AlphaBlendMode blend);

// 深度テストを有効または無効にする
void Direct3D_SetDepthTest(bool bEnable);

// 深度テストを有効にしたまま、深度値の書き込みを無効にする
void Direct3D_SetDepthWriteDisable();

// 深度テストと深度値の書き込みを有効にする
void Direct3D_SetDepthWriteEnable();

// ビューポートを設定する関数（現在は未使用）
// void SetViewPort(int n);

#endif // DIRECT3D_H
