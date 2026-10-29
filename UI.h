
/*==========================================================================
UI表示 [ui.h]

通知の種類と、UIの初期化・更新・描画などの関数を宣言する。
==========================================================================*/

#pragma once
#include <DirectXMath.h>

namespace UI
{
    // 表示する通知の種類
    enum class NotificationType
    {
        NONE,   // 通知なし
        STRIKE, // ストライク
        SPARE,  // スペア
        GUTTER, // ガター
        MAX     // 通知種類の数を扱うための終端値
    };

    // UIで使用するテクスチャなどを初期化する
    void Initialize();

    // UIのアニメーションや通知状態を更新する
    // dt : 前回の更新からの経過時間（秒）
    void Update(double dt);

    // UIを描画する
    void Draw();

    // UIの終了処理
    void Finalize();

    // 指定した種類の通知を表示する
    void ShowNotification(NotificationType type);
}
