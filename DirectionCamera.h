/*==========================================================================
    演出用カメラのクラスを宣言する [DirectionCamera.h]

    カメラ位置、向き、注視対象を管理し、
    投球時の追従や上下移動などの演出を行う。
==========================================================================*/

#pragma once

#include <DirectXMath.h>
#include "camera.h"

class DirectionCamera : public Camera
{
private:
    // カメラの正面・右・上方向を表すベクトル
    DirectX::XMFLOAT3 m_Front{ 0.0, 0.0, 1.0 };
    DirectX::XMFLOAT3 m_right{ 1.0, 0.0, 0.0 };
    DirectX::XMFLOAT3 m_up{ 0.0, 1.0, 0.0 };

    // カメラ位置と注視対象の位置
    DirectX::XMFLOAT3 m_Position{ 0, 0, 0 };
    DirectX::XMFLOAT3 m_Target{ 0, 0, 0 };
    DirectX::XMFLOAT3 m_Current_Target{ 0, 0, 0 };

    // カメラの視野角
    float m_fov = 1.0;

    // カメラをリセットする際に使用する初期値
    DirectX::XMFLOAT3 m_InitPosition;
    DirectX::XMFLOAT3 m_InitFront;
    DirectX::XMFLOAT3 m_InitUp;

    // Y方向の上下移動演出を管理する変数
    float m_YEaseTime = 0.0f;  // 演出開始からの経過時間
    bool m_YEaseActive = false; // 演出が有効かどうか

    float m_YStart = 0.0f;       // 演出開始時の高さ
    float m_YDown = -3.0f;       // 下方向への移動量
    float m_YUp = 1.5f;          // 上方向への移動量
    float m_YEaseDuration = 0.6f; // 演出全体の時間（秒）

public:
    // 指定した位置と注視対象でカメラを生成する
    DirectionCamera(
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& target);

    // 経過時間に応じてカメラを更新する
    void Update(double elapsed_time) override;

    // ビュー行列と射影行列を設定する
    void SetMatrix() override;

    // カメラの正面方向を取得する
    DirectX::XMFLOAT3 GetFront() const { return m_Front; }

    // カメラの位置と方向を初期状態に戻す
    void ResetCamera();

    // Y方向の上下移動演出を開始する
    void StartYEase();
};

// Y方向の演出に使用する変化量を計算する
float EaseInOut(float t);



