
/*==========================================================================
プレイヤーの処理[player.cpp]





---------------------------------------------------------------------------



==========================================================================*/


#include "player.h"
#include "sprite.h"
#include "texture.h"

#include <cmath> // sin, fmod 用

Player player;
static int g_TexID = -1;
static int g_PlayerTexID = -1;
static double g_AccumulatedTime;
void Player_Initilize()
{



}

void Player_Update(double elapsed_time)
{


}

void Player_Draw()
{



}

void Player_Finalize()
{

}
void Player::SetPlayerAllScorer(int index,int score)
{




}
void Player::PlayerMoveUp()
{


}


void Player::PlayerMoveDawn()
{



}

void Player::PlayerMoveLeft()
{


}

void Player::PlayerMoveRight()
{

}

Player* GetPlayer()
{
	return &player;
}


void DrawSelectedColumn(bool enabled, float x, double elapsedTime)
{

}


void DrawSelectedBlock(bool enabled, float x, float y)
{

}
