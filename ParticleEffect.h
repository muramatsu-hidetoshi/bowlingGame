


/*==========================================================================
パーティクルエフェクトの設定と関数宣言 [particleEffect.h]

パーティクルの外観や動きを設定する構造体と、
エフェクトの初期化・更新・描画・終了処理を宣言する。
==========================================================================*/

#pragma once

#include <DirectXMath.h>

// パーティクルエフェクトの設定
struct ParticleEffectParams
{
    int textureId;                       // 使用するテクスチャID
    DirectX::XMFLOAT4 startColor;        // 開始時の色
    DirectX::XMFLOAT4 endColor;          // 終了時の色

    // パーティクルの発生設定
    int particleCount;                   // パーティクル数
    float emitRadius;                    // 発生位置の半径
    float emitHeight;                    // 上下方向の揺らぎの大きさ

    // パーティクルの移動設定
    float horizontalSpeed;               // 水平方向の回転速度
    float verticalSpeed;                 // 上昇運動の速度
    float verticalRange;                 // 上下方向の移動範囲
    float particleScale;                 // 基本サイズ
    float randomness;                    // 動きに加えるランダム性（0.0～1.0）

    // パーティクルのアニメーション設定
    float rotationSpeed;                 // 回転速度
    float pulseSpeed;                    // 脈動の速度
    float pulseAmount;                   // 脈動によるサイズ変化の強さ

    bool useAdditiveBlend;               // 加算合成を使用するかどうか
};

// デフォルト設定を取得する
ParticleEffectParams ParticleEffect_GetDefaultParams();

// 初期化・終了処理
void ParticleEffect_Initialize();
void ParticleEffect_Finalize();

// 経過時間に基づいて更新する
void ParticleEffect_Update(double elapsedTime);

// 指定された設定とワールド行列を使って描画する
void ParticleEffect_Draw(
    const ParticleEffectParams& params,
    const DirectX::XMMATRIX& mtxWorld
);

// エフェクトの種類ごとの設定を取得する
ParticleEffectParams ParticleEffect_PresetFire();       // 炎
ParticleEffectParams ParticleEffect_PresetSpark();      // 火花
ParticleEffectParams ParticleEffect_PresetMagic();      // 魔法
ParticleEffectParams ParticleEffect_PresetHeal();       // 回復
ParticleEffectParams ParticleEffect_PresetExplosion();  // 爆発
