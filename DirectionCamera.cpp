/*==========================================================================
演出用カメラ[directionCamera.cpp]





---------------------------------------------------------------------------



==========================================================================*/


#include "directionCamera.h"
#include <DirectXMath.h>
#include "keylogger.h"
#include "direct3d.h"
#include "shader3d.h"
using namespace DirectX;

DirectionCamera::DirectionCamera(
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
    :m_Target(target),m_Current_Target(target)
{
    m_InitPosition = position;
    m_InitFront = m_Front;
    m_InitUp = m_up;

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

void DirectionCamera::Update(double elapsed_time)
{
    // ボールの移動に合わせて追従位置と注視点を更新する。

    const float MOVESPEED = 5.0f * static_cast<float>(elapsed_time);
    const float ROTSPEED = XMConvertToRadians(60.0f) * static_cast<float>(elapsed_time);

    XMVECTOR xfront = XMLoadFloat3(&m_Front);
    XMVECTOR xright = XMLoadFloat3(&m_right);
    XMVECTOR xup = XMLoadFloat3(&m_up);
    XMVECTOR xposition = XMLoadFloat3(&GetPosition());


    // --- ターゲットに向かってズーム ---
    XMVECTOR xtarget = XMLoadFloat3(&m_Target);
    //XMVECTOR xposition = XMLoadFloat3(&GetPosition());

   // 現在の方向（Target → Camera）
    XMVECTOR dir = XMVector3Normalize(xposition - xtarget);

    // 最終的に止まりたいカメラ位置
    float desiredDistance = 40.0f;
    XMVECTOR targetEye = xtarget + dir * desiredDistance;

    // なめらか補間（ここが肝）
    float zoomLerp = 0.4f * static_cast<float>(elapsed_time);
    xposition = XMVectorLerp(xposition, targetEye, zoomLerp);
    // ---- Yイージング処理 ----
    if (m_YEaseActive)
    {
        m_YEaseTime += static_cast<float>(elapsed_time);
        float t = m_YEaseTime / m_YEaseDuration;

        if (t >= 1.0f)
        {
            t = 1.0f;
            m_YEaseActive = false;
        }

        float curve = EaseInOut(t);

        // 下がって → 上がる
        float yOffset = (t < 0.5f)
            ? curve * m_YDown
            : curve * m_YUp;

        float newY = m_YStart + yOffset;

        xposition = XMVectorSetY(xposition, newY);
    }

    // 常にターゲットを見る
   // xfront = XMVector3Normalize(xtarget - xposition);

    // upベクトルは常に固定
    xup = XMVectorSet(0, 1, 0, 0);
    // なめらか補間
   // xposition = XMVectorLerp(xposition, targetEye, zoomLerp);

    // 
    const float MIN_CAMERA_HEIGHT = 5.0f;
    if (XMVectorGetY(xposition) < MIN_CAMERA_HEIGHT)
    {
        xposition = XMVectorSetY(xposition, MIN_CAMERA_HEIGHT);
    }

    // 常にターゲットを見る
    xfront = XMVector3Normalize(xtarget - xposition);


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



    // 値をメンバに戻す
    XMStoreFloat3(&GetPositionReference(), xposition);
    XMStoreFloat3(&m_Front, xfront);
    XMStoreFloat3(&m_right, xright);
    XMStoreFloat3(&m_up, xup);

}

void DirectionCamera::SetMatrix()
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

void DirectionCamera::ResetCamera()
{
    SetPosition(m_InitPosition);
    m_Front = m_InitFront;
    m_up = m_InitUp;

    m_YEaseActive = false;
    m_YEaseTime = 0.0f;

    m_Target = m_Current_Target;
}


void DirectionCamera::StartYEase()
{
    m_YEaseActive = true;
    m_YEaseTime = 0.0f;
    m_YStart = GetPosition().y;
}

float EaseInOut(float t)
{
    // 0→1→0 の山形カーブ
    return sinf(t * XM_PI);
}

