/*==========================================================================
    カメラ機能の実装 [Camera.cpp]

    【ファイルの役割】
    Cameraクラスの共通処理を実装する。

    ビュー行列と射影行列を定数バッファに格納し、
    頂点シェーダーから参照できるように設定する。

    定数バッファはカメラ間で共有し、参照カウントによって管理する。
==========================================================================*/

#include "camera.h"
#include "direct3d.h"
#include <DirectXMath.h>

using namespace DirectX;

// staticメンバ変数の実体を定義する
// staticメンバはクラス全体で共有される
ID3D11Buffer* Camera::m_pVSConstantBuffer1 = nullptr;
ID3D11Buffer* Camera::m_pVSConstantBuffer2 = nullptr;
int Camera::m_ReferenceCount = 0;


// カメラの初期化処理
Camera::Camera()
{
    // 定数バッファがまだ作成されていない場合に限り作成する
    // 複数のカメラで同じ定数バッファを共有する
    if (!m_pVSConstantBuffer1)
    {
        // 定数バッファの設定情報を初期化する
        D3D11_BUFFER_DESC buffer_desc{};

        // 行列を格納できるサイズを指定する
        buffer_desc.ByteWidth = sizeof(XMFLOAT4X4);

        // 頂点シェーダー用の定数バッファとして使用する
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        // ビュー行列用の定数バッファを作成する
        Direct3D_GetDevice()->CreateBuffer(
            &buffer_desc,
            nullptr,
            &m_pVSConstantBuffer1
        );

        // 射影行列用の定数バッファを作成する
        Direct3D_GetDevice()->CreateBuffer(
            &buffer_desc,
            nullptr,
            &m_pVSConstantBuffer2
        );
    }

    // カメラのインスタンス数を増やす
    m_ReferenceCount++;
}


// カメラの終了処理
Camera::~Camera()
{
    // カメラのインスタンス数を減らす
    m_ReferenceCount--;

    // カメラがすべて破棄された場合、共有している定数バッファを解放する
    if (m_ReferenceCount == 0)
    {
        SAFE_RELEASE(m_pVSConstantBuffer2);
        SAFE_RELEASE(m_pVSConstantBuffer1);
    }
}


// 射影行列を設定する
void Camera::SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
    // GPUに渡すため、行列を格納する構造体を用意する
    XMFLOAT4X4 transpose;

    // 行列を転置して、定数バッファに格納できる形式に変換する
    XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

    // 射影行列を定数バッファに転送する
    Direct3D_GetContext()->UpdateSubresource(
        m_pVSConstantBuffer2,
        0,
        nullptr,
        &transpose,
        0,
        0
    );

    // スロット2に射影行列用の定数バッファを設定する
    Direct3D_GetContext()->VSSetConstantBuffers(
        2,
        1,
        &m_pVSConstantBuffer2
    );
}


// ビュー行列を設定する
void Camera::SetViewMatrix(const DirectX::XMMATRIX& matrix)
{
    // カメラクラス内にもビュー行列を保存する
    m_viewMatrix = matrix;

    // GPUに渡すため、行列を格納する構造体を用意する
    XMFLOAT4X4 transpose;

    // 行列を転置して、定数バッファに格納できる形式に変換する
    XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

    // ビュー行列を定数バッファに転送する
    Direct3D_GetContext()->UpdateSubresource(
        m_pVSConstantBuffer1,
        0,
        nullptr,
        &transpose,
        0,
        0
    );

    // スロット1にビュー行列用の定数バッファを設定する
    Direct3D_GetContext()->VSSetConstantBuffers(
        1,
        1,
        &m_pVSConstantBuffer1
    );
}