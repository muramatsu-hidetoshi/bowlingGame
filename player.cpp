
/*==========================================================================
    プレイヤーの処理 [player.cpp]

    プレイヤーの初期化・更新・描画・移動処理を管理する。
==========================================================================*/

#include "player.h"
#include "sprite.h"
#include "texture.h"

#include <cmath> // sin、fmodなどの数学関数を使用するため

// プレイヤーの実体を作成する。
Player player;

// テクスチャIDや経過時間を管理する変数。
static int g_TexID = -1;
static int g_PlayerTexID = -1;
static double g_AccumulatedTime;

// プレイヤーの初期化処理。
void Player_Initilize()
{
    // プレイヤーの初期設定を行う。
}

// プレイヤーの更新処理。
// elapsed_timeには前回の更新からの経過時間が渡される。
void Player_Update(double elapsed_time)
{
    // プレイヤーの移動や状態の更新を行う。
}

// プレイヤーの描画処理。
void Player_Draw()
{
    // プレイヤーのスプライトなどを描画する。
}

// プレイヤー終了時の処理。
void Player_Finalize()
{
    // プレイヤーが使用したリソースなどを解放する。
}

// 指定したインデックスに対応するスコアを設定する。
void Player::SetPlayerAllScorer(int index, int score)
{
    // m_allscore配列の指定位置にスコアを保存する。
}

// プレイヤーを上方向へ移動させる。
void Player::PlayerMoveUp()
{
    // Y座標などを変更して上方向へ移動させる。
}

// プレイヤーを下方向へ移動させる。
void Player::PlayerMoveDawn()
{
    // Y座標などを変更して下方向へ移動させる。
}

// プレイヤーを左方向へ移動させる。
void Player::PlayerMoveLeft()
{
    // X座標などを変更して左方向へ移動させる。
}

// プレイヤーを右方向へ移動させる。
void Player::PlayerMoveRight()
{
    // X座標などを変更して右方向へ移動させる。
}

// プレイヤーの実体へのポインタを返す。
Player* GetPlayer()
{
    return &player;
}

// 選択中の列を描画する処理。
// enabledがtrueの場合に表示するなどの用途を想定している。
void DrawSelectedColumn(bool enabled, float x, double elapsedTime)
{
    // 選択列の表示処理を行う。
}

// 選択中のブロックを描画する処理。
void DrawSelectedBlock(bool enabled, float x, float y)
{
    // 選択ブロックの表示処理を行う。
}
