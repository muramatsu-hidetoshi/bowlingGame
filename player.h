
/*==========================================================================

	プレイヤークラス [player.h]



---------------------------------------------------------------------------



==========================================================================*/


#pragma once
class Player
{
private:


	float m_player_x ;
	float m_player_y ;
	int m_score;
	int m_allscore[100];


public:

	 Player()
	 {

		 m_player_x = 0;
		 m_player_y = 0;
		 m_score = 0;
		 m_allscore[0]={};
	 };
	~Player(){};




	void SetPlayerPosX(float x) { m_player_x = x; }
	float GetPlayerPosX()const { return m_player_x; }

	void SetPlayerPosY(float y) { m_player_y = y; }
	float GetPlayerPosY()const { return m_player_y; }

	void SetPlayerScore(float s) { m_score = s; }
	float GetPlayerScore()const { return m_score; }

	void SetPlayerAllScorer(int index,int score);
	int  GetPlayerAllScore(int index) const { return m_allscore[index]; };

	void PlayerMoveUp();
	void PlayerMoveDawn();
	void PlayerMoveLeft();
	void PlayerMoveRight();

};

Player* GetPlayer();
void Player_Initilize();
void Player_Update(double elapsed_time);
void Player_Draw();
void Player_Finalize();
void DrawSelectedColumn(bool enabled, float x, double elapsedTime = 0.0);
void DrawSelectedBlock(bool enabled, float x, float y);
//void DrawSelectedBlock(bool enabled, float x, float y);
/*
* 
 　　マウス操作に切り替え
   　クリックして選択したブロック列を全て取得
	 次にクリックしたブロックと入れ替え





*/