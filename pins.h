/*==========================================================================
    ボウリングのピンを定義するヘッダファイル [pins.h]

    ピンの状態・物理情報・当たり判定を保持し、操作する関数を宣言する。
==========================================================================*/

#pragma once

#include <DirectXMath.h>
#include "model.h"
#include "Collision.h"

class Pins
{
public:
    // ピンの各メンバー変数を初期化する。
    Pins();

    ~Pins() {};

    // 指定位置にピンを生成し、モデルや当たり判定を初期化する。
    void Initialize(const DirectX::XMFLOAT3& pos);

    // 重力や速度を使ってピンの状態を更新する。
    void Update(float deltaTime);

    // ピンのモデルと当たり判定を描画する。
    void Draw();

    // 衝撃ベクトルと作用点を使ってピンに力を加える。
    void Hit(
        const DirectX::XMFLOAT3& impulse,
        const DirectX::XMFLOAT3& hitPoint
    );

    // ピンが倒れているかどうかを取得する。
    bool IsDown() const;

    // ピンが死亡扱いになっているかどうかを取得する。
    bool IsDead() const { return m_isDead; }

    // ピンの現在位置を取得する。
    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }

    // ピンの現在速度を取得する。
    const DirectX::XMFLOAT3& GetVelocity() const { return m_velocity; }

    // ピンを画面外へ移動し、死亡扱いにする。
    void MoveOffscreen(float x);

    // ピンの位置を指定した座標に設定する。
    void SetPosition(const DirectX::XMFLOAT3& pos);

private:
    // ピンの位置・回転・大きさ。
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
    DirectX::XMFLOAT3 m_scale;

    // ピンの移動速度と回転速度。
    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_angularVelocity;

    // ピンが倒れたかどうか。
    bool m_isDown;

    // ピンの質量。
    float m_mass;

    // ピンの当たり判定。
    AABB m_Aabb;

    // ピンの3Dモデル。
    MODEL* m_model{ nullptr };

    // 倒れてから経過した時間。
    float m_aliveTime = 0.0f;

    // ピンが死亡扱いかどうか。
    bool m_isDead = false;

    // 当たり判定の基準となる半サイズ。
    DirectX::XMFLOAT3 m_baseHalf;

    // ピンが衝撃を受けたかどうか。
    bool m_isHit = false;

    // 衝撃を受けてから経過した時間。
    float m_lifeTimer = 0.0f;

    // ピンの初期位置。
    // 現在のコードでは保存のみで、更新処理では使用されていない。
    DirectX::XMFLOAT3 m_spawnPosition;

public:
    // ピンのAABBを読み取り専用で取得する。
    const AABB& GetAABB() const { return m_Aabb; }
};