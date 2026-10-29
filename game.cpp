/*==========================================================================

	ゲーム全体の初期化／更新／描画／終了処理を管理 [game.cpp]



---------------------------------------------------------------------------


==========================================================================*/


#include "game.h"
#include "game_bg.h"
#include "grid.h"
#include "game_score.h"
#include "lighth.h"
#include "keylogger.h"
#include "map.h"
#include "debug.h"
#include "camera.h"
#include "gameeffect.h"
#include "texture.h"
#include "player.h"

#include "direct3d.h"
#include "scene.h"
#include "timer.h"
#include "ball.h"
#include "sprite_anim.h"

#include "model.h";
#include "shot.h"
#include "billBord.h"
#include "sprite_anim.h"
#include "shotCamera.h"
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


static Camera* g_a[2];
static bool g_Start = false;
static int g_BgmId = -1; // BGMのID
static bool g_NextChlick = false;
static AnimPattern* g_pTestAnim = nullptr;
static float g_SelectedX = 0.0f;
static float g_SelectedY = 0.0f;
static float g_LimitTime = 0.0f;
static int Cube01tex = -1;
static AnimPatternPlayer* g_pAnimPlayer{ nullptr };
//ポインタ Newすれば作ったタイミングで生成される
//deleteすれば消える
static DebugCamera* g_pDebugCamera{}; 
static FixedCamera* g_FixedCameras[3];
static DirectionCamera* g_pDirectionCamera{};
static int g_FixedCameraIndex{0};


static MODEL* g_pKirby;
static MODEL* g_pPenis;
static AnimPattern* g_pTestAnim2 = nullptr;
static AnimPatternPlayer* g_pAnimPlayer2{ nullptr };
static ShotCamera* g_pShotCamera{ nullptr };

static int g_texid = -1;
static int g_texid2 = -1;
static int g_ShotCount = 0;
static constexpr int MAX_SHOT = 2;



static const XMFLOAT3 BALL_START_POS = { 5,5,-65 };
static const XMFLOAT3 Point_Of_Fixation ={ 5.f, 5.0f, 5.0f };

static bool g_BallInPlay = false;

static AABB g_GoalAABB;
static bool g_IsGoal = false;
BowlingBall g_BowlingBall;
static int g_PrevDownPinCount = 0;
static float g_PinSettleTimer = 0.0f;
// 余裕を増やして誤判定を減らす
static constexpr float PIN_SETTLE_TIME = 1.5f; // 1秒 -> 1.5秒に変更
static bool g_WaitingPinSettle = false;
static int g_FrameCount = 0;
static constexpr int MAX_FRAME = 4;
static int g_PrevDownPinsThisThrow = 0;
// 追加: 投球時に残っていたピン数（Remove/削除を考慮した確実な差分算出に使用）
static int g_PrevRemainingPinsThisThrow = 0;

