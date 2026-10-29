/*==========================================================================
ボール制御の実装 [ball.cpp]

概要：
    ボールの物理挙動、衝突判定、描画、およびショット処理を実装する。

主な処理：
    ・ボールモデルと当たり判定領域の初期化
    ・重力による速度変化と位置更新
    ・マップとの衝突判定と位置補正
    ・衝突方向に応じた速度の調整
    ・ボールの描画と位置情報の提供

==========================================================================*/

#include "ball.h"
#include "model.h"
#include "direct3d.h"
#include "DirectXMath.h"
#include "shader3d.h"
#include "map.h"
#include "keylogger.h"

using namespace DirectX;


//==========================================================================
// ボールの状態管理
//==========================================================================

// 前回の更新時点のボール位置
static XMFLOAT3 g_PrevPosition{};

// ボールの3Dモデル
static MODEL* g_pBall{};

// ボールの現在位置
static XMFLOAT3 g_Pos{};

// ボールの速度ベクトル
static XMFLOAT3 g_Vel{};

// ボールの当たり判定領域
static AABB g_Aabb{};


// 衝突判定用の関数
static void BallHitChack();


//==========================================================================
// 初期化
//==========================================================================
void Ball_Initialize(const DirectX::XMFLOAT3& position)
{
    // ボールの位置と速度を初期化する。
    g_Pos = position;
    g_Vel = { 0.0f, 0.0f, 0.0f };

    // ボールの3Dモデルを読み込む。
    g_pBall = ModelLoad("rom\\Model\\ball.fbx", 0.1);

    // モデルに設定されている当たり判定情報を取得する。
    g_Aabb = g_pBall->Aabb;

    // 現在位置と前回位置を初期位置に設定する。
    g_Pos = g_PrevPosition = position;


    // ボールの位置を中心とする当たり判定領域を設定する。
    g_Aabb = AABB::Make(position, { 0.3f, 0.3f, 0.3f });
}


//==========================================================================
// 終了処理
//==========================================================================
void Ball_Finalize()
{
    // 読み込んだボールモデルを解放する。
    ModelRelease(g_pBall);

    // 解放済みであることを示す。
    g_pBall = nullptr;
}


