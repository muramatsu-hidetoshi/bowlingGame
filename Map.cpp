
/*==========================================================================
ステージの初期化・描画・管理 [Map.cpp]

ステージを構成する床、ガーター、壁などのブロックを配置する。
ブロックの当たり判定を作成し、テクスチャやモデルを使って描画する。
==========================================================================*/

#include "map.h"
#include "cube.h"
#include "texture.h"
#include <DirectXMath.h>
#include "model.h"
#include "DebugDraw.h"

using namespace DirectX;

// テクスチャIDの配列サイズ
constexpr int TEXTURE_MAX = 100;

// ステージで使用するテクスチャ
static int g_MapTexId[TEXTURE_MAX]{};

// モデルの最大登録数
static constexpr int MODEL_MAX = 8;

// ステージで使用するモデル
static MODEL* g_MapModels[MODEL_MAX]{};

// レーンとステージの配置設定
constexpr int LANE_COUNT = 5;
constexpr int LEFT_LANES = 5;
constexpr int RIGHT_LANES = 5;
constexpr float LANE_WIDTH = 15.0f;
constexpr float BASE_X = 0.0f;

// 床の幅と中心位置を調整する値
constexpr float LANE_EXPAND = 3.0f;
constexpr float FLOOR_WIDTH = 8.0f + LANE_EXPAND;
constexpr float FLOOR_CENTER_X = 4.0f + LANE_EXPAND * 0.5f;
constexpr float GUTTER_OFFSET_LEFT = -0.50f - LANE_EXPAND * 0.5f;
constexpr float GUTTER_OFFSET_RIGHT = 8.50f + LANE_EXPAND * 0.5f;
constexpr float FLOOR_HALF_WIDTH = FLOOR_WIDTH * 0.5f;

// 床の左右端のワールド座標
constexpr float FLOOR_LEFT_X = FLOOR_CENTER_X - FLOOR_HALF_WIDTH;
constexpr float FLOOR_RIGHT_X = FLOOR_CENTER_X + FLOOR_HALF_WIDTH;

// ガーターの幅と中心位置
constexpr float GUTTER_WIDTH = 1.5f;
constexpr float GUTTER_CENTER_LEFT = FLOOR_LEFT_X - GUTTER_WIDTH * 0.5f;
constexpr float GUTTER_CENTER_RIGHT = FLOOR_RIGHT_X + GUTTER_WIDTH * 0.5f;

// ステージを構成するブロックの一覧
std::vector<Block> g_Blocks;

// 指定された番号のブロックを取得する
const Block& MapGetBlock(int index)
{
    return g_Blocks[index];
}

// 登録されているブロック数を取得する
int MapGetBlockCount()
{
    return static_cast<int>(g_Blocks.size());
}

// 指定されたブロックの当たり判定を取得する
const AABB& GetCollision(int index)
{
    return g_Blocks[index].GetAABB();
}

// ステージの初期化
void Map_Initialize()
{
    // 各レーンに床、ガーター、壁を配置する
    for (int lane = -LEFT_LANES; lane <= RIGHT_LANES; ++lane)
    {
        // レーンごとのX方向の配置位置
        float xOffset = BASE_X + lane * LANE_WIDTH;

        // 床ブロックを追加する
        g_Blocks.push_back({
            { FLOOR_CENTER_X + xOffset, 1.0f, 10.0f },
            { FLOOR_WIDTH, 1.0f, 160.0f },
            Block::Wood
            });

        // 左側のガーターを追加する
        g_Blocks.push_back({
            { GUTTER_CENTER_LEFT + xOffset, 0.9f, -10.0f },
            { GUTTER_WIDTH, 1.0f, 160.0f },
            Block::Gutter
            });

        // 右側のガーターを追加する
        g_Blocks.push_back({
            { GUTTER_CENTER_RIGHT + xOffset, 0.9f, -10.0f },
            { GUTTER_WIDTH, 1.0f, 160.0f },
            Block::Gutter
            });

        // 奥側の壁を追加する
        g_Blocks.push_back({
            { FLOOR_CENTER_X + xOffset, 1.0f, 32.0f },
            { FLOOR_WIDTH + 3, 20.0f, 30.0f },
            Block::WALL
            });

        // 手前側の壁を追加する
        g_Blocks.push_back({
            { FLOOR_CENTER_X + xOffset, 1.0f, 20.0f },
            { FLOOR_WIDTH + 4, 100.0f, 2.0f },
            Block::WALL3
            });

        // ガーターの外側に進入禁止壁を設置する
        constexpr float wallHeight = 2.0f;
        constexpr float wallThickness = 0.5f;
        constexpr float wallLength = 160.0f;
        constexpr float zPos = -10.0f;

        // ガーター端から壁までの距離
        constexpr float WALL2_OFFSET =
            GUTTER_WIDTH * 0.5f + wallThickness * 0.5f;

        // 左側の進入禁止壁を追加する
        g_Blocks.push_back({
            {
                GUTTER_CENTER_LEFT - WALL2_OFFSET + xOffset,
                wallHeight * 0.5f,
                zPos
            },
            { wallThickness, wallHeight, wallLength },
            Block::WALL2
            });

        // 右側の進入禁止壁を追加する
        g_Blocks.push_back({
            {
                GUTTER_CENTER_RIGHT + WALL2_OFFSET + xOffset,
                wallHeight * 0.5f,
                zPos
            },
            { wallThickness, wallHeight, wallLength },
            Block::WALL2
            });
    }

    // ステージで使用するテクスチャを読み込む
    g_MapTexId[0] = Texture_Load(L"rom\\Texture\\kankyaku.jpg");
    g_MapTexId[1] = Texture_Load(L"rom\\Texture\\jimen.jpg");
    g_MapTexId[2] = Texture_Load(L"rom\\Texture\\kuro.png");
    g_MapTexId[3] = Texture_Load(L"rom\\Texture\\sillber.jpg");

    // ステージで使用するモデルを読み込む
    g_MapModels[0] = ModelLoad("rom\\Model\\jimen.fbx");
    g_MapModels[1] = ModelLoad("rom\\Model\\GoalPost.fbx");
    g_MapModels[2] = ModelLoad("rom\\Model\\UFO.fbx");
    g_MapModels[3] = ModelLoad("rom\\Model\\Star.fbx", 2);
    g_MapModels[4] = ModelLoad("rom\\Model\\StarCore.fbx", 1);

    // テスト用の通常ブロックを追加する
    g_Blocks.push_back({
        { 1, 1, 1 },
        { 10, 10, 10 },
        Block::Normal
        });

    // 各ブロックの種類に応じて当たり判定を設定する
    for (Block& block : g_Blocks)
    {
        switch (block.GetType())
        {
        case Block::Normal:
            // 通常ブロックの当たり判定を作成する
            block.SetAABB(
                AABB::Make(block.GetPosition(), { 1.0, 1.0, 1.0 }));
            break;

        case Block::Wood:
            // 床の大きさに合わせて当たり判定を作成する
            block.SetAABB(
                AABB::Make(block.GetPosition(), block.GetScale()));
            break;

        case Block::WALL:
        {
            // 奥側の壁はZ方向の当たり判定を少し縮める
            XMFLOAT3 blockScale = block.GetScale();
            blockScale.z -= 5;
            block.SetAABB(
                AABB::Make(block.GetPosition(), blockScale));
        }
        break;

        case Block::WALL2:
        {
            // 進入禁止壁の当たり判定を設定する
            XMFLOAT3 blockScale = block.GetScale();
            blockScale.y += 5;
            block.SetAABB(
                AABB::Make(block.GetPosition(), blockScale));
        }
        break;

        case Block::GOAL:
            // ゴールの当たり判定は必要に応じて設定する
            break;

        case Block::UFO:
            // UFOの当たり判定を設定する
            block.SetAABB(
                AABB::Make(block.GetPosition(), { 1.0, 1.0, 1.0 }));
            break;

        default:
            // その他の種類は個別の設定を行わない
            break;
        }
    }
}

