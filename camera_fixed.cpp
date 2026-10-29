/*==========================================================================
    固定カメラの実装 [camera_fixed.cpp]

    【ファイルの役割】
    FixedCameraクラスの処理を実装する。

    カメラの位置と注視点からビュー行列を作成し、
    画面の縦横比などを指定して射影行列を作成する。

    また、注視点がカメラ切り替え用の判定領域に入っているかを確認する。
==========================================================================*/

#include "camera_fixed.h"
#include "direct3d.h"

using namespace DirectX;

// カメラの描画に必要な行列を設定する
void FixedCamera::SetMatrix()
{
    // カメラの位置、注視点、上方向ベクトルからビュー行列を作成する。
    // XMMatrixLookAtLHは左手座標系のビュー行列を生成する。
    XMMATRIX mtxView = XMMatrixLookAtLH(
        XMLoadFloat3(&GetPosition()), // カメラのワールド座標
        XMLoadFloat3(&m_target),      // カメラが注視する対象の座標
        XMVECTOR({ 0, 1, 0 })        // カメラの上方向を示すベクトル
    );

    // 作成したビュー行列を基底クラスに設定する
    SetViewMatrix(mtxView);

    // バックバッファの幅と高さを取得する
    // 画面の縦横比を射影行列の計算に使用する
    float w = static_cast<float>(Direct3D_GetBackBufferWidth());
    float h = static_cast<float>(Direct3D_GetBackBufferHeight());

    // 透視投影用の射影行列を作成する
    // 第1引数：垂直方向の視野角（ラジアン）
    // 第2引数：画面の縦横比
    // 第3引数：描画する最も近い距離
    // 第4引数：描画する最も遠い距離
    XMMATRIX mtxProj = XMMatrixPerspectiveFovLH(
        1.0f,
        w / h,
        0.1f,
        1000.0f
    );

    // 作成した射影行列を基底クラスに設定する
    SetProjectionMatrix(mtxProj);
}

// 注視点がカメラ切り替え用の判定領域に入っているか確認する
bool FixedCamera::IsOverlap()
{
    // 注視点の座標をAABBの判定に渡し、
    // 判定領域と重なっているかどうかを返す
    return m_aabb.IsOverlap({ m_target });
}