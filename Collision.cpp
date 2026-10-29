/*==========================================================================
    当たり判定の実装 [collision.cpp]

    【ファイルの役割】
    AABBの移動処理と、他のAABBや点との重なり判定を実装する。

    【主な処理】
    ・AABBの中心座標に合わせて最小座標と最大座標を更新する
    ・2つのAABBが重なっているか判定する
    ・衝突時に重なりが最も小さい軸を調べ、衝突法線を求める
    ・点がAABBの内部にあるか判定する
==========================================================================*/

#include "collision.h"
#include <algorithm>
#include <DirectXMath.h>

using namespace DirectX;


// AABBの中心座標を更新する
void AABB::Move(const XMFLOAT3& position)
{
    // 新しい中心座標を設定する
    m_Center = position;

    // 中心座標と半分の大きさから、各軸の最小座標を計算する
    m_Min = {
        m_Center.x - m_Half.x,
        m_Center.y - m_Half.y,
        m_Center.z - m_Half.z
    };

    // 中心座標と半分の大きさから、各軸の最大座標を計算する
    m_Max = {
        m_Center.x + m_Half.x,
        m_Center.y + m_Half.y,
        m_Center.z + m_Half.z
    };
}


// XMVECTOR型の座標を使用してAABBの中心座標を更新する
void AABB::Move(const XMVECTOR& position)
{
    // XMVECTOR型の座標をXMFLOAT3型に変換して中心座標に保存する
    XMStoreFloat3(&m_Center, position);

    // 中心座標から半分の大きさを引いて最小座標を求める
    XMStoreFloat3(
        &m_Min,
        position - XMLoadFloat3(&m_Half)
    );

    // 中心座標に半分の大きさを加えて最大座標を求める
    XMStoreFloat3(
        &m_Max,
        position + XMLoadFloat3(&m_Half)
    );
}


// 2つのAABBが重なっているか判定する
bool AABB::IsOverlap(const AABB& aabb) const
{
    // X・Y・Zのすべての軸で区間が重なっている場合、
    // 2つのAABBは重なっていると判定する。
    //
    // 各軸について、
    // 自分の最小座標が相手の最大座標より小さく、
    // 自分の最大座標が相手の最小座標より大きいことを確認する。
    //
    // 比較に < と > を使用しているため、面が接しているだけの場合は
    // 重なりとして扱わない。

    return
        m_Min.x < aabb.GetMax().x &&
        m_Max.x > aabb.GetMin().x &&
        m_Min.y < aabb.GetMax().y &&
        m_Max.y > aabb.GetMin().y &&
        m_Min.z < aabb.GetMax().z &&
        m_Max.z > aabb.GetMin().z;
}


// 2つのAABBが衝突しているか確認し、衝突法線を取得する
Hit AABB::IsHit(const AABB& aabb) const
{
    // 重なっていない場合は、衝突なしの結果を返す
    if (!IsOverlap(aabb))
    {
        return {};
    }

    // X軸方向の重なりの深さを計算する
    float depth_x =
        std::min(GetMax().x, aabb.GetMax().x) -
        std::max(GetMin().x, aabb.GetMin().x);

    // Y軸方向の重なりの深さを計算する
    float depth_y =
        std::min(GetMax().y, aabb.GetMax().y) -
        std::max(GetMin().y, aabb.GetMin().y);

    // Z軸方向の重なりの深さを計算する
    float depth_z =
        std::min(GetMax().z, aabb.GetMax().z) -
        std::max(GetMin().z, aabb.GetMin().z);

    // 重なりが最も小さい軸を記録する
    // 0：X軸、1：Y軸、2：Z軸
    int shallow_axis = 0;

    // X軸とY軸の重なりを比較する
    if (depth_x < depth_y)
    {
        // X軸とZ軸を比較する
        if (depth_x < depth_z)
        {
            shallow_axis = 0;
        }
        else
        {
            shallow_axis = 2;
        }
    }
    else
    {
        // Y軸とZ軸を比較する
        if (depth_y < depth_z)
        {
            shallow_axis = 1;
        }
        else
        {
            shallow_axis = 2;
        }
    }

    // 衝突が発生したことを示すHitを作成する
    // 法線は、どちら側から衝突したかを調べてから設定する
    Hit hit{ true, {} };

    // 2つのAABBの中心座標の差を計算する
    // 現在のコードではこの変数を使用していない
    XMVECTOR difference =
        XMLoadFloat3(&aabb.GetCenter()) -
        XMLoadFloat3(&GetCenter());

    // 衝突法線を格納する変数
    XMFLOAT3 normal{};

    // 重なりが最も小さい軸を基準に、衝突法線の向きを決める
    switch (shallow_axis)
    {
    case 0:
        // X軸方向の重なりが最も小さい場合
        // 相手の中心が自分より左なら-X方向、右なら+X方向を法線とする
        normal = aabb.GetCenter().x - GetCenter().x < 0.0f
            ? XMFLOAT3{ -1.0f, 0.0f, 0.0f }
        : XMFLOAT3{ 1.0f, 0.0f, 0.0f };
        break;

    case 1:
        // Y軸方向の重なりが最も小さい場合
        // 相手の中心が自分より下なら-Y方向、上なら+Y方向を法線とする
        normal = aabb.GetCenter().y - GetCenter().y < 0.0f
            ? XMFLOAT3{ 0.0f, -1.0f, 0.0f }
        : XMFLOAT3{ 0.0f,  1.0f, 0.0f };
        break;

    case 2:
        // Z軸方向の重なりが最も小さい場合
        // 相手の中心が自分より手前なら-Z方向、奥なら+Z方向を法線とする
        normal = aabb.GetCenter().z - GetCenter().z < 0.0f
            ? XMFLOAT3{ 0.0f, 0.0f, -1.0f }
        : XMFLOAT3{ 0.0f, 0.0f,  1.0f };
        break;

    default:
        break;
    }

    // 計算した衝突法線を設定し、衝突結果を返す
    return { true, normal };
}


// 点がAABBの内部にあるか判定する
bool AABB::IsOverlap(const Point& point) const
{
    // 点の座標がX・Y・Zのすべての軸で
    // AABBの最小座標より大きく、最大座標より小さい場合にtrueを返す。
    //
    // 境界上の点は重なりとして扱わない。

    return
        m_Min.x < point.GetPositin().x &&
        m_Max.x > point.GetPositin().x &&
        m_Min.y < point.GetPositin().y &&
        m_Max.y > point.GetPositin().y &&
        m_Min.z < point.GetPositin().z &&
        m_Max.z > point.GetPositin().z;
}