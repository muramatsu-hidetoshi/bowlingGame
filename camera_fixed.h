/*==========================================================================
    固定カメラの定義 [camera_fixed.h]

    【ファイルの役割】
    特定の位置から対象を映す固定カメラを定義する。

    Cameraクラスを継承し、カメラの位置や注視点、カメラ切り替え用の
    判定領域を管理する。

    【主な機能】
    ・カメラの位置と注視点を設定する
    ・ビュー行列と射影行列を設定する
    ・注視点が指定した判定領域に入っているか確認する
==========================================================================*/

#pragma once

#include <DirectXMath.h>
#include "camera.h"
#include "collision.h"

class FixedCamera : public Camera
{
private:
    // カメラ切り替えの判定に使用する軸平行境界ボックス
    AABB m_aabb;

    // カメラが注視する対象の位置
    DirectX::XMFLOAT3 m_target{};

    // カメラと注視点の距離を保持する変数
    // 現在の実装では使用していない
    float m_distance{};

public:
    // カメラの位置と、カメラ切り替え用の判定領域を指定して初期化する
    FixedCamera(const DirectX::XMFLOAT3& position, const AABB& aabb)
        : m_aabb(aabb)
    {
        // 基底クラスの機能を利用してカメラの位置を設定する
        SetPosition(position);
    }

    // カメラのビュー行列と射影行列を設定する
    // Cameraクラスの純粋仮想関数をオーバーライドする
    void SetMatrix() override;

    // 注視点が判定領域に入っているか確認する
    bool IsOverlap();

    // カメラが注視する対象の位置を設定する
    void SetTarget(const DirectX::XMFLOAT3& target)
    {
        m_target = target;
    }
};