
/*==========================================================================
    ゲーム全体の初期化・更新・描画・終了処理を管理する [game.cpp]

    ゲーム内で使用する各システムを初期化し、
    毎フレームの更新処理と描画処理を呼び出す。

    主な処理：
    ・カメラ、ボール、ピン、UIなどの初期化
    ・投球処理とピンの状態管理
    ・倒れたピンの数に応じたスコア計算
    ・ゲームの描画とリソースの解放
==========================================================================*/

#include "game.h"
#include "game_bg.h"
#include "grid.h"
#include "lighth.h"
#include "keylogger.h"
#include "map.h"
#include "debug.h"
#include "camera.h"
#include "gameeffect.h"

#include "direct3d.h"
#include "scene.h"
#include "timer.h"

#include "shot.h"
#include "billBord.h"
#include "camera_fixed.h"
#include "trail.h"
#include "bowlingBall.h"
#include "debugDraw.h"
#include "mouselogger.h"
#include "pinManager.h"
#include "particleEffect.h"
#include "scoreBoard.h"

#include "debug_ostream.h"
#include "trailExplosion.h"
#include "directionCamera.h"
#include "ui.h"
#include "audio.h"

using namespace DirectX;

// カメラとゲーム進行に関する管理変数

// newで生成し、deleteで解放するデバッグカメラ
static DebugCamera* g_pDebugCamera{};
static FixedCamera* g_FixedCameras[2];
static DirectionCamera* g_pDirectionCamera{};
static int g_FixedCameraIndex{ 0 };

// テクスチャIDと投球回数の管理
static int g_ShotCount = 0;
static constexpr int MAX_SHOT = 2;

// ボールの初期位置とカメラの注視点
static const XMFLOAT3 BALL_START_POS = { 5, 5, -65 };
static const XMFLOAT3 Point_Of_Fixation = { 5.f, 5.0f, 5.0f };

// ボールが投球中かどうかを管理
static bool g_BallInPlay = false;

// ボウリングボールとピンの管理
BowlingBall g_BowlingBall;
static float g_PinSettleTimer = 0.0f;

// ピンが倒れきるまで待つ時間
static constexpr float PIN_SETTLE_TIME = 1.5f;
static bool g_WaitingPinSettle = false;

// フレーム数と投球前のピンの状態を管理
static int g_FrameCount = 0;
static constexpr int MAX_FRAME = 4;

// 投球前に残っていたピン数。
// 投球後に残ったピン数との差から、倒したピン数を算出する。
static int g_PrevRemainingPinsThisThrow = 0;

PinManager g_Pinmanager;

// 値を指定範囲内に収める関数
float GameClamp(float v, float minV, float maxV);

// ゲーム開始時に必要なオブジェクトとシステムを初期化する
void Game_Initialize()
{

    // デバッグ描画機能を初期化する
    DebugDraw_Initialize();

    // ボールの初期位置を基準にカメラの位置を設定する
    XMFLOAT3 a = BALL_START_POS;
    a.z -= 7;

    // 各カメラを生成する
    g_pDebugCamera = new DebugCamera(
        { a },
        { 0.0f, 0.0f, 0.0f });

    g_pDirectionCamera = new DirectionCamera(
        { a },
        { Point_Of_Fixation });

    // 特定の位置からボールを追う固定カメラを生成する
    g_FixedCameras[0] = new FixedCamera(
        { -5.0f, 10.0f, 10.0f },
        AABB::Make({ 1.0f, 5.00f, 1.0f }, { 5.0f, 5.0f, 5.0f }));

    g_FixedCameras[1] = new FixedCamera(
        { 5.0f, 10.0f, 10.0f },
        AABB::Make({ 6.0f, 6.0f, 6.0f }, { 20.0f, 10.0f, 20.0f }));

    // ボール、スコア表示、ビルボード、マップ、ライトを初期化する
    g_BowlingBall.Init(BALL_START_POS);
    ScoreBoard_Initialize();
    Billboard_Initialize();
    Gulid_Initialize(10, 10, 1);
    Map_Initialize();
    Light_Initialize();

    // 投球位置とカメラの向きを基準に、ショット処理を初期化する
    a.z += 20;
    hal::dout << a.z;
    Shot_Initialize(a, g_pDebugCamera->GetFront());

    // スコア表示、軌跡、UIを初期化する
    Trail_Initialize();
    UI::Initialize();

    // エフェクト用テクスチャを読み込む

    // ピンを初期配置する
    g_Pinmanager.Initialize();

    // 投球回数などの管理変数を初期化する
    g_FrameCount = 0;
    g_ShotCount = 0;

    // 爆発エフェクトを初期化する
    TrailExplosion_Initialize();
}

