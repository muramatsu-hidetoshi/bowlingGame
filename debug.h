/*==========================================================================
    デバッグカメラの定義 [debugcamera.h]

    【ファイルの役割】
    デバッグ時にカメラの向きを変更し、シーンを確認するための
    DebugCameraクラスを定義する。

    Cameraクラスを継承し、カメラの位置や向きを管理する。

    【主な機能】
    ・カメラの初期位置と注視方向を設定する
    ・キー入力によってカメラの向きを変更する
    ・ビュー行列と射影行列を設定する
==========================================================================*/

#pragma once

#include <DirectXMath.h>
#include "camera.h"

class DebugCamera : public Camera
{
private:
    // カメラの正面方向を表すベクトル
    DirectX::XMFLOAT3 m_Front{ 0.0f, 0.0f, 1.0f };

    // カメラの右方向を表すベクトル
    DirectX::XMFLOAT3 m_right{ 1.0f, 0.0f, 0.0f };

    // カメラの上方向を表すベクトル
    DirectX::XMFLOAT3 m_up{ 0.0f, 1.0f, 0.0f };

    // カメラの視野角（ラジアン）
    float m_fov = 1.0f;

public:
    // カメラの位置と注視対象の位置を指定して初期化する
    DebugCamera(
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& target
    );

    // キー入力などに応じてカメラの状態を更新する
    // CameraクラスのUpdate関数をオーバーライドする
    void Update(double elapsed_time) override;

    // ビュー行列と射影行列を設定する
    // Cameraクラスの純粋仮想関数をオーバーライドする
    void SetMatrix() override;

    // カメラの正面方向を取得する
    DirectX::XMFLOAT3 GetFront() const
    {
        return m_Front;
    }
};