
/*==========================================================================
    画面上のエフェクトを管理する [gameeffect.cpp]

    エフェクトの生成・更新・描画・終了処理を行う。
    各エフェクトは生成位置、サイズ、寿命、色を持ち、
    時間の経過に応じて小さくなりながら透明になる。
==========================================================================*/

#include "gameeffect.h"
#include "texture.h"
#include "sprite.h"
#include "direct3d.h"

using namespace DirectX;

// 1つのエフェクトが持つ情報
struct Effect
{
    XMFLOAT2 position; // エフェクトの表示位置
    float size;        // エフェクトの初期サイズ
    double lifeTime;   // エフェクトの寿命
    double birthTime;  // エフェクトが生成された時間
    XMFLOAT4 color;    // エフェクトの色と透明度
};

// 同時に管理できるエフェクトの最大数
static constexpr int EFFECT_MAX = 128;

// エフェクト情報を格納する配列
static Effect g_Effects[EFFECT_MAX];

// 現在存在しているエフェクトの数
static int g_EffectCount{ 0 };

// エフェクト管理開始からの累積時間
static double g_AccumulatedTime = 0.0;

// エフェクト描画に使用するテクスチャID
static int g_TexId = -1;

// エフェクト管理に必要な情報を初期化する
void Effect_Initialize()
{
    // エフェクト用テクスチャを読み込む
    g_TexId = Texture_Load(L"rom\\aa.png");

    // 各エフェクトの寿命を初期化する
    for (Effect& e : g_Effects)
    {
        e.lifeTime = 0.0;
    }

    // エフェクト数と累積時間を初期化する
    g_EffectCount = 0;
    g_AccumulatedTime = 0.0;
}

// 経過時間に応じてエフェクトを更新する
void Effect_Update(double elapsed_time)
{
    // エフェクト管理開始からの累積時間を更新する
    g_AccumulatedTime += elapsed_time;

    // 寿命を迎えたエフェクトを配列から取り除く
    // 後ろから調べることで、要素の入れ替え後も確認しやすくする
    for (int i = g_EffectCount - 1; i >= 0; i--)
    {
        // 生成されてから現在までに経過した時間を求める
        double time = g_AccumulatedTime - g_Effects[i].birthTime;

        // 寿命を超えた場合、最後の要素と入れ替えて管理対象から外す
        if (g_Effects[i].lifeTime <= time)
        {
            g_Effects[i].lifeTime = 0.0f;
            g_Effects[i] = g_Effects[--g_EffectCount];
        }
    }
}

// エフェクトを描画する
void Effect_Draw()
{
    // 加算合成を有効にし、光が重なるような描画にする
    Direct3D_SetAlphaBlend(BLEND_ADD);

    // 現在存在しているエフェクトを順番に描画する
    for (int i = 0; i < g_EffectCount; i++)
    {
        // 生成からの経過時間を求める
        double time = g_AccumulatedTime - g_Effects[i].birthTime;

        // 寿命に対する進行度を求める
        float ratio = time / g_Effects[i].lifeTime;

        // 時間の経過に応じて透明度とサイズを小さくする
        float alpha = 1.0f - ratio;
        float size = g_Effects[i].size * (1.0f - ratio);

        // エフェクトの中心位置を基準に、描画位置を計算する
        float half = g_Effects[i].size * 0.5f;
        float dx = g_Effects[i].position.x + half - size * 0.5f;
        float dy = g_Effects[i].position.y + half - size * 0.5f;

        // 設定された色を取得し、透明度を更新する
        XMFLOAT4 color = g_Effects[i].color;
        color.w = alpha;

        // テクスチャを使用してエフェクトを描画する
        Sprite_Draw(
            g_TexId,
            dx, dy,
            size, size,
            0.0f, 0.0f,
            Texture_GetWidht(g_TexId),
            Texture_GetHeight(g_TexId),
            color);
    }

    // 通常の半透明合成に戻す
    Direct3D_SetAlphaBlend(BLEND_TRANSPARENT);
}

// エフェクト処理の終了
void Effect_Finalize()
{
    // この実装では、終了時に追加で解放するリソースはない
}

// 指定された位置・サイズ・寿命・色でエフェクトを生成する
void Effect_Create(
    const DirectX::XMFLOAT2& position,
    float size,
    double lifeTime,
    const DirectX::XMFLOAT4& color)
{
    // 管理上限に達している場合は、新しいエフェクトを生成しない
    if (g_EffectCount >= EFFECT_MAX)
        return;

    // 新しいエフェクトに必要な情報を設定する
    g_Effects[g_EffectCount].position = position;
    g_Effects[g_EffectCount].size = size;
    g_Effects[g_EffectCount].lifeTime = lifeTime;
    g_Effects[g_EffectCount].birthTime = g_AccumulatedTime;
    g_Effects[g_EffectCount].color = color;

    // 管理対象のエフェクト数を増やす
    g_EffectCount++;
}
