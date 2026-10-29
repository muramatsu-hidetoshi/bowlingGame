/*==========================================================================
    プレイヤークラス [player.h]

    プレイヤーの座標やスコアを保持し、移動・スコア操作用の関数を提供する。
==========================================================================*/

#pragma once

class Player
{
private:
    // プレイヤーの座標。
    float m_player_x;
    float m_player_y;

    // 現在のスコア。
    int m_score;

    // 複数のスコアをインデックスごとに保存する配列。
    int m_allscore[100];

public:
    // 各メンバー変数を初期値に設定する。
    Player()
    {
        m_player_x = 0;
        m_player_y = 0;
        m_score = 0;

        // 配列の先頭要素を初期化する。
        m_allscore[0] = {};
    }

    ~Player() {};

    // X座標を設定する。
    void SetPlayerPosX(float x) { m_player_x = x; }

    // X座標を取得する。
    float GetPlayerPosX() const { return m_player_x; }

    // Y座標を設定する。
    void SetPlayerPosY(float y) { m_player_y = y; }

    // Y座標を取得する。
    float GetPlayerPosY() const { return m_player_y; }

    // 現在のスコアを設定する。
    void SetPlayerScore(float s) { m_score = s; }

    // 現在のスコアを取得する。
    float GetPlayerScore() const { return m_score; }

    // 配列の指定位置にスコアを保存する。
    void SetPlayerAllScorer(int index, int score);

    // 配列の指定位置からスコアを取得する。
    int GetPlayerAllScore(int index) const { return m_allscore[index]; }

    // プレイヤーの移動処理。
    void PlayerMoveUp();
    void PlayerMoveDawn();
    void PlayerMoveLeft();
    void PlayerMoveRight();
};

// プレイヤーの実体を取得する。
Player* GetPlayer();

// プレイヤーの初期化・更新・描画・終了処理。
void Player_Initilize();
void Player_Update(double elapsed_time);
void Player_Draw();
void Player_Finalize();

// 選択中の列やブロックを表示する。
void DrawSelectedColumn(bool enabled, float x, double elapsedTime = 0.0);
void DrawSelectedBlock(bool enabled, float x, float y);

/*
    マウス操作への切り替えを想定したメモ。

    1. クリックしたブロックの列をすべて取得する。
    2. 次にクリックしたブロックと入れ替える。
*/