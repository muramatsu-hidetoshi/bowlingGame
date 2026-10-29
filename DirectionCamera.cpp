/*==========================================================================
    投球中の演出用カメラを制御する [directionCamera.cpp]

    ボールなどの注視対象を見ながらカメラ位置を補間し、
    投球中のズームや上下移動によって演出を行う。
==========================================================================*/

#include "directionCamera.h"
#include <DirectXMath.h>
#include "keylogger.h"
#include "direct3d.h"
#include "shader3d.h"

using namespace DirectX;

// 初期位置と注視対象を指定して、演出用カメラを生成する
DirectionCamera::DirectionCamera(
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
    : m_Target(target), m_Current_Target(target)
{
    // リセット時に使用する初期カメラ情報を保存する
    m_InitPosition = position;
    m_InitFront = m_Front;
    m_InitUp = m_up;

    // カメラ位置から注視対象へ向かう正面方向を計算する
    XMVECTOR xtarget = XMLoadFloat3(&target);
    XMVECTOR xposition = XMLoadFloat3(&position);
    XMVECTOR xFront = XMVector3Normalize(xtarget - xposition);
    XMStoreFloat3(&m_Front, xFront);

    // カメラ位置を設定する
    SetPosition(position);

    // ワールド座標の上方向を設定する
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    XMStoreFloat3(&m_up, XMLoadFloat3(&up));

    // 上方向と正面方向の外積から、カメラの右方向を求める
    XMVECTOR xright = XMVector3Cross(XMLoadFloat3(&m_up), xFront);
    xright = XMVector3Normalize(xright);
    XMStoreFloat3(&m_right, xright);

    // 視野角を45度に設定する
    m_fov = XM_PI / 4.0f;
}

// 経過時間に応じてカメラ位置と向きを更新する
void DirectionCamera::Update(double elapsed_time)
{
    // 移動速度と回転速度を、経過時間に応じた値として計算する
    const float MOVESPEED = 5.0f * static_cast<float>(elapsed_time);
    const float ROTSPEED =
        XMConvertToRadians(60.0f) * static_cast<float>(elapsed_time);

    // カメラの方向ベクトルと位置を読み込む
    XMVECTOR xfront = XMLoadFloat3(&m_Front);
    XMVECTOR xright = XMLoadFloat3(&m_right);
    XMVECTOR xup = XMLoadFloat3(&m_up);
    XMVECTOR xposition = XMLoadFloat3(&GetPosition());

    // 注視対象の位置を取得する
    XMVECTOR xtarget = XMLoadFloat3(&m_Target);

    // カメラから注視対象へ向かう方向とは逆向きのベクトルを求める
    // この方向を基準に、注視対象から一定距離離れた位置を計算する
    XMVECTOR dir = XMVector3Normalize(xposition - xtarget);

    // カメラを注視対象から離す距離
    float desiredDistance = 40.0f;

    // 注視対象から指定距離離れた、カメラの目標位置を求める
    XMVECTOR targetEye = xtarget + dir * desiredDistance;

    // 現在位置から目標位置へ向けて、カメラ位置を補間する
    float zoomLerp = 0.4f * static_cast<float>(elapsed_time);
    xposition = XMVectorLerp(xposition, targetEye, zoomLerp);

    // Y方向のイージング演出が有効な場合、上下移動を行う
    if (m_YEaseActive)
    {
        // 演出開始からの経過時間を加算する
        m_YEaseTime += static_cast<float>(elapsed_time);

        // 演出全体に対する進行度を0～1で求める
        float t = m_YEaseTime / m_YEaseDuration;

        // 演出が終了時間に達したら進行度を1に固定する
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_YEaseActive = false;
        }

        // イージング関数で、上下移動の変化を滑らかにする
        float curve = EaseInOut(t);

        // 進行度の前半では下方向、後半では上方向への移動量を求める
        float yOffset = (t < 0.5f)
            ? curve * m_YDown
            : curve * m_YUp;

        // 初期のY座標に移動量を加えて、新しい高さを設定する
        float newY = m_YStart + yOffset;
        xposition = XMVectorSetY(xposition, newY);
    }

    // カメラの上方向をワールド座標の上方向に固定する
    xup = XMVectorSet(0, 1, 0, 0);

    // カメラが低くなりすぎないよう、最低高度を設定する
    const float MIN_CAMERA_HEIGHT = 5.0f;
    if (XMVectorGetY(xposition) < MIN_CAMERA_HEIGHT)
    {
        xposition = XMVectorSetY(xposition, MIN_CAMERA_HEIGHT);
    }

    // カメラ位置から注視対象へ向かう正面方向を計算する
    xfront = XMVector3Normalize(xtarget - xposition);

    // 正面・右・上の各方向を正規直交化する
    // 各ベクトルの長さを1にし、互いに直交するように整える
    xfront = XMVector3Normalize(xfront);
    xright = XMVector3Normalize(XMVector3Cross(xup, xfront));
    xup = XMVector3Normalize(XMVector3Cross(xfront, xright));

    // キー入力によるカメラ移動処理
    // 現在はコメントアウトされているため、キー操作による移動は行わない
    /*
    if (KeyLogger_IsPressed(KK_W))
    {
        xposition += XMVector3Normalize(XMVectorSetY(xfront, 0.0f)) * MOVESPEED;
    }
    if (KeyLogger_IsPressed(KK_S))
    {
        xposition -= XMVector3Normalize(XMVectorSetY(xfront, 0.0f)) * MOVESPEED;
    }
    if (KeyLogger_IsPressed(KK_A))
    {
        xposition -= xright * MOVESPEED;
    }
    if (KeyLogger_IsPressed(KK_D))
    {
        xposition += xright * MOVESPEED;
    }
    if (KeyLogger_IsPressed(KK_Q))
    {
        xposition += xup * MOVESPEED;
    }
    if (KeyLogger_IsPressed(KK_E))
    {
        xposition -= xup * MOVESPEED;
    }
    */

    // 計算した位置と方向ベクトルをメンバ変数に反映する
    XMStoreFloat3(&GetPositionReference(), xposition);
    XMStoreFloat3(&m_Front, xfront);
    XMStoreFloat3(&m_right, xright);
    XMStoreFloat3(&m_up, xup);
}

