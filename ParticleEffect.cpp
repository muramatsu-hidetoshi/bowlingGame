/*==========================================================================
パーティクルエフェクトの実装 [particleEffect.cpp]

複数のパーティクルを生成・描画し、時間に応じて位置、色、回転、
大きさを変化させる。
炎、火花、魔法、回復、爆発などのエフェクトをプリセットとして提供する。
==========================================================================*/

#include "particleEffect.h"
#include "billBord.h"
#include "texture.h"
#include <cmath>

using namespace DirectX;

// エフェクトの経過時間
static double g_totalTime = 0.0;

// デフォルトのパーティクル設定を取得する
ParticleEffectParams ParticleEffect_GetDefaultParams()
{
    // 構造体のメンバーを初期化する
    ParticleEffectParams params{};

    // パーティクルに使用するテクスチャを読み込む
    params.textureId = Texture_Load(L"rom\\Texture\\gra_effect_lightA.png");

    // 開始時は白色、終了時は透明になるように設定する
    params.startColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    params.endColor = { 1.0f, 1.0f, 1.0f, 0.0f };

    // パーティクルの生成数と発生範囲
    params.particleCount = 50;
    params.emitRadius = 2.0f;
    params.emitHeight = 1.0f;

    // パーティクルの移動速度や大きさ、ランダム性
    params.horizontalSpeed = 2.0f;
    params.verticalSpeed = 2.0f;
    params.verticalRange = 3.0f;
    params.particleScale = 0.5f;
    params.randomness = 0.5f;

    // 回転と脈動の設定
    params.rotationSpeed = 3.0f;
    params.pulseSpeed = 4.0f;
    params.pulseAmount = 0.3f;

    // 加算合成を使用するかどうか
    params.useAdditiveBlend = true;

    return params;
}

// 初期化処理
void ParticleEffect_Initialize()
{
    // エフェクトの経過時間を初期化する
    g_totalTime = 0.0;
}

// 終了処理
void ParticleEffect_Finalize()
{
    // 現状では追加の終了処理はない
}

// 更新処理
void ParticleEffect_Update(double elapsedTime)
{
    // 経過時間を加算し、パーティクルの動きに使用する
    g_totalTime += elapsedTime;
}

