
/*==========================================================================
トレイルベース爆発エフェクト [trailExplosion.h]

既存のTrailシステムを利用した爆発エフェクトを定義する。
==========================================================================*/

#pragma once
#include <DirectXMath.h>

// 爆発エフェクトの形状
enum class TrailExplosionType
{
    RADIAL, // 平面上に放射状に広がる
    SPHERE, // 球状に広がる
    CONE_UP,// 上向きの円錐状に広がる
    RING,   // リング状に広がる
    SPIRAL  // 螺旋状に広がる
};

// 爆発エフェクトの生成設定
struct TrailExplosionSettings
{
    DirectX::XMFLOAT3 position;       // 爆発の発生位置
    TrailExplosionType type;           // 爆発の形状
    int trailCount;                    // 生成するトレイル数
    float speed;                       // トレイルの基本速度
    float maxDistance;                 // 最大到達距離の設定値
    DirectX::XMFLOAT4 startColor;      // トレイル開始時の色
    DirectX::XMFLOAT4 endColor;        // トレイル終了時の色
    float duration;                    // 爆発の持続時間（秒）
    bool useGravity;                   // 重力を適用するかどうか
    float scale;                       // トレイル全体のスケール
    DirectX::XMFLOAT3 rotation;        // 方向に適用する回転角度（ラジアン）
};

// 爆発エフェクトの初期化
void TrailExplosion_Initialize();

// 爆発エフェクトの終了とリソース解放
void TrailExplosion_Finalize();

// 爆発エフェクトの位置や寿命を更新する
void TrailExplosion_Update(float deltaTime);

// 爆発エフェクトを描画する
void TrailExplosion_Draw();

// 設定構造体を使って爆発エフェクトを生成する
void TrailExplosion_Create(const TrailExplosionSettings& settings);

// 放射状の爆発を生成する
void TrailExplosion_CreateRadial(
    const DirectX::XMFLOAT3& position,
    float scale = 1.0f
);

// 球状の爆発を生成する
void TrailExplosion_CreateSphere(
    const DirectX::XMFLOAT3& position,
    float scale = 1.0f
);

// 上向き円錐状の爆発を生成する
void TrailExplosion_CreateConeUp(
    const DirectX::XMFLOAT3& position,
    float scale = 1.0f
);

// リング状の爆発を生成する
void TrailExplosion_CreateRing(
    const DirectX::XMFLOAT3& position,
    float scale = 1.0f
);

// 螺旋状の爆発を生成する
void TrailExplosion_CreateSpiral(
    const DirectX::XMFLOAT3& position,
    float scale = 1.0f
);

// トレイル描画に使用するカメラ位置を設定する
void TrailExplosion_SetCameraPosition(const DirectX::XMFLOAT3& position);

// 管理している爆発数を取得する
int TrailExplosion_GetActiveCount();
