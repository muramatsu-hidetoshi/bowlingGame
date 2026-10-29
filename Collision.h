/*==========================================================================
    当たり判定の定義 [collision.h]

    【ファイルの役割】
    ゲーム内のオブジェクト同士の重なりや衝突を判定するための
    クラスを定義する。

    【主なクラス】
    ・Hit       ：衝突結果と衝突法線を保持する
    ・Collision ：当たり判定の基底クラス
    ・AABB      ：軸に沿った箱型の当たり判定
    ・Point     ：1点の位置を使った当たり判定
==========================================================================*/

#pragma once

#include <DirectXMath.h>

// クラスの前方宣言。
// クラスの詳細な定義を後で記述する場合に使用する。
class AABB;
class Point;


// 衝突判定の結果を保持するクラス
class Hit
{
private:
    // 衝突が発生したかどうか
    bool m_isHit{ false };

    // 衝突した面の向きを表す法線ベクトル
    DirectX::XMFLOAT3 m_Normal{};

public:
    // 初期状態の衝突結果を生成する
    Hit() = default;

    // 衝突の有無と法線を指定して初期化する
    Hit(bool isHit, const DirectX::XMFLOAT3& normal)
        : m_isHit(isHit), m_Normal(normal)
    {
    }

    // 衝突が発生したかどうかを取得する
    bool IsHit() const
    {
        return m_isHit;
    }

    // 衝突法線を取得する
    const DirectX::XMFLOAT3& GetNormal() const
    {
        return m_Normal;
    }

    // 衝突法線を設定する
    void SetNormal(const DirectX::XMFLOAT3& normal)
    {
        m_Normal = normal;
    }
};


// 当たり判定の基底クラス
// 異なる形状の当たり判定を共通のインターフェイスで扱うために使用する
class Collision
{
public:
    // 基底クラスのポインタを通して派生クラスを破棄できるようにする
    virtual ~Collision() = default;

    // AABB同士が重なっているかを判定する
    // 基底クラスでは未実装のためfalseを返す
    virtual bool IsOverlap(const AABB& aabb) const
    {
        return false;
    }

    // AABBとの衝突結果を取得する
    // 基底クラスでは衝突なしの結果を返す
    virtual Hit IsHit(const AABB& aabb) const
    {
        return {};
    }

    // 点との重なりを判定する
    // 基底クラスでは未実装のためfalseを返す
    virtual bool IsOverlap(const Point& point) const
    {
        return false;
    }

    // 点との衝突結果を取得する
    // 基底クラスでは衝突なしの結果を返す
    virtual Hit IsHit(const Point& point) const
    {
        return {};
    }
};


// AABB（Axis-Aligned Bounding Box）
// 各辺がX・Y・Z軸に平行な箱型の当たり判定を表す
class AABB : public Collision
{
private:
    // 箱の各軸方向の半分の大きさ
    DirectX::XMFLOAT3 m_Half{};

    // 箱の中心座標
    DirectX::XMFLOAT3 m_Center{};

    // 箱の最小座標
    DirectX::XMFLOAT3 m_Min{};

    // 箱の最大座標
    DirectX::XMFLOAT3 m_Max{};

public:
    // 初期状態のAABBを生成する
    AABB() = default;

    // 最小座標と最大座標を指定してAABBを生成する
    AABB(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
        : m_Half(
            (max.x - min.x) * 0.5f,
            (max.y - min.y) * 0.5f,
            (max.z - min.z) * 0.5f
        )
        , m_Center(
            min.x + m_Half.x,
            min.y + m_Half.y,
            min.z + m_Half.z
        )
        , m_Min(min)
        , m_Max(max)
    {
    }

    // 中心座標と全体のサイズからAABBを生成する
    static AABB Make(
        const DirectX::XMFLOAT3& center,
        const DirectX::XMFLOAT3& size)
    {
        return {
            {
                center.x - size.x * 0.5f,
                center.y - size.y * 0.5f,
                center.z - size.z * 0.5f
            },
            {
                center.x + size.x * 0.5f,
                center.y + size.y * 0.5f,
                center.z + size.z * 0.5f
            }
        };
    }

    // AABBの中心座標を変更する
    // 中心座標に合わせて最小座標と最大座標も更新する
    void Move(const DirectX::XMFLOAT3& position);
    void Move(const DirectX::XMVECTOR& position);

    // AABBの最小座標を取得する
    const DirectX::XMFLOAT3& GetMin() const
    {
        return m_Min;
    }

    // AABBの最大座標を取得する
    const DirectX::XMFLOAT3& GetMax() const
    {
        return m_Max;
    }

    // AABBの中心座標を取得する
    const DirectX::XMFLOAT3& GetCenter() const
    {
        return m_Center;
    }

    // AABBの中心座標を設定する
    // この関数単体では最小座標・最大座標は更新しない
    void SetCenter(const DirectX::XMFLOAT3& center)
    {
        m_Center = center;
    }

    // AABBの各軸方向の半分の大きさを取得する
    const DirectX::XMFLOAT3& GetHalfSize() const
    {
        return m_Half;
    }

    // 別のAABBと重なっているか判定する
    bool IsOverlap(const AABB&) const override;

    // 別のAABBとの衝突結果と法線を取得する
    Hit IsHit(const AABB&) const override;

    // 点がAABBの内部にあるか判定する
    bool IsOverlap(const Point& point) const override;
};


// 1点の位置を使った当たり判定を表すクラス
class Point : public Collision
{
private:
    // 判定対象となる点の座標
    DirectX::XMFLOAT3 m_pos;

public:
    // 初期状態のPointを生成する
    Point() = default;

    // 点の座標を指定して生成する
    Point(const DirectX::XMFLOAT3& position)
        : m_pos(position)
    {
    }

    // 点の座標を取得する
    const DirectX::XMFLOAT3& GetPositin() const
    {
        return m_pos;
    }

    // AABB側の判定処理を利用して、点とAABBの重なりを判定する
    bool IsOverlap(const AABB& aabb) const override
    {
        return aabb.IsOverlap(*this);
    }
};