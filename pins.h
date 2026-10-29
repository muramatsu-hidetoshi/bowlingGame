/*==========================================================================
ボーリングのピンを定義するヘッダファイル[pins.h]


---------------------------------------------------------------------------

==========================================================================*/

#pragma once
#include <DirectXMath.h>
#include "model.h"
#include "Collision.h"
class Pins
{
public:
    Pins();
    ~Pins() {};

    // 初期化
    void Initialize(const DirectX::XMFLOAT3& pos);

    // 更新
    void Update(float deltaTime);

    // 描画
    void Draw() ;

    // 衝撃
    void Hit(const DirectX::XMFLOAT3& impulse, const DirectX::XMFLOAT3& hitPoint);

    // 判定取得
    bool IsDown() const;
    bool IsDead() const { return m_isDead; }
    // 位置/速度取得
    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }
    const DirectX::XMFLOAT3& GetVelocity() const { return m_velocity; }

    // 新: オフスクリーンへ移動して死亡扱いにする
    void MoveOffscreen(float x);

    // 新: 位置を明示的にセット（必要なら使用）
    void SetPosition(const DirectX::XMFLOAT3& pos);

private:

    // 内部データ
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
    DirectX::XMFLOAT3 m_scale;
    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_angularVelocity;
    bool  m_isDown;
    float m_mass;
    AABB m_Aabb;
    MODEL* m_model{ nullptr };
    float m_aliveTime = 0.0f;
    bool  m_isDead = false;
    DirectX::XMFLOAT3 m_baseHalf;
    bool m_isHit = false;
    float m_lifeTimer = 0.0f;

    // 新: 初期スポーン位置（X/Z の変位判定に使用）
    DirectX::XMFLOAT3 m_spawnPosition;

public:
    const AABB& GetAABB() const { return m_Aabb; }

};
