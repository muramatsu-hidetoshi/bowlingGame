/*==========================================================================
ボウリング球クラスの定義 [bowringBall.h]

概要：
    ボウリング球の状態と操作を管理するクラスを定義する。

主な機能：
    ・ボールの初期化とリセット
    ・速度の更新と力の適用
    ・重力および地形との衝突判定
    ・移動に応じた回転処理
    ・ボールの描画と状態の取得

==========================================================================*/

#pragma once

#include <DirectXMath.h>
#include "collision.h"


//==========================================================================
// ボウリング球クラス
//==========================================================================
class BowlingBall
{
public:
    // コンストラクタ
    BowlingBall();

    // ボールの初期位置を設定し、モデルや描画に必要なリソースを初期化する。
    void Init(const DirectX::XMFLOAT3& pos);

    // 経過時間に応じて、移動・重力・衝突判定・回転を更新する。
    void Update(float deltaTime);

    // ボール本体と関連するエフェクトを描画する。
    void Draw();

    // ボールに力を加え、質量に応じて速度を変化させる。
    void AddForce(const DirectX::XMFLOAT3& force);

    // ボールのワールド変換行列を取得する。
    DirectX::XMMATRIX GetWorldMatrix() const { return m_mtxWorld; }

    // ボールの位置と速度を初期状態に戻す。
    void Reset(const DirectX::XMFLOAT3& pos);

    // ボールが接地しており、速度が十分に小さいかを判定する。
    bool IsStopped() const;


    //======================================================================
    // ボールの状態取得
    //======================================================================

    // ボールの半径を取得する。
    float GetRadius() const { return m_radius; }

    // ボールの現在位置を取得する。
    const DirectX::XMFLOAT3& GetPosition() const { return m_position; }

    // ボールの現在速度を取得する。
    const DirectX::XMFLOAT3& GetVelocity() const { return m_velocity; }

    // ボールの質量を取得する。
    float GetMass() const { return m_mass; }

    // ボールの当たり判定領域を取得する。
    const AABB& GetAABB() { return m_Aabb; }


private:
    //======================================================================
    // 物理状態
    //======================================================================

    DirectX::XMFLOAT3 m_position{}; // ボールのワールド座標
    DirectX::XMFLOAT3 m_velocity{}; // ボールの速度ベクトル

    float m_gravity = -9.8f;        // 重力加速度
    bool m_onGround = false;        // 接地状態

    float m_pawer = 50.0f;          // 力の強さ（現在の実装では未使用）

    AABB m_Aabb{};                  // ボールの当たり判定領域

    float m_mass = 6.8f;             // ボールの質量（kg）

    // 描画に使用するワールド変換行列
    DirectX::XMMATRIX m_mtxWorld{};

    // ガター領域との接触状態
    bool m_gatterFlg = false;


    //======================================================================
    // 回転・形状情報
    //======================================================================

    DirectX::XMFLOAT3 m_rotation{};  // XYZ軸の回転角（ラジアン）

    float m_radius = 1.0f;           // 回転計算に使用するボールの半径
};