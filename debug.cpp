/*==========================================================================
    デバッグカメラの実装 [debug.cpp]

    【ファイルの役割】
    DebugCameraクラスの初期化・更新・行列設定を実装する。

    カメラの位置と向きからビュー行列を作成し、
    画面の縦横比と視野角から射影行列を作成する。

    デバッグビルドでは矢印キーによるカメラの回転操作を行う。
==========================================================================*/

#include "debug.h"
#include <DirectXMath.h>
#include "keylogger.h"
#include "direct3d.h"
#include "shader3d.h"

using namespace DirectX;


// デバッグカメラの初期化
// position：カメラの初期位置
// target  ：カメラが向く対象の位置
DebugCamera::DebugCamera(
    const XMFLOAT3& position,
    const XMFLOAT3& target)
{
    // 注視対象の位置とカメラの位置をベクトルに変換する
    XMVECTOR xtarget = XMLoadFloat3(&target);
    XMVECTOR xposition = XMLoadFloat3(&position);

    // カメラの位置から注視対象に向かう方向ベクトルを求める
    // 正規化することで、ベクトルの長さを1にする
    XMVECTOR xFront = XMVector3Normalize(xtarget - xposition);

    // 正面方向をメンバ変数に保存する
    XMStoreFloat3(&m_Front, xFront);

    // 基底クラスにカメラの位置を設定する
    SetPosition(position);

    // ワールド座標系の上方向を設定する
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    XMStoreFloat3(&m_up, XMLoadFloat3(&up));

    // 上方向と正面方向の外積から、カメラの右方向を求める
    XMVECTOR xright = XMVector3Cross(
        XMLoadFloat3(&m_up),
        xFront
    );

    // 右方向のベクトルを正規化する
    xright = XMVector3Normalize(xright);

    // 右方向をメンバ変数に保存する
    XMStoreFloat3(&m_right, xright);

    // 視野角を45度に設定する
    // DirectXMathでは角度をラジアンで指定する
    m_fov = XM_PI / 4.0f;
}


// デバッグカメラの状態を更新する
void DebugCamera::Update(double elapsed_time)
{
    // 経過時間に応じて移動速度と回転速度を計算する
    // フレームレートに左右されにくい処理にするため、
    // どちらも経過時間を乗算する
    const float MOVESPEED =
        5.0f * static_cast<float>(elapsed_time);

    // 1秒あたり60度回転する速度をラジアンに変換する
    const float ROTSPEED =
        XMConvertToRadians(60.0f) *
        static_cast<float>(elapsed_time);

    // メンバ変数のベクトルをXMVECTOR型に変換する
    // DirectXMathのベクトル演算に使用する
    XMVECTOR xfront = XMLoadFloat3(&m_Front);
    XMVECTOR xright = XMLoadFloat3(&m_right);
    XMVECTOR xup = XMLoadFloat3(&m_up);
    XMVECTOR xposition = XMLoadFloat3(&GetPosition());

#ifdef _DEBUG

    // 右キー：Y軸を中心に右方向へ回転する
    if (KeyLogger_IsPressed(KK_RIGHT))
    {
        XMMATRIX rot = XMMatrixRotationY(ROTSPEED);

        // 正面方向と右方向を回転させる
        xfront = XMVector3TransformNormal(xfront, rot);
        xright = XMVector3TransformNormal(xright, rot);
    }

    // 左キー：Y軸を中心に左方向へ回転する
    if (KeyLogger_IsPressed(KK_LEFT))
    {
        XMMATRIX rot = XMMatrixRotationY(-ROTSPEED);

        // 正面方向と右方向を回転させる
        xfront = XMVector3TransformNormal(xfront, rot);
        xright = XMVector3TransformNormal(xright, rot);
    }

    // 上キー：右方向の軸を中心に上向きへ回転する
    if (KeyLogger_IsPressed(KK_UP))
    {
        // 右方向を回転軸として、正面方向を回転させる
        XMMATRIX rot = XMMatrixRotationAxis(xright, -ROTSPEED);
        xfront = XMVector3TransformNormal(xfront, rot);
    }

    // 下キー：右方向の軸を中心に下向きへ回転する
    if (KeyLogger_IsPressed(KK_DOWN))
    {
        // 右方向を回転軸として、正面方向を回転させる
        XMMATRIX rot = XMMatrixRotationAxis(xright, ROTSPEED);
        xfront = XMVector3TransformNormal(xfront, rot);
    }

#endif // _DEBUG

    // 上方向をワールド座標系のY軸方向に設定する
    // カメラが傾き続けないようにする
    xup = XMVectorSet(0, 1, 0, 0);

    // 正面方向を正規化する
    xfront = XMVector3Normalize(xfront);

    // 上方向と正面方向から右方向を再計算し、正規化する
    xright = XMVector3Normalize(
        XMVector3Cross(xup, xfront)
    );

    // 正面方向と右方向から上方向を再計算し、正規化する
    // 3つの方向ベクトルが直交するように調整する
    xup = XMVector3Normalize(
        XMVector3Cross(xfront, xright)
    );

    // 更新後のカメラ位置と方向ベクトルをメンバ変数に保存する
    XMStoreFloat3(&GetPositionReference(), xposition);
    XMStoreFloat3(&m_Front, xfront);
    XMStoreFloat3(&m_right, xright);
    XMStoreFloat3(&m_up, xup);
}


// デバッグカメラのビュー行列と射影行列を設定する
void DebugCamera::SetMatrix()
{
    // カメラの位置と正面方向から注視点を求める
    // 正面方向に1単位進んだ位置を注視点とする
    XMMATRIX mtxView = XMMatrixLookAtLH(
        XMLoadFloat3(&GetPosition()),
        XMLoadFloat3(&GetPosition()) +
        XMLoadFloat3(&m_Front),
        XMLoadFloat3(&m_up)
    );

    // 作成したビュー行列を基底クラスに設定する
    SetViewMatrix(mtxView);

    // バックバッファの幅と高さを取得する
    // 画面の縦横比を射影行列の計算に使用する
    float w = static_cast<float>(
        Direct3D_GetBackBufferWidth()
        );

    float h = static_cast<float>(
        Direct3D_GetBackBufferHeight()
        );

    // 透視投影用の射影行列を作成する
    // m_fov：視野角
    // w / h：画面の縦横比
    // 0.1f：描画する最も近い距離
    // 1000.0f：描画する最も遠い距離
    XMMATRIX mtxProj = XMMatrixPerspectiveFovLH(
        m_fov,
        w / h,
        0.1f,
        1000.0f
    );

    // 作成した射影行列を基底クラスに設定する
    SetProjectionMatrix(mtxProj);
}