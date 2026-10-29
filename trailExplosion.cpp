
/*==========================================================================
トレイルを使った爆発エフェクト [trailExplosion.cpp]

複数のトレイルを異なる方向へ飛ばし、爆発のようなエフェクトを作成する。
==========================================================================*/

#include "trailExplosion.h"
#include "trail.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>

using namespace DirectX;

// 同時に管理する爆発の最大数
static constexpr int MAX_EXPLOSIONS = 50;

// 1つの爆発から生成するトレイルの最大数
static constexpr int MAX_TRAILS_PER_EXPLOSION = 32;

// 円周の計算に使用する円周率
static constexpr float PI = 3.14159265f;

// 爆発を構成する個々のトレイルのデータ
struct ExplosionTrail
{
    XMFLOAT3 position;             // 現在位置
    XMFLOAT3 velocity;             // 移動速度
    XMFLOAT3 positionHistory[50];  // 描画に使う位置履歴
    bool active;                   // トレイルが有効かどうか
};

// 1回分の爆発エフェクトのデータ
struct ExplosionInstance
{
    ExplosionTrail trails[MAX_TRAILS_PER_EXPLOSION]; // 爆発を構成するトレイル
    int trailCount;        // トレイル数
    float timer;           // 爆発開始からの経過時間
    float duration;        // 爆発の持続時間
    float maxDistance;     // 最大到達距離の設定値
    XMFLOAT4 startColor;   // トレイル開始時の色
    XMFLOAT4 endColor;     // トレイル終了時の色
    bool useGravity;       // 重力を適用するかどうか
    bool active;           // 爆発が有効かどうか
};

// 生成された爆発を管理する配列
static std::vector<ExplosionInstance> g_Explosions;

// トレイル描画に使用するカメラ位置
static XMFLOAT3 g_CameraPosition{};

