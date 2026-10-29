/*==========================================================================
ショットカメラの機能[shotCamera.cpp]





---------------------------------------------------------------------------



==========================================================================*/


#include "shotCamera.h"
#include "direct3d.h"


using namespace DirectX;
ShotCamera::ShotCamera(const DirectX::XMFLOAT3& target_a, const DirectX::XMFLOAT3& target_b, float distance)
	: m_target_a(target_a), m_target_b(target_b),m_distance(distance)

{


    XMVECTOR dir = XMLoadFloat3(&m_target_a) - XMLoadFloat3(&m_target_b);
    dir = XMVector3Normalize(dir);
    XMVECTOR position = XMLoadFloat3(&m_target_b) - dir * m_distance;

    XMStoreFloat3(&GetPositionReference(), position);

}

void ShotCamera::SetMatrix()
{

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
