/*==========================================================================

キー入力の管理[keylogger.cpp]


---------------------------------------------------------------------------






=========================================================================*/


#include "keylogger.h"

static Keyboard_State g_PrevState = {};
static Keyboard_State g_TriggerState = {};
static Keyboard_State g_ReleaseState = {};

void KeyLogger_Initialize()
{

    Keyboard_Initialize();


}

void KeyLogger_Update()
{
    // 前回のキー状態を保存してから現在値を取得し、押下・解放を判定できるようにする。

	const Keyboard_State* State = Keyboard_GetState();
	LPBYTE pt = (LPBYTE)&g_TriggerState;
    LPBYTE pn = (LPBYTE)State;
    LPBYTE po = (LPBYTE)&g_PrevState;
	LPBYTE pr = (LPBYTE)&g_ReleaseState;

    for (int i = 0; i < sizeof(Keyboard_State); i++) {
		pt[i] = (po[i] ^ pn[i]) & pn[i];
       pr[i] = (po[i] ^ pn[i]) & ~pn[i];
    }


	g_PrevState = *State; // 現在の状態を前回の状態として保存

}

bool KeyLogger_IsPressed(Keyboard_Keys key)
{
    return Keyboard_IsKeyDown(key);
}

bool KeyLogger_IsTrigger(Keyboard_Keys key)
{
    return Keyboard_IsKeyDown(key,&g_TriggerState);
}

bool KeyLogger_IsRelease(Keyboard_Keys key)
{
    return Keyboard_IsKeyDown(key, &g_ReleaseState);
}
