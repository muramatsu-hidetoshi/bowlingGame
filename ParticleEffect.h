/*==========================================================================

パーティクルエフェクトシステム


---------------------------------------------------------------------------
多数の粒子を使った派手なエフェクトシステム

=========================================================================*/

#pragma once

#include <DirectXMath.h>

// パーティクルエフェクトの設定
struct ParticleEffectParams
{
    int textureId;                      // 使用するテクスチャID
    DirectX::XMFLOAT4 startColor;       // 開始色
    DirectX::XMFLOAT4 endColor;         // 終了色

    // パーティクル生成設定
    int particleCount;                  // パーティクル数
    float emitRadius;                   // 発生半径
    float emitHeight;                   // 発生高さ範囲

    // パーティクル動き設定
    float horizontalSpeed;              // 水平回転速度
    float verticalSpeed;                // 上昇速度
    float verticalRange;                // 上下移動範囲
    float particleScale;                // パーティクルサイズ
    float randomness;                   // ランダム性 (0.0~1.0)

    // アニメーション設定
    float rotationSpeed;                // 個別回転速度
    float pulseSpeed;                   // 脈動速度
    float pulseAmount;                  // 脈動の強さ

    bool useAdditiveBlend;              // 加算合成を使用
};

// デフォルト設定
ParticleEffectParams ParticleEffect_GetDefaultParams();

// 初期化・終了
void ParticleEffect_Initialize();
void ParticleEffect_Finalize();

// 更新
void ParticleEffect_Update(double elapsedTime);

// 描画
void ParticleEffect_Draw(
    const ParticleEffectParams& params,
    const DirectX::XMMATRIX& mtxWorld
);

// プリセット
ParticleEffectParams ParticleEffect_PresetFire();       // 炎
ParticleEffectParams ParticleEffect_PresetSpark();      // 火花
ParticleEffectParams ParticleEffect_PresetMagic();      // 魔法
ParticleEffectParams ParticleEffect_PresetHeal();       // 回復
ParticleEffectParams ParticleEffect_PresetExplosion();  // 爆発