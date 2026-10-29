/*==========================================================================
    ピンの処理 [pins.cpp]

    ピンの位置・回転・速度を管理し、重力や衝突による挙動を更新する。
==========================================================================*/

#include "pins.h"
#include "cube.h"
#include "debugDraw.h"
#include "trailExplosion.h"

using namespace DirectX;

// 重力加速度。
static constexpr float GRAVITY = -9.8f;

// 床のY座標。
static constexpr float FLOOR_Y = 0.0f;

// X/Z方向の移動量を使って倒れたと判定する場合のしきい値。
// 現在のUpdate処理では、この定数は使用されていない。
static constexpr float POSITION_MOVE_THRESHOLD = 0.15f;

// 角速度の合計がこの値を超えた場合、倒れたと判定する。
static constexpr float ANGULAR_VELOCITY_THRESHOLD = 0.3f;

// ピンの各メンバー変数を初期化する。
Pins::Pins()
    : m_position{ 0,0,0 }
    , m_rotation{ 0,0,0 }
    , m_velocity{ 0,0,0 }
    , m_angularVelocity{ 0,0,0 }
    , m_isDown(false)
    , m_mass(9.8f)
{
}

// ピンの位置や状態を初期化し、モデルと当たり判定を準備する。
void Pins::Initialize(const XMFLOAT3& pos)
{
    // 初期位置と姿勢を設定する。
    m_position = pos;
    m_rotation = { 0,0,0 };

    // 初速と角速度をゼロにする。
    m_velocity = { 0,0,0 };
    m_angularVelocity = { 0,0,0 };

    // ピンの大きさを設定する。
    m_scale = { 1,1,1 };

    // ピンの状態とタイマーを初期化する。
    m_isDown = false;
    m_isDead = false;
    m_isHit = false;
    m_aliveTime = 0.0f;
    m_lifeTimer = 0.0f;

    // ピンの3Dモデルを読み込む。
    m_model = ModelLoad("rom\\Model\\Pin.fbx", 0.9);

    // 当たり判定の半サイズを設定する。
    m_baseHalf = { 1.5f, 3.f, 1.5f };

    // 初期位置を基準にAABBを作成する。
    m_Aabb = AABB::Make(m_position, m_baseHalf);

    // 生成時の位置を保存する。
    // 移動量を使った倒れ判定などに利用できる。
    m_spawnPosition = pos;
}

// 経過時間deltaTimeを使い、ピンの物理状態を更新する。
void Pins::Update(float deltaTime)
{
    // 死亡扱いのピンは物理計算を行わない。
    if (m_isDead) {
        // 現在のスケールを反映した当たり判定サイズを求める。
        XMFLOAT3 half =
        {
            m_baseHalf.x * m_scale.x,
            m_baseHalf.y * m_scale.y,
            m_baseHalf.z * m_scale.z
        };

        // ピンの位置に合わせてAABBの中心を設定する。
        XMFLOAT3 aabbCenter = m_position;
        aabbCenter.y += half.y;

        m_Aabb.SetCenter(half);
        m_Aabb.Move(aabbCenter);
        return;
    }

    // 重力によってY方向の速度を変化させる。
    m_velocity.y += GRAVITY * deltaTime;

    // 速度と経過時間から位置を更新する。
    m_position.x += m_velocity.x * deltaTime;
    m_position.y += m_velocity.y * deltaTime;
    m_position.z += m_velocity.z * deltaTime;

    // 角速度と経過時間から回転角度を更新する。
    m_rotation.x += m_angularVelocity.x * deltaTime;
    m_rotation.y += m_angularVelocity.y * deltaTime;
    m_rotation.z += m_angularVelocity.z * deltaTime;

    // 床より下に移動した場合、床の位置に戻す。
    if (m_position.y < FLOOR_Y)
    {
        m_position.y = FLOOR_Y;

        // Y方向の速度を反転させ、跳ね返りを弱める。
        m_velocity.y *= -0.2f;

        // 床との接触によって水平方向の速度を減衰させる。
        m_velocity.x *= 0.9f;
        m_velocity.z *= 0.9f;

        // X/Z方向の回転速度も減衰させる。
        m_angularVelocity.x *= 0.9f;
        m_angularVelocity.z *= 0.9f;
    }

    // X軸またはZ軸の回転が45度を超えた場合、倒れたと判定する。
    if (fabsf(m_rotation.x) > XM_PIDIV4 ||
        fabsf(m_rotation.z) > XM_PIDIV4)
    {
        m_isDown = true;
    }

    // 回転角度だけでは倒れ判定されていない場合、角速度も確認する。
    if (!m_isDown)
    {
        // 各軸の角速度の絶対値を合計する。
        float angSpeed =
            fabsf(m_angularVelocity.x) +
            fabsf(m_angularVelocity.y) +
            fabsf(m_angularVelocity.z);

        // 回転が十分に大きければ倒れたと判定する。
        if (angSpeed > ANGULAR_VELOCITY_THRESHOLD)
        {
            m_isDown = true;
        }
    }

    // 倒れたピンは一定時間経過後に死亡扱いにする。
    if (m_isDown)
    {
        m_aliveTime += deltaTime;

        if (m_aliveTime > 1.0f)
        {
            m_isDead = true;
        }
    }

    // ボールなどに当たったピンも、一定時間経過後に死亡扱いにする。
    if (m_isHit)
    {
        m_lifeTimer += deltaTime;

        if (m_lifeTimer > 1.0f)
        {
            m_isDead = true;
        }
    }

    // ピンのスケールを反映した当たり判定の半サイズを計算する。
    XMFLOAT3 half =
    {
        m_baseHalf.x * m_scale.x,
        m_baseHalf.y * m_scale.y,
        m_baseHalf.z * m_scale.z
    };

    // AABBの中心を更新する。
    // ピンの位置を底面基準として扱うため、Y方向に半サイズ分加算する。
    XMFLOAT3 aabbCenter = m_position;
    aabbCenter.y += half.y;

    m_Aabb.SetCenter(half);
    m_Aabb.Move(aabbCenter);
}

