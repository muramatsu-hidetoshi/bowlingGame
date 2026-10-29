
/*==========================================================================
マウス入力の管理 [MouseLogger.cpp]

マウスの状態を記録し、ボタンの押下・押下開始・解放や、
座標の移動量を取得する。
==========================================================================*/

#include "mouseLogger.h"
#include <cstring>

// 現在のマウス状態と前回のマウス状態
static Mouse_State gCurrentState;
static Mouse_State gPrevState;

// 現在と前回のホイール値
static int gWheelDelta = 0;
static int gPrevWheelDelta = 0;

// 前回のマウス座標
static int gPrevX = 0;
static int gPrevY = 0;

// 初期化処理
void MouseLogger_Initialize()
{
    // マウス状態をゼロで初期化する
    memset(&gCurrentState, 0, sizeof(Mouse_State));
    memset(&gPrevState, 0, sizeof(Mouse_State));

    // 前回の座標を初期化する
    gPrevX = 0;
    gPrevY = 0;
}

// 更新処理
void MouseLogger_Update()
{
    // 現在の状態を前回の状態として保存する
    memcpy(&gPrevState, &gCurrentState, sizeof(Mouse_State));

    // ホイールの前回値を保存する
    gPrevWheelDelta = gWheelDelta;

    // 更新前のマウス座標を保存する
    gPrevX = gCurrentState.x;
    gPrevY = gCurrentState.y;

    // マウスの最新状態を取得する
    Mouse_GetState(&gCurrentState);

    // 最新のホイール値を記録する
    gWheelDelta = gCurrentState.scrollWheelValue;
}

// 指定されたボタンが押されているかを取得する
static bool getButton(const Mouse_State& s, MouseKey key)
{
    switch (key)
    {
    case MouseKey::Left:
        return s.leftButton;

    case MouseKey::Right:
        return s.rightButton;

    case MouseKey::Middle:
        return s.middleButton;

    case MouseKey::X1:
        return s.xButton1;

    case MouseKey::X2:
        return s.xButton2;

    case MouseKey::WheelUp:
        // ホイール値が正の場合は上方向として扱う
        return (s.scrollWheelValue > 0);

    case MouseKey::WheelDown:
        // ホイール値が負の場合は下方向として扱う
        return (s.scrollWheelValue < 0);
    }

    // 対応するボタンがない場合はfalseを返す
    return false;
}

// ボタンが押されている間、trueを返す
bool MouseLogger_IsPressed(MouseKey key)
{
    return getButton(gCurrentState, key);
}

// ボタンが押された瞬間だけtrueを返す
bool MouseLogger_IsTrigger(MouseKey key)
{
    // 現在押されていて、前回は押されていない場合に成立する
    return (getButton(gCurrentState, key)
        && !getButton(gPrevState, key));
}

// ボタンが離された瞬間だけtrueを返す
bool MouseLogger_IsRelease(MouseKey key)
{
    // 現在押されておらず、前回は押されていた場合に成立する
    return (!getButton(gCurrentState, key)
        && getButton(gPrevState, key));
}

// 前回の更新からのX方向の移動量を取得する
int MouseLogger_GetRelativeX()
{
    return gCurrentState.x - gPrevX;
}

// 前回の更新からのY方向の移動量を取得する
int MouseLogger_GetRelativeY()
{
    return gCurrentState.y - gPrevY;
}

// マウスの現在のX座標を取得する
int MouseLogger_GetX()
{
    return gCurrentState.x;
}

// マウスの現在のY座標を取得する
int MouseLogger_GetY()
{
    return gCurrentState.y;
}


