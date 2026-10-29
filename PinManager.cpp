
/*==========================================================================
    ピンの管理者 [pinManager.cpp]

    複数のピンをまとめて管理し、配置・更新・描画・衝突処理を行う。
==========================================================================*/

#include "pinManager.h"
#include "bowlingBall.h"
#include "trailExplosion.h"
#include "audio.h"

using namespace DirectX;

// ピンの配置間隔。
static constexpr float PIN_SPACING_X = 1.6f; // 横方向の間隔
static constexpr float PIN_SPACING_Z = 1.6f; // 奥行き方向の間隔

// 衝突時に使用する効果音のID。
static int g_StrikeSound{};
static int g_HitSound{};

// ピンの配置と衝突時に使用する効果音を初期化する。
void PinManager::Initialize()
{
    // ピンを初期配置する。
    ResetPins();

    // 衝突時とストライク時に使用する効果音を読み込む。
    g_HitSound = LoadAudio("rom\\sound\\HitSound.wav");
    g_StrikeSound = LoadAudio("rom\\sound\\StrikeSound.wav");
}

// 管理しているすべてのピンを描画する。
void PinManager::Draw()
{
    for (auto& p : m_pins)
        p.Draw();
}

// ピンの物理更新と衝突判定を行う。
void PinManager::Update(float dt, BowlingBall& ball)
{
    // ① すべてのピンの位置・速度・回転などを更新する。
    for (auto& p : m_pins)
        p.Update(dt);

    // ② ボールとピンの衝突判定を行う。
    for (auto& p : m_pins)
    {
        // 倒れたピンはボールとの衝突判定を行わない。
        if (p.IsDown()) continue;

        // ボールとピンのAABBが衝突しているか確認する。
        Hit hit = ball.GetAABB().IsHit(p.GetAABB());

        // 衝突していなければ次のピンへ進む。
        if (!hit.IsHit()) continue;

        // 衝突した場合は、ボールの速度をもとにピンへ力を加える。
        ResolveBallPinHit(ball, p);
    }

    // ③ ピン同士の衝突判定を行う。
    // 同じ組み合わせを2回処理しないよう、jはi+1から開始する。
    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        for (size_t j = i + 1; j < m_pins.size(); ++j)
        {
            // 2本のピンのAABBが衝突しているか確認する。
            Hit hit = m_pins[i].GetAABB().IsHit(m_pins[j].GetAABB());

            if (!hit.IsHit()) continue;

            // 衝突したピン同士の速度をもとに力を計算する。
            ResolvePinPin(m_pins[i], m_pins[j]);
        }
    }
}

// ピン管理で使用した効果音を解放する。
void PinManager::Finalize()
{
    UnloadAudio(g_StrikeSound);
    UnloadAudio(g_HitSound);
}

// 指定位置に新しいピンを追加する。
void PinManager::AddPin(const XMFLOAT3& pos)
{
    // ピンを生成して初期化する。
    Pins pin;
    pin.Initialize(pos);

    // 管理対象の配列に追加する。
    m_pins.emplace_back(pin);
}

// 2本のピンが衝突したときの力を計算する。
void PinManager::ResolvePinPin(Pins& a, Pins& b)
{
    // ピンaからピンbへ向かう方向ベクトルを求める。
    XMVECTOR dir =
        XMLoadFloat3(&b.GetPosition()) -
        XMLoadFloat3(&a.GetPosition());

    // 方向ベクトルを正規化する。
    dir = XMVector3Normalize(dir);

    // それぞれのピンの速度を取得する。
    XMFLOAT3 va = a.GetVelocity();
    XMFLOAT3 vb = b.GetVelocity();

    // 2本のピンの相対速度を求める。
    XMVECTOR relVel =
        XMLoadFloat3(&va) - XMLoadFloat3(&vb);

    // 相対速度を衝突方向へ射影し、衝突方向の速度成分を求める。
    float speed = XMVectorGetX(XMVector3Dot(relVel, dir));

    // 離れる方向に動いている場合は、衝突による力を加えない。
    if (speed > 0.0f) return;

    // 衝突時の力の大きさを計算する。
    // 0.5fは力の強さを調整する係数。
    float impulse = speed * 0.5f;

    // 衝突方向に沿ったインパルスをベクトルに変換する。
    XMFLOAT3 f;
    XMStoreFloat3(&f, dir * impulse);

    // 互いに反対方向の衝撃を加える。
    // 作用点には各ピンの位置を指定する。
    a.Hit({ -f.x, -f.y, -f.z }, a.GetPosition());
    b.Hit({ f.x, f.y, f.z }, b.GetPosition());
}