//==========================================================================
// 更新処理
//==========================================================================
void Ball_Update(double et)
{
    // DirectXMathのベクトル形式に変換する。
    XMVECTOR position = XMLoadFloat3(&g_Pos);
    XMVECTOR velocity = XMLoadFloat3(&g_Vel);


    //======================================================================
    // 入力による移動処理
    //======================================================================

    // 移動方向を初期化する。
    XMVECTOR dir{};

    /*
    // キー入力による移動処理の例。
    // 現在はコメントアウトされているため、実行されない。

    if (KeyLogger_IsPressed(KK_I)) {
        dir += {0.0f, 0.0f, 1.0f};
    }

    if (KeyLogger_IsPressed(KK_J)) {
        dir += {-1.0f, 0.0f, 0.0f};
    }

    if (KeyLogger_IsPressed(KK_K)) {
        dir += {0.0f, 0.0f, -1.0f};
    }

    if (KeyLogger_IsPressed(KK_L)) {
        dir += {1.0f, 0.0f, 0.0f};
    }
    */


    // 移動方向が設定されている場合は、方向を正規化して速度に反映する。
    if (XMVectorGetX(XMVector3LengthSq(dir)) > 0.0f)
    {
        // 移動方向を正規化し、経過時間に応じた移動量を求める。
        dir = XMVector3Normalize(dir) * 5.0f * (float)et;

        // 移動方向に応じた速度を加算する。
        velocity += dir * 5.0f * (float)et;
    }


    //======================================================================
    // 重力と位置更新
    //======================================================================

    // Y軸の負方向に重力を適用する。
    const XMVECTOR G{ 0.0f, -9.8f, 0.0f };

    // 重力によって速度を更新する。
    velocity += G * 1.0f * (float)et;

    // 更新後の速度を使用して位置を更新する。
    position += velocity * (float)et;

    // 移動後の位置を当たり判定領域に反映する。
    g_Aabb.Move(position);


    //======================================================================
    // 地面との衝突判定
    //======================================================================
    for (int i = 0; i < MapGetBlockCount(); i++)
    {
        // ボールとマップのブロックの衝突を判定する。
        Hit hit = GetCollision(i).IsHit(g_Aabb);

        // 衝突していない場合は次のブロックを調べる。
        if (hit.IsHit())
        {
            // 法線のY成分が正の場合、上方向からの接触として処理する。
            if (hit.GetNormal().y > 0.0f)
            {
                // ボールをブロックの上面に配置する。
                position = XMVectorSetY(
                    position,
                    GetCollision(i).GetMax().y + g_Aabb.GetHalfSize().y
                );

                // 位置補正後の当たり判定領域を更新する。
                g_Aabb.Move(position);

                // Y方向の速度を反転させ、反発する動きを表現する。
                // 反発係数0.8に相当する速度倍率を使用する。
                velocity *= { 1.0f, -0.8f, 1.0f };
            }
        }
    }


    //======================================================================
    // 側面との衝突判定
    //======================================================================
    for (int i = 0; i < MapGetBlockCount(); i++)
    {
        // ボールとブロックの衝突を判定する。
        Hit hit = GetCollision(i).IsHit(g_Aabb);

        if (hit.IsHit())
        {
            // X軸方向の衝突を処理する。
            if (hit.GetNormal().x < 0.0f)
            {
                // ブロックの側面から離れる位置へ補正する。
                position = XMVectorSetX(
                    position,
                    GetCollision(i).GetMin().x - g_Aabb.GetHalfSize().x
                );

                // X方向の速度を停止する。
                velocity *= { 0.0f, 1.0f, 1.0f };
            }
            else if (hit.GetNormal().x > 0.0f)
            {
                // 反対側の側面から離れる位置へ補正する。
                position = XMVectorSetX(
                    position,
                    GetCollision(i).GetMin().x + g_Aabb.GetHalfSize().x
                );

                // X方向の速度を停止する。
                velocity *= { 0.0f, 1.0f, 1.0f };
            }

            // Y軸方向の下側からの衝突を処理する。
            else if (hit.GetNormal().y < 0.0f)
            {
                // ブロックの下面より下へ位置を補正する。
                position = XMVectorSetY(
                    position,
                    GetCollision(i).GetMin().y - g_Aabb.GetHalfSize().y
                );

                // Y方向の速度を停止する。
                velocity *= { 1.0f, 0.0f, 1.0f };
            }

            // Z軸方向の衝突を処理する。
            else if (hit.GetNormal().z < 0.0f)
            {
                // ブロックの側面から離れる位置へ補正する。
                position = XMVectorSetZ(
                    position,
                    GetCollision(i).GetMin().z - g_Aabb.GetHalfSize().z
                );

                // Z方向の速度を停止する。
                velocity *= { 1.0f, 1.0f, 0.0f };
            }
            else if (hit.GetNormal().z > 0.0f)
            {
                // 反対側の側面から離れる位置へ補正する。
                position = XMVectorSetZ(
                    position,
                    GetCollision(i).GetMin().z + g_Aabb.GetHalfSize().z
                );

                // Z方向の速度を停止する。
                velocity *= { 1.0f, 1.0f, 0.0f };
            }

            // 位置補正後の座標を当たり判定領域に反映する。
            g_Aabb.Move(position);
        }
    }


    //======================================================================
    // 更新結果の保存
    //======================================================================

    // 衝突判定用の関数を呼び出す。
    BallHitChack();

    // 更新前の位置を保存する。
    g_PrevPosition = g_Pos;

    // 計算した位置と速度をメンバ変数に反映する。
    XMStoreFloat3(&g_Pos, position);
    XMStoreFloat3(&g_Vel, velocity);
}


//==========================================================================
// 描画処理
//==========================================================================
void Ball_Draw()
{
    // 3D描画用のシェーダーを開始する。
    Shader3d_Begin();

    // ボールの位置からワールド変換行列を作成する。
    XMMATRIX mtxWorld =
        XMMatrixTranslation(g_Pos.x, g_Pos.y, g_Pos.z);

    // ボールのモデルを描画する。
    ModelDraw(g_pBall, mtxWorld);
}


//==========================================================================
// 停止判定
//==========================================================================
bool Ball_IsStationary()
{
    // 停止判定に使用する移動量のしきい値。
    static constexpr float STATIONARY_THRESHOLD = 1e-4f;

    // 前回の位置と現在の位置の差から、移動量を判定する。
    return XMVectorGetX(
        XMVector3Length(
            XMLoadFloat3(&g_PrevPosition) -
            XMLoadFloat3(&g_PrevPosition)
        )
    ) < STATIONARY_THRESHOLD;
}


//==========================================================================
// 位置の取得
//==========================================================================
DirectX::XMFLOAT3 Ball_GetPosition()
{
    // ボールの現在位置を返す。
    return g_Pos;
}


//==========================================================================
// ショット処理
//==========================================================================
void BallShot(const DirectX::XMFLOAT3& velocity)
{
    // 指定された速度をボールの速度に加算する。
    g_Vel.x += velocity.x;

    // Y方向は入力値を5倍して速度に反映する。
    g_Vel.y += velocity.y * 5;

    g_Vel.z += velocity.z;
}


//==========================================================================
// 当たり判定領域の取得
//==========================================================================
const AABB& Ball_GetAABB()
{
    // ボールの当たり判定領域を参照で返す。
    return g_Aabb;
}


//==========================================================================
// 衝突判定用処理
//==========================================================================
void BallHitChack()
{
    // 現在は処理を実装していない。
}
