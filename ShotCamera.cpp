/*==========================================================================
ショットカメラの機能[shotCamera.cpp]





---------------------------------------------------------------------------



==========================================================================*/


#include "shotCamera.h"
#include "direct3d.h"


using namespace DirectX;

// 2つの注視対象とカメラまでの距離を受け取り、初期位置を計算する。
ShotCamera::ShotCamera(const DirectX::XMFLOAT3& target_a, const DirectX::XMFLOAT3& target_b, float distance)
	: m_target_a(target_a), m_target_b(target_b),m_distance(distance)

{

    // 2点間の方向を求め、正規化する。
   // その方向と距離を使ってカメラ位置を計算する。
    XMVECTOR dir = XMLoadFloat3(&m_target_a) - XMLoadFloat3(&m_target_b);
    dir = XMVector3Normalize(dir);
    XMVECTOR position = XMLoadFloat3(&m_target_b) - dir * m_distance;

    XMStoreFloat3(&GetPositionReference(), position);

}
// カメラの位置・視点行列・射影行列を更新する。
void ShotCamera::SetMatrix()
{

    // 注視対象間の方向と距離からカメラ位置を求める。

    // 視点行列を作成する。
    // dirはカメラが向く方向、Y軸方向はカメラの上方向を表す。

    // バックバッファの縦横比を使って射影行列を作成する。
    // m_fovは視野角、0.1fと1000.0fは描画する奥行き範囲。
    XMVECTOR dir = XMLoadFloat3(&m_target_a)- XMLoadFloat3(&m_target_b);
    dir =  XMVector3Normalize(dir);
    XMVECTOR position = XMLoadFloat3(&m_target_b) - dir * m_distance;

    XMStoreFloat3(&GetPositionReference(), position);
      XMMATRIX mtxView =  XMMatrixLookToLH(
           position,
           dir,
            XMVECTOR({ 0,1,0 }));

    SetViewMatrix(mtxView);

    float w = static_cast<float>(Direct3D_GetBackBufferWidth());
    float h = static_cast<float>(Direct3D_GetBackBufferHeight());

    XMMATRIX mtxProj = XMMatrixPerspectiveFovLH(m_fov, w / h, 0.1f, 1000.0f);
    SetProjectionMatrix(mtxProj);

}