// 指定範囲の浮動小数点乱数を生成する
static float RandFloat(float min, float max)
{
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

// 爆発エフェクトを初期化する
void TrailExplosion_Initialize()
{
    // 爆発管理用の配列に容量を確保する
    g_Explosions.reserve(MAX_EXPLOSIONS);

    // トレイル描画システムを初期化する
    Trail_Initialize();
}

// 爆発エフェクトを終了する
void TrailExplosion_Finalize()
{
    // 管理している爆発データをすべて削除する
    g_Explosions.clear();

    // トレイル描画システムを終了する
    Trail_Finalize();
}

// 爆発の位置や速度、寿命を更新する
void TrailExplosion_Update(float deltaTime)
{
    // 登録されている爆発を順番に更新する
    for (auto& explosion : g_Explosions)
    {
        // 無効な爆発は更新しない
        if (!explosion.active)
            continue;

        // 爆発の経過時間を進める
        explosion.timer += deltaTime;

        // 持続時間を超えた爆発は無効にする
        if (explosion.timer >= explosion.duration)
        {
            explosion.active = false;
            continue;
        }

        // 爆発に含まれる各トレイルを更新する
        for (int i = 0; i < explosion.trailCount; i++)
        {
            ExplosionTrail& trail = explosion.trails[i];

            // 無効なトレイルは更新しない
            if (!trail.active)
                continue;

            // 速度と経過時間から位置を更新する
            trail.position.x += trail.velocity.x * deltaTime;
            trail.position.y += trail.velocity.y * deltaTime;
            trail.position.z += trail.velocity.z * deltaTime;

            // 重力が有効な場合、下方向へ速度を加える
            if (explosion.useGravity)
            {
                trail.velocity.y -= 9.8f * deltaTime;
            }

            // 速度を少しずつ減らし、空気抵抗のような動きを表現する
            trail.velocity.x *= 0.98f;
            trail.velocity.y *= 0.98f;
            trail.velocity.z *= 0.98f;

            // 位置履歴を1つずつ前へずらす
            // 古い位置が先頭側へ移動し、最新位置が末尾に入る
            for (int j = 0; j < 49; j++)
            {
                trail.positionHistory[j] = trail.positionHistory[j + 1];
            }

            // 最新の位置を履歴の最後に保存する
            trail.positionHistory[49] = trail.position;
        }
    }

    // 寿命が終了した爆発を管理配列から削除する
    g_Explosions.erase(
        std::remove_if(
            g_Explosions.begin(),
            g_Explosions.end(),
            [](const ExplosionInstance& e)
            {
                return !e.active;
            }
        ),
        g_Explosions.end()
    );
}

// 生存中の爆発エフェクトを描画する
void TrailExplosion_Draw()
{
    // トレイル描画システムへカメラ位置を設定する
    Trail_SetCameraPosition(g_CameraPosition);

    // 登録されている爆発を順番に描画する
    for (const auto& explosion : g_Explosions)
    {
        // 無効な爆発は描画しない
        if (!explosion.active)
            continue;

        // 終了間際の一定時間で透明度を下げる
        float fadeAmount = 1.0f;

        if (explosion.timer > explosion.duration * 0.7f)
        {
            fadeAmount =
                1.0f -
                (explosion.timer - explosion.duration * 0.7f)
                / (explosion.duration * 0.3f);
        }

        // 開始色と終了色を取得する
        XMFLOAT4 startColor = explosion.startColor;
        XMFLOAT4 endColor = explosion.endColor;

        // 色のアルファ値にフェード量を反映する
        startColor.w *= fadeAmount;
        endColor.w *= fadeAmount;

        // トレイルの色と描画設定を更新する
        Trail_SetColor(startColor, endColor);
        Trail_SetEnabled(true);
        Trail_SetFadeEnabled(true);

        // 爆発に含まれるトレイルを1本ずつ描画する
        for (int i = 0; i < explosion.trailCount; i++)
        {
            const ExplosionTrail& trail = explosion.trails[i];

            // 無効なトレイルは描画しない
            if (!trail.active)
                continue;

            // 前のトレイルの位置履歴を消去する
            Trail_Clear();

            // このトレイル用の色と描画設定を再設定する
            Trail_SetColor(startColor, endColor);
            Trail_SetEnabled(true);
            Trail_SetFadeEnabled(true);

            // 位置履歴をトレイル描画システムへ登録する
            for (int j = 0; j < 50; j++)
            {
                Trail_AddPosition(trail.positionHistory[j]);
            }

            // 登録した位置履歴を使ってトレイルを描画する
            Trail_Draw();
        }
    }
}

// 設定に従って爆発エフェクトを生成する
void TrailExplosion_Create(const TrailExplosionSettings& settings)
{
    // 同時に管理できる爆発数を超えている場合は生成しない
    if (g_Explosions.size() >= MAX_EXPLOSIONS)
        return;

    // 新しい爆発データを作成する
    ExplosionInstance explosion;

    // 指定されたトレイル数を最大数以内に制限する
    explosion.trailCount = settings.trailCount;

    if (explosion.trailCount > MAX_TRAILS_PER_EXPLOSION)
        explosion.trailCount = MAX_TRAILS_PER_EXPLOSION;

    // 爆発の初期状態を設定する
    explosion.timer = 0.0f;
    explosion.duration = settings.duration;
    explosion.maxDistance = settings.maxDistance;
    explosion.startColor = settings.startColor;
    explosion.endColor = settings.endColor;
    explosion.useGravity = settings.useGravity;
    explosion.active = true;

    // 爆発を構成するトレイルを初期化する
    for (int i = 0; i < explosion.trailCount; i++)
    {
        ExplosionTrail& trail = explosion.trails[i];

        // すべてのトレイルを爆発の発生位置から開始させる
        trail.position = settings.position;

        // トレイルが飛ぶ方向を計算する
        XMFLOAT3 direction;

        switch (settings.type)
        {
        case TrailExplosionType::RADIAL:
        {
            // XZ平面上で円形に広がる方向を計算する
            float angle =
                (360.0f / explosion.trailCount) * i * (PI / 180.0f);

            direction.x = cosf(angle);
            direction.y = RandFloat(-0.2f, 0.3f);
            direction.z = sinf(angle);
            break;
        }

        case TrailExplosionType::SPHERE:
        {
            // 球面上に広がる方向をランダムに計算する
            float theta = RandFloat(0, PI * 2);
            float phi = acosf(RandFloat(-1.0f, 1.0f));

            direction.x = sinf(phi) * cosf(theta);
            direction.y = sinf(phi) * sinf(theta);
            direction.z = cosf(phi);
            break;
        }

        case TrailExplosionType::CONE_UP:
        {
            // 上方向へ広がる円錐状の方向を計算する
            float angle =
                (360.0f / explosion.trailCount) * i * (PI / 180.0f);

            float elevation = RandFloat(0.3f, 0.8f);

            direction.x = cosf(angle) * (1.0f - elevation);
            direction.y = elevation;
            direction.z = sinf(angle) * (1.0f - elevation);
            break;
        }

        case TrailExplosionType::RING:
        {
            // Y方向の変化を小さくし、リング状に広がる方向を計算する
            float angle =
                (360.0f / explosion.trailCount) * i * (PI / 180.0f);

            direction.x = cosf(angle);
            direction.y = RandFloat(-0.05f, 0.05f);
            direction.z = sinf(angle);
            break;
        }

        case TrailExplosionType::SPIRAL:
        {
            // 角度と高さを変化させ、螺旋状に広がる方向を計算する
            float angle =
                (720.0f / explosion.trailCount) * i * (PI / 180.0f);

            float height = (float)i / explosion.trailCount;

            direction.x = cosf(angle);
            direction.y = height * 2.0f - 1.0f;
            direction.z = sinf(angle);
            break;
        }
        }

        // 計算した方向ベクトルを、設定された回転角度に合わせて回転する
        {
            XMVECTOR dir = XMLoadFloat3(&direction);

            XMMATRIX rot = XMMatrixRotationRollPitchYaw(
                settings.rotation.x, // X軸回転（Pitch、ラジアン）
                settings.rotation.y, // Y軸回転（Yaw、ラジアン）
                settings.rotation.z  // Z軸回転（Roll、ラジアン）
            );

            // 回転行列を方向ベクトルに適用する
            dir = XMVector3TransformNormal(dir, rot);

            // ベクトルの長さを1にそろえる
            dir = XMVector3Normalize(dir);

            // 計算結果をXMFLOAT3へ戻す
            XMStoreFloat3(&direction, dir);
        }

        // トレイルごとに速度を少し変化させる
        float speedVariation = RandFloat(0.8f, 1.2f);

        // 方向・基本速度・ランダム変化・全体スケールから速度を設定する
        trail.velocity.x =
            direction.x * settings.speed * speedVariation * settings.scale;

        trail.velocity.y =
            direction.y * settings.speed * speedVariation * settings.scale;

        trail.velocity.z =
            direction.z * settings.speed * speedVariation * settings.scale;

        // 位置履歴を発生位置で埋め、生成直後の軌跡を初期化する
        for (int j = 0; j < 50; j++)
        {
            trail.positionHistory[j] = settings.position;
        }

        // トレイルを有効にする
        trail.active = true;
    }

    // 初期化した爆発を管理配列へ追加する
    g_Explosions.push_back(explosion);
}

// 放射状の爆発エフェクトを生成するプリセット
void TrailExplosion_CreateRadial(const XMFLOAT3& position, float scale)
{
    TrailExplosionSettings settings;

    settings.position = position;
    settings.position.y += 5; // 発生位置を少し上げる
    settings.type = TrailExplosionType::RADIAL;
    settings.trailCount = 8;
    settings.speed = 100.0f;
    settings.maxDistance = 10.0f;
    settings.startColor = { 1.0f, 0.5f, 0.0f, 1.0f };
    settings.endColor = { 1.0f, 0.0f, 1.0f, 0.0f };
    settings.duration = 0.4f;
    settings.useGravity = true;
    settings.scale = scale;
    settings.rotation = { -XM_PIDIV2, 0.0f, 0.0f };

    TrailExplosion_Create(settings);
}

// 球状の爆発エフェクトを生成するプリセット
void TrailExplosion_CreateSphere(const XMFLOAT3& position, float scale)
{
    TrailExplosionSettings settings;

    settings.position = position;
    settings.type = TrailExplosionType::SPHERE;
    settings.trailCount = 24;
    settings.speed = 10.0f;
    settings.maxDistance = 25.0f;
    settings.startColor = { 1.0f, 0.8f, 0.2f, 1.0f };
    settings.endColor = { 1.0f, 0.3f, 0.0f, 0.0f };
    settings.duration = 2.0f;
    settings.useGravity = true;
    settings.scale = scale;

    TrailExplosion_Create(settings);
}

// 上向き円錐状の爆発エフェクトを生成するプリセット
void TrailExplosion_CreateConeUp(const XMFLOAT3& position, float scale)
{
    TrailExplosionSettings settings;

    settings.position = position;
    settings.type = TrailExplosionType::CONE_UP;
    settings.trailCount = 20;
    settings.speed = 30.0f;
    settings.maxDistance = 30.0f;
    settings.startColor = { 0.8f, 0.0f, 1.0f, 1.0f };
    settings.endColor = { 1.0f, 0.5f, 1.0f, 0.0f };
    settings.duration = 2.5f;
    settings.useGravity = true;
    settings.scale = scale;

    TrailExplosion_Create(settings);
}

// リング状の爆発エフェクトを生成するプリセット
void TrailExplosion_CreateRing(const XMFLOAT3& position, float scale)
{
    TrailExplosionSettings settings;

    settings.position = position;
    settings.type = TrailExplosionType::RING;
    settings.trailCount = 24;
    settings.speed = 15.0f;
    settings.maxDistance = 35.0f;
    settings.startColor = { 0.5f, 0.5f, 1.0f, 1.0f };
    settings.endColor = { 0.8f, 0.8f, 1.0f, 0.0f };
    settings.duration = 1.2f;
    settings.useGravity = false;
    settings.scale = scale;

    TrailExplosion_Create(settings);
}

// 螺旋状の爆発エフェクトを生成するプリセット
void TrailExplosion_CreateSpiral(const XMFLOAT3& position, float scale)
{
    TrailExplosionSettings settings;

    settings.position = position;
    settings.type = TrailExplosionType::SPIRAL;
    settings.trailCount = 32;
    settings.speed = 7.0f;
    settings.maxDistance = 20.0f;
    settings.startColor = { 0.0f, 1.0f, 0.5f, 1.0f };
    settings.endColor = { 0.0f, 0.5f, 0.0f, 0.0f };
    settings.duration = 3.0f;
    settings.useGravity = false;
    settings.scale = scale;

    TrailExplosion_Create(settings);
}

// トレイル描画に使用するカメラ位置を設定する
void TrailExplosion_SetCameraPosition(const XMFLOAT3& position)
{
    g_CameraPosition = position;
}

// 管理している爆発数を取得する
int TrailExplosion_GetActiveCount()
{
    return static_cast<int>(g_Explosions.size());
}