// パーティクルを描画する
void ParticleEffect_Draw(
    const ParticleEffectParams& params,
    const DirectX::XMMATRIX& mtxWorld)
{
    // 設定された数だけパーティクルを描画する
    for (int i = 0; i < params.particleCount; ++i)
    {
        // 経過時間とパーティクル番号から乱数の初期値を作成する
        int seed = static_cast<int>(g_totalTime * 1000.0) + i * 1234567;

        // 簡易的な疑似乱数を生成する関数
        auto getRandom = [&seed]() -> float {
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            return static_cast<float>(seed) / static_cast<float>(0x7fffffff);
            };

        // パーティクルを円周上に配置するための基本角度
        float baseAngle =
            (static_cast<float>(i) / static_cast<float>(params.particleCount))
            * XM_2PI;

        // 配置にランダムな角度を加える
        float angleOffset = getRandom() * params.randomness * XM_2PI;

        // 時間経過に応じて水平方向に回転させる
        float angle =
            static_cast<float>(g_totalTime * params.horizontalSpeed)
            + baseAngle + angleOffset;

        // パーティクルごとに発生半径を変化させる
        float radiusVar =
            1.0f + (getRandom() - 0.5f) * params.randomness;

        // 半径を周期的に変化させ、動きに揺らぎを加える
        float radiusWave =
            std::sinf(static_cast<float>(g_totalTime * 2.0 + i * 0.3f))
            * 0.2f;

        // 最終的な発生半径を計算する
        float radius =
            params.emitRadius * radiusVar * (1.0f + radiusWave);

        // 0～1の範囲でパーティクルの進行度を計算する
        // 1を超えると0に戻り、上昇運動を繰り返す
        float lifeProgress = std::fmodf(
            static_cast<float>(g_totalTime * params.verticalSpeed + i * 0.1f),
            1.0f);

        // 進行度に応じた基本の高さを求める
        float heightBase = lifeProgress * params.verticalRange;

        // 上下方向に波のような動きを加える
        float waveHeight =
            std::sinf(static_cast<float>(g_totalTime * 3.0 + i * 0.5f))
            * params.emitHeight;

        // パーティクルの位置を計算する
        XMFLOAT3 offset;
        offset.x = std::cosf(angle) * radius;
        offset.y = heightBase + waveHeight - params.verticalRange * 0.5f;
        offset.z = std::sinf(angle) * radius;

        // 時間経過とパーティクル番号に応じて回転させる
        float rotation =
            static_cast<float>(g_totalTime * params.rotationSpeed + i * 1.5f);

        // 脈動の位相を計算する
        float pulsePhase =
            static_cast<float>(g_totalTime * params.pulseSpeed + i * 0.2f);

        // サイン波を利用して大きさを周期的に変化させる
        float pulse = 1.0f + std::sinf(pulsePhase) * params.pulseAmount;

        // 進行度に応じてパーティクルの大きさを変化させる
        float scaleByLife =
            1.0f - std::powf(std::abs(lifeProgress - 0.5f) * 2.0f, 2.0f)
            * 0.5f;

        // 基本サイズに脈動と進行度による変化を適用する
        float scale = params.particleScale * pulse * scaleByLife;

        // 進行度に応じて開始色から終了色へ変化させる
        XMFLOAT4 color;
        color.x = params.startColor.x
            + (params.endColor.x - params.startColor.x) * lifeProgress;
        color.y = params.startColor.y
            + (params.endColor.y - params.startColor.y) * lifeProgress;
        color.z = params.startColor.z
            + (params.endColor.z - params.startColor.z) * lifeProgress;

        // アルファ値にも進行度とサイズの変化を反映する
        color.w = (params.startColor.w
            + (params.endColor.w - params.startColor.w) * lifeProgress)
            * scaleByLife;

        // パーティクルの回転行列と移動行列を作成する
        XMMATRIX mtxRot = XMMatrixRotationZ(rotation);
        XMMATRIX mtxTrans =
            XMMatrixTranslation(offset.x, offset.y, offset.z);

        // パーティクルの変換をワールド行列に適用する
        XMMATRIX mtxParticle = mtxRot * mtxTrans * mtxWorld;

        // ビルボードを使用してパーティクルを描画する
        Billboard_Draw(
            params.textureId,
            mtxParticle,
            XMFLOAT2(0.0f, 0.0f),
            XMFLOAT2(
                static_cast<float>(Texture_GetWidht(params.textureId)),
                static_cast<float>(Texture_GetHeight(params.textureId))),
            XMFLOAT2(scale, scale),
            color,
            XMFLOAT2(0.0f, 0.0f)
        );
    }
}

// 炎エフェクトのプリセット
ParticleEffectParams ParticleEffect_PresetFire()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();

    // 黄色から赤色へ変化させ、徐々に透明にする
    params.startColor = { 1.0f, 0.9f, 0.3f, 1.0f };
    params.endColor = { 1.0f, 0.2f, 0.0f, 0.0f };

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

// 火花エフェクトのプリセット
ParticleEffectParams ParticleEffect_PresetSpark()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();

    // 白色からオレンジ色へ変化させる
    params.startColor = { 1.0f, 1.0f, 0.8f, 1.0f };
    params.endColor = { 1.0f, 0.5f, 0.0f, 0.0f };

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

// 魔法エフェクトのプリセット
ParticleEffectParams ParticleEffect_PresetMagic()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();

    // 紫色から青色へ変化させる
    params.startColor = { 0.6f, 0.3f, 1.0f, 1.0f };
    params.endColor = { 0.3f, 0.6f, 1.0f, 0.0f };

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

// 回復エフェクトのプリセット
ParticleEffectParams ParticleEffect_PresetHeal()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();

    // 緑色から黄色へ変化させる
    params.startColor = { 0.3f, 1.0f, 0.3f, 1.0f };
    params.endColor = { 1.0f, 1.0f, 0.5f, 0.0f };

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

// 爆発エフェクトのプリセット
ParticleEffectParams ParticleEffect_PresetExplosion()
{
    ParticleEffectParams params = ParticleEffect_GetDefaultParams();

    // 明るいオレンジ色から暗い赤色へ変化させる
    params.startColor = { 1.0f, 0.8f, 0.3f, 1.0f };
    params.endColor = { 0.5f, 0.1f, 0.0f, 0.0f };

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