
/*==========================================================================
デバック用カメラの実装[debug.cpp]





---------------------------------------------------------------------------



==========================================================================*/

#include "debug.h"
#include <DirectXMath.h>
#include "keylogger.h"
#include "direct3d.h"
#include "shader3d.h"

using namespace DirectX;

DebugCamera::DebugCamera(const XMFLOAT3& position, const XMFLOAT3& target)

{
    // 初期 Front ベクトル（カメラの正面方向）
    XMVECTOR xtarget = XMLoadFloat3(&target);
    XMVECTOR xposition = XMLoadFloat3(&position);
    XMVECTOR xFront = XMVector3Normalize(xtarget - xposition);
    XMStoreFloat3(&m_Front, xFront);
    SetPosition(position);
    // ワールド上方向
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    XMStoreFloat3(&m_up, XMLoadFloat3(&up));

    // Right ベクトル（右方向）
    XMVECTOR xright = XMVector3Cross(XMLoadFloat3(&m_up), xFront);
    xright = XMVector3Normalize(xright);
    XMStoreFloat3(&m_right, xright);

    // FOV の初期値（45度）
    m_fov = XM_PI / 4.0f;
}

void DebugCamera::Update(double elapsed_time)
{
    const float MOVESPEED = 5.0f * static_cast<float>(elapsed_time);
    const float ROTSPEED = XMConvertToRadians(60.0f) * static_cast<float>(elapsed_time);

    XMVECTOR xfront = XMLoadFloat3(&m_Front);
    XMVECTOR xright = XMLoadFloat3(&m_right);
    XMVECTOR xup = XMLoadFloat3(&m_up);
    XMVECTOR xposition = XMLoadFloat3(&GetPosition());

#ifdef _DEBUG
    //// --- 回転操作 ---
    if (KeyLogger_IsPressed(KK_RIGHT))
    {
        // Y軸回転
        XMMATRIX rot = XMMatrixRotationY(ROTSPEED);
        xfront = XMVector3TransformNormal(xfront, rot);
        xright = XMVector3TransformNormal(xright, rot);
    }
    if (KeyLogger_IsPressed(KK_LEFT))
    {
        XMMATRIX rot = XMMatrixRotationY(-ROTSPEED);
        xfront = XMVector3TransformNormal(xfront, rot);
        xright = XMVector3TransformNormal(xright, rot);
    }
    if (KeyLogger_IsPressed(KK_UP))
    {
        // ピッチ（上下回転）
        XMMATRIX rot = XMMatrixRotationAxis(xright, -ROTSPEED);
        xfront = XMVector3TransformNormal(xfront, rot);
    }
    if (KeyLogger_IsPressed(KK_DOWN))
    {
        XMMATRIX rot = XMMatrixRotationAxis(xright, ROTSPEED);
        xfront = XMVector3TransformNormal(xfront, rot);
    }
#endif // _DEBUG



    // upベクトルは常に固定
    xup = XMVectorSet(0, 1, 0, 0);

    // 正規直交化
    xfront = XMVector3Normalize(xfront);
    xright = XMVector3Normalize(XMVector3Cross(xup, xfront));
    xup = XMVector3Normalize(XMVector3Cross(xfront, xright));

    //// --- 移動操作 ---
    //if (KeyLogger_IsPressed(KK_W))
    //{
    //    xposition += XMVector3Normalize(XMVectorSetY(xfront, 0.0f)) * MOVESPEED;
    //}
    //if (KeyLogger_IsPressed(KK_S))
    //{
    //    xposition -= XMVector3Normalize(XMVectorSetY(xfront, 0.0f)) * MOVESPEED;
    //}
    //if (KeyLogger_IsPressed(KK_A))
    //{
    //    xposition -= xright * MOVESPEED;
    //}
    //if (KeyLogger_IsPressed(KK_D))
    //{
    //    xposition += xright * MOVESPEED;
    //}
    //if (KeyLogger_IsPressed(KK_Q)) // 上昇
    //{
    //    xposition += xup * MOVESPEED;
    //}
    //if (KeyLogger_IsPressed(KK_E)) // 下降
    //{
    //    xposition -= xup * MOVESPEED;
    //}

    //// --- FOV変更 ---
    //if (KeyLogger_IsPressed(KK_X)) m_fov -= 0.05f;
    //if (KeyLogger_IsPressed(KK_Z)) m_fov += 0.05f;

    // 値をメンバに戻す
    XMStoreFloat3(&GetPositionReference(), xposition);
    XMStoreFloat3(&m_Front, xfront);
    XMStoreFloat3(&m_right, xright);
    XMStoreFloat3(&m_up, xup);
}

void DebugCamera::SetMatrix() 
{
    XMMATRIX mtxView =
        XMMatrixLookAtLH(
            XMLoadFloat3(&GetPosition()),
            XMLoadFloat3(&GetPosition()) + XMLoadFloat3(&m_Front),
            XMLoadFloat3(&m_up));

    SetViewMatrix(mtxView);

    float w = static_cast<float>(Direct3D_GetBackBufferWidth());
    float h = static_cast<float>(Direct3D_GetBackBufferHeight());

    XMMATRIX mtxProj = XMMatrixPerspectiveFovLH(m_fov, w / h, 0.1f, 1000.0f);
    SetProjectionMatrix(mtxProj);
}