// ゲーム中の各システムを更新する
void Game_Update(double elapsed_time)
{

    // ボールの移動とショット位置、UIを更新する
    g_BowlingBall.Update(elapsed_time);
    Shot_SetPosition(g_BowlingBall.GetPosition());
    UI::Update(elapsed_time);

    // ショット、軌跡、ビルボードを更新する
    Shot_Update(elapsed_time);
    Trail_AddPosition(g_BowlingBall.GetPosition());
    Billboard_Update(elapsed_time);

    // ボールの位置を基準に、固定カメラの注視対象を更新する
    for (int i = 0; i < 2; i++)
    {
        g_FixedCameras[i]->SetTarget(g_BowlingBall.GetPosition());

        // カメラの判定範囲に入っていたら、そのカメラを選択する
        if (g_FixedCameras[i]->IsOverlap())
        {
            g_FixedCameraIndex = i;
            break;
        }
    }

    // 選択中の固定カメラのビュー行列を設定する
    g_FixedCameras[g_FixedCameraIndex]->SetMatrix();

    // ビルボードと爆発エフェクトにカメラ情報を設定する
    Billboard_SetViewMatrix(g_pDebugCamera->GetViewMatrix());
    TrailExplosion_SetCameraPosition(g_pDebugCamera->GetPosition());

    // 爆発エフェクトとスコアを更新する
    TrailExplosion_Update(elapsed_time);

    // Rキーでピンをリセットする
    if (KeyLogger_IsPressed(KK_R))
    {
        g_Pinmanager.ResetPins();
    }

    // Tキーで結果シーンへ移動する
    if (KeyLogger_IsPressed(KK_T))
    {
        Scene_SetNextScene(SCENE_RESULT);
    }

    // ショット入力から、投球が行われたかを取得する
    bool fired = Shot_ConsumeFire();

    // 投球していない状態で発射入力があれば、投球を開始する
    if (!g_BallInPlay && fired)
    {
        // 投球前に倒れているピン数と残っているピン数を記録する
        g_PrevRemainingPinsThisThrow =
            g_Pinmanager.GetRemainingPinCount();

        // ボールに投球速度を与え、投球状態へ切り替える
        g_BowlingBall.AddForce(Shot_GetShotVelocity());
        Shot_ResetPower();
        g_BallInPlay = true;
    }

    // デバッグカメラを更新する
    g_pDebugCamera->Update(elapsed_time);

    // 投球中は演出用カメラを更新し、投球していないときは初期状態に戻す
    if (g_BallInPlay)
    {
        g_pDirectionCamera->Update(elapsed_time);
    }
    else
    {
        g_pDirectionCamera->ResetCamera();
    }

    // ボールが停止したら、投球を終了してピンが落ち着くのを待つ
    if (g_BallInPlay && g_BowlingBall.IsStopped())
    {
        g_BallInPlay = false;
        g_WaitingPinSettle = true;
        g_PinSettleTimer = 0.0f;

        // 投球前のピン数は、投球開始時に記録した値を使用する
    }

    // ピンが倒れきるまで一定時間待つ
    if (g_WaitingPinSettle)
    {
        g_PinSettleTimer += elapsed_time;

        // 待機時間が経過したら、倒れたピン数を計算する
        if (g_PinSettleTimer >= PIN_SETTLE_TIME)
        {
            g_WaitingPinSettle = false;
            g_ShotCount++;

            // 投球後に倒れたピンを取得し、倒れたピンを削除する
            g_Pinmanager.RemoveDeadPins();

            // 投球前後の残存ピン数の差から、倒れたピン数を算出する
            int remainingAfterRemove =
                g_Pinmanager.GetRemainingPinCount();

            int fallenPins =
                g_PrevRemainingPinsThisThrow - remainingAfterRemove;

            // 倒れたピン数を0～10の範囲に制限する
            if (fallenPins < 0)
                fallenPins = 0;

            fallenPins = static_cast<int>(
                GameClamp((float)fallenPins, 0.0f, 10.0f));

            // ストライク、スペア、ガターを判定する
            bool isStrike = (fallenPins == 10 && g_ShotCount == 1);
            bool isSpare =
                (fallenPins > 0 && remainingAfterRemove == 0
                    && g_ShotCount == 2);
            bool isGutter = (fallenPins == 0);

            // 判定結果に応じてUIの通知を表示する
            if (isStrike)
            {
                UI::ShowNotification(UI::NotificationType::STRIKE);
                // PlayAudio(g_StrikeSound, false);
            }
            else if (isSpare)
            {
                UI::ShowNotification(UI::NotificationType::SPARE);
                // PlayAudio(g_HitSound, false);
            }
            else if (isGutter)
            {
                UI::ShowNotification(UI::NotificationType::GUTTER);
            }

            // 今回の投球で倒したピン数をスコアへ加算する
            Score_AddThrow(fallenPins);

            // ボールを初期位置へ戻す
            g_BowlingBall.Reset(BALL_START_POS);

            // ストライク、または2投目終了時に次のフレームへ進む
            if (isStrike || g_ShotCount >= MAX_SHOT)
            {
                // ピンを再配置し、フレーム関連の状態を更新する
                g_Pinmanager.DestroyAndRecreatePins();
                g_ShotCount = 0;
                g_FrameCount++;

                // 最大フレーム数に達したら結果シーンへ移動する
                if (g_FrameCount >= MAX_FRAME)
                {
                    Scene_SetNextScene(SCENE_RESULT);
                    return;
                }
            }
        }
    }

    // ピンとボールの相互作用を更新する
    g_Pinmanager.Update(elapsed_time, g_BowlingBall);

    // パーティクルエフェクトを更新する
    // AuraEffect_Update(elapsed_time);
    ParticleEffect_Update(elapsed_time);
}

