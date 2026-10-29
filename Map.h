/*==========================================================================
マップデータの管理 [map.h]

ステージを構成するBlockクラスと、ブロック情報や当たり判定を
取得する関数、マップの初期化・描画・終了処理を宣言する。
==========================================================================*/

#pragma once

#include <vector>
#include <DirectXMath.h>
#include "Collision.h"

// ステージ内のブロックを管理するクラス
class Block
{
public:
    // ブロックの種類
    enum Type
    {
        EMPTY,      // 空のブロック
        Normal,     // 通常ブロック
        Wood,       // 床
        Gutter,     // ガーター
        Tree,       // 木
        Audience,   // 観客席
        Rock,       // 岩
        WALL,       // 壁
        WALL2,      // 進入禁止壁
        WALL3,      // 手前側の壁
        START,      // スタート地点
        GOAL,       // ゴール地点
        Sand,       // 砂地
        UFO,        // UFO
        Star,       // 星
        StarCore,   // 星の中心部分
        Moon,       // 月
        TYPE_MAX    // 種類の数を示す値
    };

private:
    Type m_Type{ Normal }; // ブロックの種類

    // ブロックの位置と大きさ
    DirectX::XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };

    // ブロックの当たり判定
    AABB m_Aabb{};

public:
    // 位置、大きさ、種類を指定してブロックを作成する
    Block(
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& scale,
        Type type)
        : m_Position(position), m_Scale(scale), m_Type(type)
    {
    }

    // ブロックの大きさを取得する
    const DirectX::XMFLOAT3& GetScale() const
    {
        return m_Scale;
    }

    // ブロックの位置を取得する
    const DirectX::XMFLOAT3& GetPosition() const
    {
        return m_Position;
    }

    // 当たり判定を設定する
    void SetAABB(const AABB& aabb)
    {
        m_Aabb = aabb;
    }

    // 当たり判定を取得する
    const AABB& GetAABB()
    {
        return m_Aabb;
    }

    // ブロックの種類を取得する
    Type GetType() const
    {
        return m_Type;
    }

    // ブロックを描画する
    void Draw() const;
};

// 指定された番号のブロックを取得する
const Block& MapGetBlock(int index);

// マップ内のブロック数を取得する
int MapGetBlockCount();

// 指定された番号の当たり判定を取得する
const AABB& GetCollision(int index);

// マップの初期化・終了・描画
void Map_Initialize();
void Map_Finalize();
void Map_Draw();
