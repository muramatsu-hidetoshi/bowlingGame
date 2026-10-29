
/*==========================================================================
ボウリング球の挙動実装 [bowlingBall.cpp]

概要：
    BowlingBallクラスの物理挙動、衝突判定、描画処理を実装する。

主な処理：
    ・ボールモデルと描画リソースの初期化
    ・重力と速度に基づく位置更新
    ・マップとのAABB衝突判定
    ・衝突方向に応じた位置補正と速度調整
    ・接地時の摩擦と回転処理
    ・ボールおよびエフェクトの描画

==========================================================================*/

#include "bowlingBall.h"
#include "model.h"
#include "direct3d.h"
#include "shader3d.h"
#include "debugDraw.h"
#include "map.h"
#include "shot.h"
#include "texture.h"
#include "particleEffect.h"
#include "lighth.h"

using namespace DirectX;


// ボウリング球のモデル
static MODEL* g_pBall{};

// エフェクト用テクスチャの管理番号
static int g_Texid = -1;


//==========================================================================
// コンストラクタ
//==========================================================================
BowlingBall::BowlingBall()
{
    // メンバ変数はクラス宣言時の初期値を使用する。
}


//==========================================================================
// 初期化
//==========================================================================
void BowlingBall::Init(const XMFLOAT3& pos)
{
    // ボールの位置と速度を初期化する。
    m_position = pos;
    m_velocity = { 0.0f, 0.0f, 0.0f };

    // ボールの3Dモデルを読み込む。
    g_pBall = ModelLoad("rom\\Model\\bowlingBall1.fbx", 0.9);

    // モデルの当たり判定情報を取得する。
    m_Aabb = g_pBall->Aabb;

    // ボールの位置を中心とする当たり判定領域を設定する。
    m_Aabb = AABB::Make(GetPosition(), { 2.f, 2.f, 2.f });

    // エフェクトに使用するテクスチャを読み込む。
    g_Texid = Texture_Load(L"rom\\Texture\\gra_effect_lightA.png");

    // パーティクルエフェクトを初期化する。
    ParticleEffect_Initialize();


    //======================================================================
    // ライティング設定
    //======================================================================

    // ライト機能を初期化する。
    Light_Initialize();

    // 環境光を設定し、オブジェクト全体の明るさを調整する。
    Light_SetAmnient(XMFLOAT4(0.30f, 0.30f, 0.30f, 1.0f));

    // 方向光の色と向きを設定する。
    Light_SetDiffuse(
        XMFLOAT3(1.0f, 1.0f, 0.95f),
        XMFLOAT3(-0.5f, -1.0f, 0.5f)
    );
}


//==========================================================================
// 更新処理
//==========================================================================
void BowlingBall::Update(float deltaTime)
{
    // フレームごとにガター判定を初期化する。
    m_gatterFlg = false;

    //======================================================================
    // 重力・移動処理
    //======================================================================

    // 重力によってY方向の速度を変化させる。
    m_velocity.y += m_gravity * deltaTime;

    // 現在の速度と経過時間からボールの位置を更新する。
    m_position.x += m_velocity.x * deltaTime;
    m_position.y += m_velocity.y * deltaTime;
    m_position.z += m_velocity.z * deltaTime;

    // 移動後の位置に合わせて当たり判定領域を更新する。
    m_Aabb.Move(m_position);

    // 接地状態をリセットし、衝突判定で再設定する。
    m_onGround = false;


    //======================================================================
    // マップとの衝突判定
    //======================================================================
    for (int i = 0; i < MapGetBlockCount(); ++i)
    {
        // 判定対象のブロックと当たり判定領域を取得する。
        const Block& block = MapGetBlock(i);
        const AABB& blockAabb = GetCollision(i);

        // ボールとブロックの当たり判定を行う。
        Hit hit = m_Aabb.IsHit(blockAabb);

        // 衝突していない場合は次のブロックを調べる。
        if (!hit.IsHit())
            continue;


        //==================================================================
        // 貫入深度の計算
        //==================================================================

        // X・Y・Z各軸について、AABB同士が重なっている深さを求める。
        float depth_x =
            std::min(m_Aabb.GetMax().x, blockAabb.GetMax().x)
            - std::max(m_Aabb.GetMin().x, blockAabb.GetMin().x);

        float depth_y =
            std::min(m_Aabb.GetMax().y, blockAabb.GetMax().y)
            - std::max(m_Aabb.GetMin().y, blockAabb.GetMin().y);

        float depth_z =
            std::min(m_Aabb.GetMax().z, blockAabb.GetMax().z)
            - std::max(m_Aabb.GetMin().z, blockAabb.GetMin().z);


        // 最も重なりが浅い軸を、押し戻しに使用する軸として選択する。
        // 0：X軸、1：Y軸、2：Z軸
        int shallow_axis = 0;

        if (depth_x < depth_y)
        {
            shallow_axis = (depth_x < depth_z) ? 0 : 2;
        }
        else
        {
            shallow_axis = (depth_y < depth_z) ? 1 : 2;
        }


        // ボールとブロックの中心座標、およびボールの半サイズを取得する。
        const XMFLOAT3& ballCenter = m_Aabb.GetCenter();
        const XMFLOAT3& blockCenter = blockAabb.GetCenter();
        const XMFLOAT3& ballHalf = m_Aabb.GetHalfSize();


        //==================================================================
        // 衝突方向に応じた位置補正
        //==================================================================

        if (shallow_axis == 1) // Y軸方向の衝突
        {
            if (ballCenter.y > blockCenter.y)
            {
                // ボールをブロックの上面まで押し戻し、接地状態にする。
                m_position.y = blockAabb.GetMax().y + ballHalf.y;
                m_velocity.y = 0.0f;
                m_onGround = true;
            }
            else
            {
                // ブロックの下面より下へ押し戻し、上昇方向の速度を止める。
                m_position.y = blockAabb.GetMin().y - ballHalf.y;
                m_velocity.y = 0.0f;
            }
        }
        else if (shallow_axis == 0) // X軸方向の衝突
        {
            // ボールの位置に応じて、ブロックの左右いずれかへ押し戻す。
            if (ballCenter.x > blockCenter.x)
            {
                m_position.x = blockAabb.GetMax().x + ballHalf.x;
            }
            else
            {
                m_position.x = blockAabb.GetMin().x - ballHalf.x;
            }

            // 衝突した方向の速度を停止する。
            m_velocity.x = 0.0f;
        }
        else // Z軸方向の衝突
        {
            // ボールの位置に応じて、ブロックの前後いずれかへ押し戻す。
            if (ballCenter.z > blockCenter.z)
            {
                m_position.z = blockAabb.GetMax().z + ballHalf.z;
            }
            else
            {
                m_position.z = blockAabb.GetMin().z - ballHalf.z;
            }

            // 衝突した方向の速度を停止する。
            m_velocity.z = 0.0f;
        }


        // 位置補正後の座標を当たり判定領域に反映する。
        m_Aabb.Move(m_position);


        //==================================================================
        // ブロックの種類に応じた処理
        //==================================================================
        if (hit.IsHit())
        {
            if (block.GetType() == Block::Type::WALL)
            {
                // 壁との接触時はガター状態を解除し、ブロック判定を終了する。
                m_gatterFlg = false;
                break;
            }

            if (block.GetType() == Block::Type::Gutter)
            {
                // ガター領域に接触したことを記録する。
                m_gatterFlg = true;
            }
        }
    }


    //======================================================================
    // 摩擦処理
    //======================================================================
    if (m_onGround)
    {
        // 接地中はX方向の速度を減衰させる。
        m_velocity.x *= 0.995f;

        // ガター接触中はZ方向の速度も減衰させる。
        if (m_gatterFlg)
            m_velocity.z *= 0.98f;
    }


    //======================================================================
    // 回転処理
    //======================================================================
    if (m_onGround)
    {
        // Z方向の速度を取得する。
        float speedZ = m_velocity.z;

        // 微小な速度による不要な回転を防ぐ。
        if (fabs(speedZ) > 0.001f)
        {
            // 経過時間から移動距離を求める。
            float moveDist = speedZ * deltaTime;

            // 移動距離と半径から回転角を計算する。
            float rot = moveDist / m_radius;

            // Z方向の移動に応じてX軸回転を更新する。
            m_rotation.x -= rot;
        }
    }


    // ボールの位置をショット管理側に反映する。
    Shot_SetPosition(m_position);
}


