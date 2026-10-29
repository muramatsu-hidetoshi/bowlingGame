/*==========================================================================
マウス入力の管理 [mouseLogger.h]

マウスボタンの種類と、入力状態や座標を取得する関数を宣言する。
==========================================================================*/

#pragma once
#include "mouse.h"

// 入力判定に使用するマウスボタン
enum class MouseKey
{
    Left,       // 左ボタン
    Right,      // 右ボタン
    Middle,     // 中ボタン
    X1,         // サイドボタン1
    X2,         // サイドボタン2
    WheelUp,    // ホイール上方向
    WheelDown   // ホイール下方向
};

// マウス入力の初期化と更新
void MouseLogger_Initialize();
void MouseLogger_Update();

// ボタン入力の状態を取得する
bool MouseLogger_IsPressed(MouseKey key); // 押されている間
bool MouseLogger_IsTrigger(MouseKey key); // 押された瞬間
bool MouseLogger_IsRelease(MouseKey key); // 離された瞬間

// マウスの相対移動量を取得する
int MouseLogger_GetRelativeX();
int MouseLogger_GetRelativeY();

// マウスの絶対座標を取得する
int MouseLogger_GetX();
int MouseLogger_GetY();