// カメラのビュー行列と射影行列を設定する
void DirectionCamera::SetMatrix()
{
    // カメラ位置・正面方向・上方向からビュー行列を作成する
    XMMATRIX mtxView =
        XMMatrixLookAtLH(
            XMLoadFloat3(&GetPosition()),
            XMLoadFloat3(&GetPosition()) + XMLoadFloat3(&m_Front),
            XMLoadFloat3(&m_up));

    // 作成したビュー行列をカメラに設定する
    SetViewMatrix(mtxView);

    // バックバッファの幅と高さからアスペクト比を求める
    float w = static_cast<float>(Direct3D_GetBackBufferWidth());
    float h = static_cast<float>(Direct3D_GetBackBufferHeight());

    // 視野角・アスペクト比・描画範囲を指定して射影行列を作成する
    XMMATRIX mtxProj =
        XMMatrixPerspectiveFovLH(m_fov, w / h, 0.1f, 1000.0f);

    // 作成した射影行列をカメラに設定する
    SetProjectionMatrix(mtxProj);
}

// カメラ位置と方向を初期状態に戻す
void DirectionCamera::ResetCamera()
{
    SetPosition(m_InitPosition);
    m_Front = m_InitFront;
    m_up = m_InitUp;

    // Y方向の演出状態と経過時間をリセットする
    m_YEaseActive = false;
    m_YEaseTime = 0.0f;

    // 注視対象を初期値に戻す
    m_Target = m_Current_Target;
}

// Y方向の上下移動演出を開始する
void DirectionCamera::StartYEase()
{
    // 演出を有効にし、経過時間をリセットする
    m_YEaseActive = true;
    m_YEaseTime = 0.0f;

    // 演出開始時のカメラの高さを保存する
    m_YStart = GetPosition().y;
}

// Y方向の演出に使用する変化量を計算する
float EaseInOut(float t)
{
    // sinカーブを利用して、0から増加し、その後0へ戻る変化を作る
    return sinf(t * XM_PI);
}