// ピンの3Dモデルと当たり判定を描画する。
void Pins::Draw()
{
    // 死亡扱いのピンは描画しない。
    if (m_isDead) {
        return;
    }

    // ピンの底面位置からモデルの中心までの描画オフセットを計算する。
    float drawOffsetY = m_baseHalf.y * m_scale.y;

    // 拡大縮小・回転・移動を組み合わせたワールド行列を作成する。
    XMMATRIX mtxworld =
        XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
        XMMatrixRotationX(m_rotation.x) *
        XMMatrixRotationY(m_rotation.y) *
        XMMatrixRotationZ(m_rotation.z) *
        XMMatrixTranslation(
            m_position.x,
            m_position.y + drawOffsetY,
            m_position.z
        );

    // ピンのモデルを描画する。
    ModelDraw(m_model, mtxworld);

    // デバッグ用にAABBを黄色で表示する。
    DebugDraw_AddAABB(m_Aabb, { 1,1,0,1 });
}

// 衝撃によってピンの移動速度と回転速度を変化させる。
void Pins::Hit(const XMFLOAT3& impulse, const XMFLOAT3& hitPoint)
{
    // 最初に衝撃を受けたとき、被弾状態とタイマーを初期化する。
    if (!m_isHit)
    {
        m_isHit = true;
        m_lifeTimer = 0.0f;
    }

    // インパルスを質量で割り、各方向の速度に加える。
    m_velocity.x += impulse.x / m_mass;
    m_velocity.y += impulse.y / m_mass;
    m_velocity.z += impulse.z / m_mass;

    // ピンの位置から衝撃が加わった位置までのベクトルを求める。
    XMVECTOR r = XMLoadFloat3(&hitPoint) - XMLoadFloat3(&m_position);

    // 衝撃ベクトルをDirectXMathのベクトルに変換する。
    XMVECTOR f = XMLoadFloat3(&impulse);

    // 作用点の位置ベクトルと衝撃ベクトルの外積からトルクを求める。
    XMVECTOR torque = XMVector3Cross(r, f);

    // 計算したトルクをXMFLOAT3に変換する。
    XMFLOAT3 t;
    XMStoreFloat3(&t, torque);

    // トルクを角速度に加える。
    // 3.fは回転の強さを調整する係数。
    m_angularVelocity.x += t.x * 3.f;
    m_angularVelocity.y += t.y * 3.f;
    m_angularVelocity.z += t.z * 3.f;
}

// ピンが倒れた状態かどうかを返す。
bool Pins::IsDown() const
{
    return m_isDown;
}

// ピンを指定したX座標へ移動し、死亡扱いにする。
void Pins::MoveOffscreen(float x)
{
    m_position.x = x;

    // 移動・回転速度を停止する。
    m_velocity = { 0,0,0 };
    m_angularVelocity = { 0,0,0 };

    // 死亡扱いにして描画や通常の物理更新を止める。
    m_isDead = true;

    // 現在の位置に合わせてAABBを更新する。
    XMFLOAT3 half =
    {
        m_baseHalf.x * m_scale.x,
        m_baseHalf.y * m_scale.y,
        m_baseHalf.z * m_scale.z
    };

    XMFLOAT3 aabbCenter = m_position;
    aabbCenter.y += half.y;

    m_Aabb.SetCenter(half);
    m_Aabb.Move(aabbCenter);
}

// ピンの位置を明示的に設定し、AABBも更新する。
void Pins::SetPosition(const XMFLOAT3& pos)
{
    m_position = pos;

    // スケールを反映したAABBの半サイズを計算する。
    XMFLOAT3 half =
    {
        m_baseHalf.x * m_scale.x,
        m_baseHalf.y * m_scale.y,
        m_baseHalf.z * m_scale.z
    };

    // AABBの中心を新しい位置に合わせる。
    XMFLOAT3 aabbCenter = m_position;
    aabbCenter.y += half.y;

    m_Aabb.SetCenter(half);
    m_Aabb.Move(aabbCenter);
}
