
/*==========================================================================
UI表示 [ui.cpp]

ストライク、スペア、ガターなどの通知表示とアニメーションを管理する。
==========================================================================*/

#include "ui.h"
#include "sprite.h"
#include "texture.h"
#include <algorithm>

using namespace DirectX;

namespace UI
{
    // 各通知に使用するテクスチャID
    static int g_StrikeTex = -1;
    static int g_SpareTex = -1;
    static int g_GutterTex = -1;
    static int g_setumeiTex = -1;

    // 現在表示している通知の種類
    static NotificationType g_CurrentNotification = NotificationType::NONE;

    // 通知アニメーションの管理変数
    static float g_AnimTimer = 0.0f;       // アニメーション開始からの経過時間
    static float g_DisplayDuration = 2.0f; // 通知を完全表示する時間
    static float g_FadeInDuration = 0.5f;  // フェードインにかける時間
    static float g_FadeOutDuration = 0.5f; // フェードアウトにかける時間

    // UIの配置に使用する画面サイズと余白
    constexpr float SCREEN_WIDTH = 1920.0f;
    constexpr float SCREEN_HEIGHT = 720.0f;
    constexpr float UI_MARGIN = 20.0f;

    // EaseOutCubic：最初に大きく変化し、徐々に変化を緩やかにする
    float EaseOutCubic(float t)
    {
        return 1.0f - powf(1.0f - t, 3.0f);
    }

    // EaseInCubic：最初はゆっくり変化し、徐々に変化を大きくする
    float EaseInCubic(float t)
    {
        return t * t * t;
    }

    // EaseOutBack：目標値を少し超えてから戻る動きを作る
    // 通知がポップアップするような演出に使用する
    float EaseOutBack(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        return 1.0f
            + c3 * powf(t - 1.0f, 3.0f)
            + c1 * powf(t - 1.0f, 2.0f);
    }

    // UIで使用するテクスチャを読み込み、状態を初期化する
    void Initialize()
    {
        g_StrikeTex = Texture_Load(L"rom\\Texture\\Strike.png");
        g_SpareTex = Texture_Load(L"rom\\Texture\\Spare.png");
        g_GutterTex = Texture_Load(L"rom\\Texture\\Gutter.png");
        g_setumeiTex = Texture_Load(L"rom\\Texture\\setumei.png");

        // 通知を非表示状態にし、アニメーション時間をリセットする
        g_CurrentNotification = NotificationType::NONE;
        g_AnimTimer = 0.0f;
    }

    // 通知の表示時間を更新する
    void Update(double dt)
    {
        // 通知が表示されている場合のみ時間を進める
        if (g_CurrentNotification != NotificationType::NONE)
        {
            g_AnimTimer += static_cast<float>(dt);

            // フェードイン・完全表示・フェードアウトの合計時間を求める
            float totalDuration =
                g_FadeInDuration +
                g_DisplayDuration +
                g_FadeOutDuration;

            // 表示時間を超えたら通知を非表示にする
            if (g_AnimTimer >= totalDuration)
            {
                g_CurrentNotification = NotificationType::NONE;
                g_AnimTimer = 0.0f;
            }
        }
    }

    // UIを描画する
    void Draw()
    {
        //================================
        // 説明画像を常時表示する
        //================================
        if (g_setumeiTex != -1)
        {
            const float width = 400.0f;
            const float height = 200.0f;

            // 画面サイズと余白を基準に表示位置を計算する
            float x = SCREEN_WIDTH - width * 0.5f - UI_MARGIN - 400;
            float y = SCREEN_HEIGHT - height * 0.5f - UI_MARGIN - 500;

            // 通常の不透明な白色で描画する
            XMFLOAT4 color = { 1, 1, 1, 1 };

            Sprite_DrawCenter(
                g_setumeiTex,
                x,
                y,
                width,
                height,
                color
            );
        }

        // 通知がなければ、ここで描画処理を終了する
        if (g_CurrentNotification == NotificationType::NONE)
            return;

        // 通知の種類に応じて使用するテクスチャを選択する
        int texId = -1;

        switch (g_CurrentNotification)
        {
        case NotificationType::STRIKE:
            texId = g_StrikeTex;
            break;

        case NotificationType::SPARE:
            texId = g_SpareTex;
            break;

        case NotificationType::GUTTER:
            texId = g_GutterTex;
            break;

        default:
            return;
        }

        // 有効なテクスチャがない場合は描画しない
        if (texId == -1)
            return;

        // 通知の透明度と拡大率を初期化する
        float alpha = 1.0f;
        float scale = 1.0f;

        // アニメーション全体の時間を計算する
        float totalDuration =
            g_FadeInDuration +
            g_DisplayDuration +
            g_FadeOutDuration;

        if (g_AnimTimer < g_FadeInDuration)
        {
            // フェードイン中：透明状態から徐々に表示する
            float t = g_AnimTimer / g_FadeInDuration;

            alpha = EaseOutCubic(t);

            // 少し拡大しすぎてから戻ることで、ポップアップ感を出す
            scale = EaseOutBack(t);
        }
        else if (g_AnimTimer < g_FadeInDuration + g_DisplayDuration)
        {
            // 完全表示中：不透明で通常の大きさにする
            alpha = 1.0f;
            scale = 1.0f;
        }
        else
        {
            // フェードアウト中：徐々に透明にする
            float t =
                (g_AnimTimer - g_FadeInDuration - g_DisplayDuration)
                / g_FadeOutDuration;

            alpha = 1.0f - EaseInCubic(t);

            // 透明になるにつれて少しずつ拡大する
            scale = 1.0f + EaseInCubic(t) * 0.2f;
        }

        // 通知画像の基本サイズと画面中央の座標
        const float baseWidth = 512.0f;
        const float baseHeight = 256.0f;
        const float screenCenterX = 960.0f;
        const float screenCenterY = 360.0f;

        // アニメーションの拡大率を画像サイズに反映する
        float width = baseWidth * scale;
        float height = baseHeight * scale;

        // アルファ値を設定し、透明度を調整する
        XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, alpha };

        // 画面中央に通知画像を描画する
        Sprite_DrawCenter(
            texId,
            screenCenterX,
            screenCenterY,
            width,
            height,
            color
        );
    }

    // UIの終了処理
    void Finalize()
    {
        // テクスチャの解放はTextureシステム側で行う
        // ここでは通知状態とアニメーション時間を初期状態に戻す
        g_CurrentNotification = NotificationType::NONE;
        g_AnimTimer = 0.0f;
    }

    // 指定した種類の通知を表示する
    void ShowNotification(NotificationType type)
    {
        // 表示する通知の種類を設定する
        g_CurrentNotification = type;

        // アニメーションを最初から再生するため時間をリセットする
        g_AnimTimer = 0.0f;
    }
}