// ゲーム内の3Dオブジェクトと2D UIを描画する
void Game_Draw()
{
    // 描画前に深度テストを有効にする
    Direct3D_SetDepthTest(true);

    // 投球中は演出用カメラ、それ以外はデバッグカメラを使用する
    if (g_BallInPlay)
    {
        g_pDirectionCamera->SetMatrix();
    }
    else
    {
        g_pDebugCamera->SetMatrix();
    }

    // ビルボード描画に使用するビュー行列を設定する
    Billboard_SetViewMatrix(g_pDebugCamera->GetViewMatrix());

    // ボールとピンを描画する
    g_BowlingBall.Draw();
    g_Pinmanager.Draw();

    // グリッドとライトの設定
    Gulid_Draw();
    Light_SetAmnient({ 0.5f, 0.5f, 0.6f, 1.0f });

    XMFLOAT3 direction;
    XMStoreFloat3(&direction, { -1.0f, -1.5f, 1.0f });
    Light_SetDiffuse({ 0.5f, 0.5f, 0.5f }, direction);

    // ショット、マップ、爆発エフェクトを描画する
    Shot_Draw();
    Map_Draw();
    TrailExplosion_Draw();

    // 2D描画では深度テストを無効にする
    Direct3D_SetDepthTest(false);

    // ショット用UI、スコアボード、通常UIを描画する
    Shot_DrawUI();
    ScoreBoard_Draw();
    UI::Draw();
}

// ゲーム終了時に、確保したオブジェクトや各システムのリソースを解放する
void Game_Finalize()
{
    delete g_pDebugCamera;

    // モデルと各描画・演出システムを終了する
    DebugDraw_Finalize();
    Map_Finalize();
    Gulid_Finalize();
    Light_Finalize();
    Shot_Finalize();
    Trail_Finalize();
    TrailExplosion_Finalize();
    UI::Finalize();

    // ピン管理システムを終了する
    g_Pinmanager.Finalize();
}

// 値を指定された最小値から最大値の範囲内に収める
float GameClamp(float v, float minV, float maxV)
{
    if (v < minV) return minV;
    if (v > maxV) return maxV;
    return v;
}