//==========================================================================
// 力の適用
//==========================================================================
void BowlingBall::AddForce(const XMFLOAT3& force)
{
    // 力を質量で割り、各軸の速度に加算する。
    m_velocity.x += force.x / m_mass;
    m_velocity.y += force.y / m_mass;
    m_velocity.z += force.z / m_mass;
}


//==========================================================================
// ボールのリセット
//==========================================================================
void BowlingBall::Reset(const XMFLOAT3& pos)
{
    // 指定された位置に戻し、速度をゼロにする。
    m_position = pos;
    m_velocity = { 0.0f, 0.0f, 0.0f };
}


//==========================================================================
// 停止判定
//==========================================================================
bool BowlingBall::IsStopped() const
{
    // 各軸の速度がこの値未満であれば、停止しているものとみなす。
    const float STOP_EPS = 0.05f;

    // すべての軸の速度が十分に小さく、接地している場合に停止と判定する。
    return
        fabs(m_velocity.x) < STOP_EPS &&
        fabs(m_velocity.y) < STOP_EPS &&
        fabs(m_velocity.z) < STOP_EPS &&
        m_onGround;
}


//==========================================================================
// 描画処理
//==========================================================================
void BowlingBall::Draw()
{
    // 3D描画用のシェーダーを開始する。
    Shader3d_Begin();

    // 深度バッファへの書き込みを有効にする。
    Direct3D_SetDepthWriteEnable();


    //======================================================================
    // ワールド変換行列の作成
    //======================================================================

    // ボールの回転行列を作成する。
    XMMATRIX mtxRot =
        XMMatrixRotationRollPitchYaw(
            m_rotation.x,
            m_rotation.y,
            m_rotation.z
        );

    // ボールの位置を表す平行移動行列を作成する。
    XMMATRIX mtxTrans =
        XMMatrixTranslation(
            m_position.x,
            m_position.y,
            m_position.z
        );

    // 回転と平行移動を組み合わせ、ワールド変換行列を作成する。
    XMMATRIX mtxWorld = mtxRot * mtxTrans;

    // 作成したワールド変換行列をメンバ変数に保存する。
    m_mtxWorld = mtxWorld;


    //======================================================================
    // ボールとエフェクトの描画
    //======================================================================

    // ボール本体を描画する。
    ModelDraw(g_pBall, mtxWorld);

    // 炎エフェクトの描画設定を取得する。
    ParticleEffectParams fire = ParticleEffect_PresetFire();

    // ボールのワールド変換行列を使用してエフェクトを描画する。
    ParticleEffect_Draw(fire, mtxWorld);

    // デバッグ用にボールの当たり判定領域を描画する。
    DebugDraw_AddAABB(m_Aabb, { 1, 0, 0, 1 });
}
