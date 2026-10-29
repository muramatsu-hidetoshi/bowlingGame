/*==========================================================================

パーティクルエフェクト実装[particleEffect.cpp]


---------------------------------------------------------------------------

=========================================================================*/

#include "particleEffect.h"
#include "billBord.h"
#include "texture.h"
#include <cmath>

using namespace DirectX;

static double g_totalTime = 0.0;

ParticleEffectParams ParticleEffect_GetDefaultParams()
{
    ParticleEffectParams params{};
    params.textureId = Texture_Load(L"rom\\Texture\\gra_effect_lightA.png");
    params.startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    params.endColor = { 1.0f, 1.0f, 1.0f, 0.0f };

    params.particleCount = 50;
    params.emitRadius = 2.0f;
    params.emitHeight = 1.0f;

    params.horizontalSpeed = 2.0f;
    params.verticalSpeed = 2.0f;
    params.verticalRange = 3.0f;
    params.particleScale = 0.5f;
    params.randomness = 0.5f;

    params.rotationSpeed = 3.0f;
    params.pulseSpeed = 4.0f;
    params.pulseAmount = 0.3f;

    params.useAdditiveBlend = true;

    return params;
}

void ParticleEffect_Initialize()
{
    g_totalTime = 0.0;
}

void ParticleEffect_Finalize()
{
}

void ParticleEffect_Update(double elapsedTime)
{
    // 各パーティクルの経過時間を進め、寿命を終えたものを取り除く。
    g_totalTime += elapsedTime;
}

void ParticleEffect_Draw(
    const ParticleEffectParams& params,
    const DirectX::XMMATRIX& mtxWorld)
{
    // パーティクルループ
    for (int i = 0; i < params.particleCount; ++i)
    {
        // 疑似乱数生成
        int seed = static_cast<int>(g_totalTime * 1000.0) + i * 1234567;
        auto getRandom = [&seed]() -> float {
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            return static_cast<float>(seed) / static_cast<float>(0x7fffffff);
            };

        // 初期角度 (円周上に均等配置 + ランダム要素)
        float baseAngle = (static_cast<float>(i) / static_cast<float>(params.particleCount)) * XM_2PI;
        float angleOffset = getRandom() * params.randomness * XM_2PI;

        // 水平回転
        float angle = static_cast<float>(g_totalTime * params.horizontalSpeed) + baseAngle + angleOffset;

        // 半径の変動
        float radiusVar = 1.0f + (getRandom() - 0.5f) * params.randomness;
        float radiusWave = std::sinf(static_cast<float>(g_totalTime * 2.0 + i * 0.3f)) * 0.2f;
        float radius = params.emitRadius * radiusVar * (1.0f + radiusWave);

        // 上昇運動 (ループして下から上へ)
        float lifeProgress = std::fmodf(static_cast<float>(g_totalTime * params.verticalSpeed + i * 0.1f), 1.0f);
        float heightBase = lifeProgress * params.verticalRange;

        // 波動的な上下動
        float waveHeight = std::sinf(static_cast<float>(g_totalTime * 3.0 + i * 0.5f)) * params.emitHeight;

        // 位置計算
        XMFLOAT3 offset;
        offset.x = std::cosf(angle) * radius;
        offset.y = heightBase + waveHeight - params.verticalRange * 0.5f;
        offset.z = std::sinf(angle) * radius;

        // 個別回転
        float rotation = static_cast<float>(g_totalTime * params.rotationSpeed + i * 1.5f);

        // 脈動
        float pulsePhase = static_cast<float>(g_totalTime * params.pulseSpeed + i * 0.2f);
        float pulse = 1.0f + std::sinf(pulsePhase) * params.pulseAmount;

        // スケール (ライフタイムで変化)
        float scaleByLife = 1.0f - std::powf(std::abs(lifeProgress - 0.5f) * 2.0f, 2.0f) * 0.5f;
        float scale = params.particleScale * pulse * scaleByLife;

        // 色 (ライフタイムで変化)
        XMFLOAT4 color;
        color.x = params.startColor.x + (params.endColor.x - params.startColor.x) * lifeProgress;
        color.y = params.startColor.y + (params.endColor.y - params.startColor.y) * lifeProgress;
        color.z = params.startColor.z + (params.endColor.z - params.startColor.z) * lifeProgress;
        color.w = (params.startColor.w + (params.endColor.w - params.startColor.w) * lifeProgress) * scaleByLife;

        // ワールド行列計算
        XMMATRIX mtxRot = XMMatrixRotationZ(rotation);
        XMMATRIX mtxTrans = XMMatrixTranslation(offset.x, offset.y, offset.z);
        XMMATRIX mtxParticle = mtxRot * mtxTrans * mtxWorld;

        // 描画
        Billboard_Draw(
            params.textureId,
            mtxParticle,
            XMFLOAT2(0.0f, 0.0f),
            XMFLOAT2(static_cast<float>(Texture_GetWidht(params.textureId)),
                static_cast<float>(Texture_GetHeight(params.textureId))),
            XMFLOAT2(scale, scale),
            color,
            XMFLOAT2(0.0f, 0.0f)
        );
    }
}