// ステージの終了処理
void Map_Finalize()
{
    // 読み込んだモデルを解放する
    for (int i = 0; i < MODEL_MAX; i++)
    {
        if (g_MapModels[i])
        {
            ModelRelease(g_MapModels[i]);
            g_MapModels[i] = nullptr;
        }
    }
}

// ステージの描画
void Map_Draw()
{
    // 登録されているブロックを順番に描画する
    for (auto& block : g_Blocks)
    {
        block.Draw();
    }

    // デバッグ描画が必要な場合は有効にする
    // DebugDraw_Draw();
}

// ブロックを描画する
void Block::Draw() const
{
    // ブロックの位置と大きさから変換行列を作成する
    XMMATRIX pos =
        XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

    XMMATRIX scale =
        XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

    // 拡大縮小と平行移動を適用する
    XMMATRIX mtxworld = scale * pos;

    // ブロックの種類に応じて描画方法を切り替える
    switch (m_Type)
    {
    case Block::EMPTY:
        // 何も描画しない
        break;

    case Block::Normal:
        // 通常ブロックの描画
        // ModelDraw(g_MapModels[5], XMMatrixIdentity());
        break;

    case Block::WALL2:
        // 進入禁止壁を銀色のテクスチャで描画する
        Cube_Draw(mtxworld, g_MapTexId[3]);
        break;

    case Block::WALL3:
        // 手前側の壁を描画する
        Cube_Draw(mtxworld, 0);
        break;

    case Block::Audience:
        // 観客席を描画する
        Cube_Draw(mtxworld, g_MapTexId[0]);
        break;

    case Block::Wood:
        // 床を地面のテクスチャで描画する
        Cube_Draw(mtxworld, g_MapTexId[1]);
        // ModelDraw(g_MapModels[0], mtxworld);
        break;

    case Block::WALL:
    case Block::Gutter:
        // 壁とガーターを黒いテクスチャで描画する
        Cube_Draw(mtxworld, g_MapTexId[2]);
        break;

    case Block::Tree:
    case Block::Rock:
    case Block::START:
    case Block::GOAL:
        // 指定された種類にゴールポストのモデルを使用する
        ModelDraw(g_MapModels[1], mtxworld);
        break;

    case Block::Sand:
    case Block::TYPE_MAX:
    case Block::UFO:
        // UFOモデルを描画する
        ModelDraw(g_MapModels[2], mtxworld);
        break;

    case Block::Star:
        // 星のモデルを描画する
        ModelDraw(g_MapModels[3], mtxworld);
        break;

    case Block::StarCore:
        // 星の中心部分のモデルを描画する
        ModelDraw(g_MapModels[4], mtxworld);
        break;

    case Block::Moon:
        // 月をキューブとして描画する
        Cube_Draw(mtxworld, g_MapTexId[0]);
        break;

    default:
        // 未対応の種類は描画しない
        break;
    }

    // デバッグ用にブロックの当たり判定を赤い線で表示する
    DebugDraw_AddAABB(m_Aabb, { 1.0f, 0.0f, 0.0f, 1.0f });
}