PinManager g_Pinmanager;
float GameClamp(float v, float minV, float maxV);
void Game_Initialize()
{
    // ゲームオブジェクトと各描画・入力システムを初期化する。
	g_GoalAABB = AABB::Make({ float(1.0f), 0.9f, float(1.0f) }, { 1,1,1 });
	DebugDraw_Initialize();
	XMFLOAT3 a = BALL_START_POS;
	a.z -= 7;
	g_pDebugCamera = new DebugCamera({ a }, { 0.0f,0.0f,0.0f });
	g_pShotCamera = new ShotCamera({ 0.0f,0.0f,50.0f }, {2.0f,1.0f,2.0f},2.0f);
	g_pDirectionCamera = new DirectionCamera({ a }, { Point_Of_Fixation });


	g_FixedCameras[0] = new FixedCamera({ -5.0f, 10.0f, 10.0f }, AABB::Make({ 1.0f,5.00f,1.0f }, {5.0f,5.0f,5.0f}));
	g_FixedCameras[1] = new FixedCamera({5.0f, 10.0f, 10.0f}, AABB::Make({ 6.0f,6.0f,6.0f }, { 20.0f,10.0f,20.0f }));
	g_BowlingBall.Init(BALL_START_POS);
	ScoreBoard_Initialize();
	Billboard_Initialize();
	Gulid_Initialize(10,10,1);
	Map_Initialize();
	Light_Initialize();

	a.z += 20;
	hal::dout << a.z;
	Shot_Initialize(a,g_pDebugCamera->GetFront());
	Score_Initialize(100,100,2);
	Trail_Initialize();
	UI::Initialize();
	g_texid = Texture_Load(L"rom\\Texture\\gra_effect_lightA.png");


	g_Pinmanager.Initialize();


	g_pTestAnim = new AnimPattern(g_texid, 10, 5, 0.1, { 0,0 }, { 140,200});
	g_pAnimPlayer = new AnimPatternPlayer(g_pTestAnim);
	g_FrameCount = 0;
	g_ShotCount = 0;
	g_PrevDownPinCount = 0;

	TrailExplosion_Initialize();

}
static double g_time{ 0.0 };
void Game_Update(double elapsed_time)
{
    // 投球、ピン、スコア、演出を経過時間に合わせて更新する。
	g_time += elapsed_time;


	g_BowlingBall.Update(elapsed_time);
	Shot_SetPosition(g_BowlingBall.GetPosition()); //  追加
	UI::Update(elapsed_time);


	Shot_Update(elapsed_time);

	Trail_AddPosition(Ball_GetPosition());
	Billboard_Update(elapsed_time);
	for (int i = 0; i < 2; i++)
	{
		g_FixedCameras[i]->SetTarget(Ball_GetPosition());
		if (g_FixedCameras[i]->IsOverlap())
		{
			g_FixedCameraIndex = i;
			break;
		}
	}
	g_FixedCameras[g_FixedCameraIndex]->SetMatrix();


	Billboard_SetViewMatrix(g_pDebugCamera->GetViewMatrix());
	TrailExplosion_SetCameraPosition(g_pDebugCamera->GetPosition());
 // 爆発更新
	TrailExplosion_Update(elapsed_time);

	Score_Update();
	if (KeyLogger_IsPressed(KK_R))
	{
		g_Pinmanager.ResetPins();
	}
	if (KeyLogger_IsPressed(KK_T))
	{
		Scene_SetNextScene(SCENE_RESULT);
	}




	bool fired = Shot_ConsumeFire();

	if (!g_BallInPlay && fired)
	{
		// 投球直前の倒れているピン数を記録（基準）
		g_PrevDownPinsThisThrow = g_Pinmanager.GetDownPinCount();
		// 追加: 投球直前の「残っているピン数」も記録しておく（削除を踏まえた差分算出用）
		g_PrevRemainingPinsThisThrow = g_Pinmanager.GetRemainingPinCount();


		g_BowlingBall.AddForce(Shot_GetShotVelocity());
		Shot_ResetPower();
		g_BallInPlay = true;
	}

	g_pDebugCamera->Update(elapsed_time);

	if (g_BallInPlay)
	{
		g_pDirectionCamera->Update(elapsed_time);
	}
	else
	{
		g_pDirectionCamera->ResetCamera();
	}


	if (g_BallInPlay && g_BowlingBall.IsStopped())
	{
		g_BallInPlay = false;
		g_WaitingPinSettle = true;
		g_PinSettleTimer = 0.0f;
		// ※ 記録は投球時に行うためここで上書きしない
	}
	if (g_WaitingPinSettle)
	{
		g_PinSettleTimer += elapsed_time;

		if (g_PinSettleTimer >= PIN_SETTLE_TIME)
		{

			g_WaitingPinSettle = false;
			g_ShotCount++;

			int nowDown = g_Pinmanager.GetDownPinCount();
			g_Pinmanager.RemoveDeadPins();

			int remainingAfterRemove = g_Pinmanager.GetRemainingPinCount();
			int fallenPins = g_PrevRemainingPinsThisThrow - remainingAfterRemove;
			if (fallenPins < 0) fallenPins = 0;
			fallenPins = static_cast<int>(GameClamp((float)fallenPins, 0.0f, 10.0f));

			// 
			bool isStrike = (fallenPins == 10 && g_ShotCount == 1);
			bool isSpare = (fallenPins > 0 && remainingAfterRemove == 0 && g_ShotCount == 2);
			bool isGutter = (fallenPins == 0);



			// UI通知を表示
			if (isStrike)
			{
				UI::ShowNotification(UI::NotificationType::STRIKE);
				//PlayAudio(g_StrikeSound,false);
			}
			else if (isSpare)
			{
				UI::ShowNotification(UI::NotificationType::SPARE);
				//PlayAudio(g_HitSound,false);
			}
			else if (isGutter)
			{
				UI::ShowNotification(UI::NotificationType::GUTTER);
			}

			// 

			Score_AddThrow(fallenPins);
			g_BowlingBall.Reset(BALL_START_POS);

			if (isStrike || g_ShotCount >= MAX_SHOT)
			{
				g_Pinmanager.DestroyAndRecreatePins();
				g_PrevDownPinCount = 0;
				g_ShotCount = 0;
				g_FrameCount++;

				if (g_FrameCount >= MAX_FRAME)
				{
					Scene_SetNextScene(SCENE_RESULT);
					return;
				}
			}
		}
	}
	g_Pinmanager.Update(elapsed_time, g_BowlingBall);

	Hit hit = g_GoalAABB.IsHit(Ball_GetAABB());
	//上向き法線がTrueだったらのを取ればまぁいいよね
	if (hit.IsHit()) {

		//ゴール演出

		g_IsGoal = true;

	}

	g_pAnimPlayer->Update(elapsed_time);

	if(g_pAnimPlayer2)
	{
		g_pAnimPlayer2->Update(elapsed_time);
		if (g_pAnimPlayer2->IsAnimStopeed())
		{
			delete g_pAnimPlayer2;
			delete g_pTestAnim2;
			g_pAnimPlayer2 = nullptr;
		}
	}

	//AuraEffect_Update(elapsed_time);
	ParticleEffect_Update(elapsed_time);


}