// プリセット: 炎エフェクト
ParticleEffectParams ParticleEffect_PresetFire()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();
    params.startColor = { 1.0f, 0.9f, 0.3f, 1.0f };    // 明るい黄色
    params.endColor = { 1.0f, 0.2f, 0.0f, 0.0f };      // 赤→透明

    params.particleCount = 80;
    params.emitRadius = 0.3f;
    params.emitHeight = 0.5f;

    params.horizontalSpeed = 3.0f;
    params.verticalSpeed = 2.0f;
    params.verticalRange = 0.1f;
    params.particleScale = 0.6f;
    params.randomness = 0.6f;

    params.rotationSpeed = 5.0f;
    params.pulseSpeed = 6.0f;
    params.pulseAmount = 0.4f;

    params.useAdditiveBlend = true;

    return params;
}

// プリセット: 火花エフェクト
ParticleEffectParams ParticleEffect_PresetSpark()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();
    params.startColor = { 1.0f, 1.0f, 0.8f, 1.0f };    // 白
    params.endColor = { 1.0f, 0.5f, 0.0f, 0.0f };      // オレンジ→透明

    params.particleCount = 100;
    params.emitRadius = 0.8f;
    params.emitHeight = 0.3f;

    params.horizontalSpeed = 5.0f;
    params.verticalSpeed = 4.0f;
    params.verticalRange = 3.0f;
    params.particleScale = 0.3f;
    params.randomness = 0.8f;

    params.rotationSpeed = 8.0f;
    params.pulseSpeed = 10.0f;
    params.pulseAmount = 0.5f;

    params.useAdditiveBlend = true;

    return params;
}

// プリセット: 魔法エフェクト
ParticleEffectParams ParticleEffect_PresetMagic()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();
    params.startColor = { 0.6f, 0.3f, 1.0f, 1.0f };    // 紫
    params.endColor = { 0.3f, 0.6f, 1.0f, 0.0f };      // 青→透明

    params.particleCount = 60;
    params.emitRadius = 2.5f;
    params.emitHeight = 1.0f;

    params.horizontalSpeed = 1.5f;
    params.verticalSpeed = 1.5f;
    params.verticalRange = 4.0f;
    params.particleScale = 0.7f;
    params.randomness = 0.3f;

    params.rotationSpeed = 2.0f;
    params.pulseSpeed = 3.0f;
    params.pulseAmount = 0.3f;

    params.useAdditiveBlend = true;

    return params;
}

// プリセット: 回復エフェクト
ParticleEffectParams ParticleEffect_PresetHeal()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();
    params.startColor = { 0.3f, 1.0f, 0.3f, 1.0f };    // 明るい緑
    params.endColor = { 1.0f, 1.0f, 0.5f, 0.0f };      // 黄色→透明

    params.particleCount = 50;
    params.emitRadius = 1.8f;
    params.emitHeight = 0.8f;

    params.horizontalSpeed = 1.0f;
    params.verticalSpeed = 1.2f;
    params.verticalRange = 4.5f;
    params.particleScale = 0.5f;
    params.randomness = 0.4f;

    params.rotationSpeed = 1.5f;
    params.pulseSpeed = 4.0f;
    params.pulseAmount = 0.4f;

    params.useAdditiveBlend = true;

    return params;
}

// プリセット: 爆発エフェクト
ParticleEffectParams ParticleEffect_PresetExplosion()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();
    params.startColor = { 1.0f, 0.8f, 0.3f, 1.0f };    // 明るいオレンジ
    params.endColor = { 0.5f, 0.1f, 0.0f, 0.0f };      // 暗い赤→透明

    params.particleCount = 120;
    params.emitRadius = 3.0f;
    params.emitHeight = 2.0f;

    params.horizontalSpeed = 4.0f;
    params.verticalSpeed = 3.0f;
    params.verticalRange = 6.0f;
    params.particleScale = 0.8f;
    params.randomness = 0.9f;

    params.rotationSpeed = 6.0f;
    params.pulseSpeed = 8.0f;
    params.pulseAmount = 0.5f;

    params.useAdditiveBlend = true;

    return params;
}