// ボールとピンが衝突したときの力を計算する。
void PinManager::ResolveBallPinHit(BowlingBall& ball, Pins& pin)
{
    // ボールからピンへ向かう方向ベクトルを求める。
    XMVECTOR dir =
        XMLoadFloat3(&pin.GetPosition()) -
        XMLoadFloat3(&ball.GetPosition());

    // 衝突方向を正規化する。
    dir = XMVector3Normalize(dir);

    // ボールの速度を取得する。
    XMFLOAT3 ballVel = ball.GetVelocity();
    XMVECTOR v = XMLoadFloat3(&ballVel);

    // ボールの速度から、ピンへ向かう方向の速度成分を求める。
    float speed = XMVectorGetX(XMVector3Dot(v, dir));

    // ピンへ向かう速度成分がない場合は処理を終了する。
    if (speed <= 0.0f) return;

    // ボールの質量と速度から衝撃の強さを求める。
    // 1.2fは衝撃の強さを調整する係数。
    float impulsePower = speed * ball.GetMass() * 1.2f;

    // 衝突方向に沿ったインパルスを作成する。
    XMVECTOR impulse = dir * impulsePower;

    // 衝撃を加える位置をピンの位置より少し上に設定する。
    XMFLOAT3 hitPoint = pin.GetPosition();
    hitPoint.y += 1.0f;

    // インパルスをXMFLOAT3に変換する。
    XMFLOAT3 impulseVec;
    XMStoreFloat3(&impulseVec, impulse);

    // 上方向の力を加え、ピンが跳ね上がるようにする。
    impulseVec.y += impulsePower;

    // ピンに衝撃を加え、移動と回転を発生させる。
    pin.Hit(impulseVec, hitPoint);

    // 衝突位置に放射状の爆発エフェクトを生成する。
    TrailExplosion_CreateRadial(pin.GetPosition(), 0.5f);

    // 衝突時の効果音を再生する。
    PlayAudio(g_StrikeSound, false);
}

// 死亡扱いのピンを画面外へ移動させる。
void PinManager::RemoveDeadPins()
{
    for (auto& p : m_pins)
    {
        if (p.IsDead())
        {
            // 配列から削除せず、画面外へ移動させて描画対象から外す。
            p.MoveOffscreen(100.0f);
        }
    }
}

// 死亡扱いではないピンの数を取得する。
int PinManager::GetRemainingPinCount() const
{
    int remaining = 0;

    for (const auto& p : m_pins)
    {
        if (!p.IsDead())
            ++remaining;
    }

    return remaining;
}

// 初期ピン数を取得する。
// 現在は10本固定で返す。
int PinManager::GetInitialPinCount() const
{
    return 10;
}

// 指定位置を先頭ピンとして、10本のピンを三角形状に配置する。
void PinManager::SetupBowlingPins(const XMFLOAT3& headPinPos)
{
    // 既存のピンをすべて削除してから配置し直す。
    m_pins.clear();

    // ピン同士の横方向と奥行き方向の間隔。
    static constexpr float DX = 1.6f;
    static constexpr float DZ = 1.6f;

    // 1段目：先頭のピンを1本配置する。
    AddPin(headPinPos);

    // 2段目：左右に2本配置する。
    AddPin({ headPinPos.x - DX * 0.5f, headPinPos.y, headPinPos.z + DZ });
    AddPin({ headPinPos.x + DX * 0.5f, headPinPos.y, headPinPos.z + DZ });

    // 3段目：3本配置する。
    AddPin({ headPinPos.x - DX, headPinPos.y, headPinPos.z + DZ * 2 });
    AddPin({ headPinPos.x,      headPinPos.y, headPinPos.z + DZ * 2 });
    AddPin({ headPinPos.x + DX, headPinPos.y, headPinPos.z + DZ * 2 });

    // 4段目：4本配置する。
    AddPin({ headPinPos.x - DX * 1.5f, headPinPos.y, headPinPos.z + DZ * 3 });
    AddPin({ headPinPos.x - DX * 0.5f, headPinPos.y, headPinPos.z + DZ * 3 });
    AddPin({ headPinPos.x + DX * 0.5f, headPinPos.y, headPinPos.z + DZ * 3 });
    AddPin({ headPinPos.x + DX * 1.5f, headPinPos.y, headPinPos.z + DZ * 3 });
}

// ピンを初期位置に配置する。
void PinManager::ResetPins()
{
    // 指定した座標を先頭ピンの位置として10本を配置する。
    SetupBowlingPins({ 5.f, 5.0f, 10.0f });
}

// 倒れたピンと死亡扱いのピンの合計数を取得する。
int PinManager::GetDownPinCount() const
{
    int down = 0;

    for (const auto& p : m_pins)
    {
        // 倒れたピン、または死亡扱いのピンをカウントする。
        if (p.IsDown() || p.IsDead())
            down++;
    }

    return down;
}

// 現在のピンをすべて削除し、初期位置に再配置する。
// ボウリングの1フレームが終了したときなどに使用する。
void PinManager::DestroyAndRecreatePins()
{
    m_pins.clear();
    SetupBowlingPins({ 5.f, 5.0f, 10.0f });
}

