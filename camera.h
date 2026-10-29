/*==========================================================================
    カメラインターフェイス [camera.h]

    【ファイルの役割】
    カメラに共通する機能を定義する基底クラス。

    派生クラスがカメラの位置やビュー行列、射影行列を設定できるようにし、
    頂点シェーダーに行列を渡すための定数バッファを管理する。

    【主な機能】
    ・カメラの位置を管理する
    ・ビュー行列と射影行列を設定する
    ・カメラの数に応じて定数バッファの生成と解放を管理する
==========================================================================*/

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class Camera
{
private:
    // ビュー行列を格納する頂点シェーダー用定数バッファ
    static ID3D11Buffer* m_pVSConstantBuffer1;

    // 射影行列を格納する頂点シェーダー用定数バッファ
    static ID3D11Buffer* m_pVSConstantBuffer2;

    // Cameraクラスのインスタンス数を管理する
    // すべてのインスタンスが破棄されたときに定数バッファを解放する
    static int m_ReferenceCount;

    // 現在のカメラのビュー行列
    DirectX::XMMATRIX m_viewMatrix{};

    // カメラのワールド座標
    DirectX::XMFLOAT3 m_Pos{};

public:
    // カメラの初期化処理
    Camera();

    // カメラの終了処理
    virtual ~Camera();

    // カメラの更新処理
    // 派生クラスで必要に応じてオーバーライドする
    virtual void Update(double elapsed_time) {};

    // カメラの行列を設定する
    // 派生クラスで具体的な処理を実装する
    virtual void SetMatrix() = 0;

    // 現在のビュー行列を取得する
    const DirectX::XMMATRIX& GetViewMatrix() const
    {
        return m_viewMatrix;
    }

    // カメラの位置を取得する
    const DirectX::XMFLOAT3& GetPosition() const
    {
        return m_Pos;
    }

protected:
    // 派生クラスからカメラの位置を直接変更するための参照を取得する
    DirectX::XMFLOAT3& GetPositionReference()
    {
        return m_Pos;
    }

    // 射影行列を設定し、頂点シェーダーに渡す
    void SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

    // ビュー行列を設定し、頂点シェーダーに渡す
    void SetViewMatrix(const DirectX::XMMATRIX& matrix);

    // カメラの位置を設定する
    void SetPosition(const DirectX::XMFLOAT3& position)
    {
        m_Pos = position;
    }
};