void Game_Draw()
{
    // 現在のゲーム状態に応じて3Dオブジェクトと画面UIを描画する。

	Direct3D_SetDepthTest(true);
	if (g_BallInPlay)
	{
		g_pDirectionCamera->SetMatrix();
	}
	else
	{
		g_pDebugCamera->SetMatrix();
	}

	Billboard_SetViewMatrix(g_pDebugCamera->GetViewMatrix());


	g_BowlingBall.Draw();
	g_Pinmanager.Draw();

	Gulid_Draw();
	Light_SetAmnient({0.5f,0.5f,0.6f,1.0f});
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, { -1.0f,-1.5f,1.0f });
	Light_SetDiffuse({ 0.5f,0.5f,0.5f }, direction);


		Shot_Draw();








	Map_Draw();
	TrailExplosion_Draw();
	//2D描画はここに
	Direct3D_SetDepthTest(false);
	Shot_DrawUI();
	ScoreBoard_Draw();
	UI::Draw();


	if(g_IsGoal)
	{
		XMFLOAT3 pos = g_GoalAABB.GetCenter();
		pos.y += 5.0f;
		if (g_pAnimPlayer2)
		{
			g_pAnimPlayer2->BillboardDraw({ pos }, { 1.4,2.0 }, { 0,0 });
		}
	}


}

void Game_Finalize()
{
    // ゲーム中に確保したモデルや各システムのリソースを解放する。
	delete g_pTestAnim;
	delete g_pDebugCamera;
	delete g_pShotCamera;
	ModelRelease(g_pKirby);
	DebugDraw_Finalize();
	Ball_Finalize();
	Map_Finalize();
	Gulid_Finalize();
	Light_Finalize();
	Shot_Finalize();
	Trail_Finalize();
	TrailExplosion_Finalize();
	UI::Finalize();
	g_Pinmanager.Finalize();

}



float GameClamp(float v, float minV, float maxV)
{
	if (v < minV) return minV;
	if (v > maxV) return maxV;
	return v;
}