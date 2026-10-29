/*==========================================================================
ステージの初期化、描画など[Map.cpp]





---------------------------------------------------------------------------



==========================================================================*/


#include "map.h"
#include "cube.h"
#include "texture.h"
#include <DirectXMath.h>
#include "model.h"
#include "DebugDraw.h"
using namespace DirectX;

constexpr int TEXTURE_MAX = 100;
static int g_MapTexId[TEXTURE_MAX]{};
static constexpr int MODEL_MAX = 8;
static MODEL* g_MapModels[MODEL_MAX]{};
constexpr int   LANE_COUNT = 5;     // レーン数

constexpr int LEFT_LANES = 5;
constexpr int RIGHT_LANES = 5;
constexpr float LANE_WIDTH = 15.0f;
constexpr float BASE_X = 0.0f;
constexpr float LANE_EXPAND = 3.0f;          // ← 追加
constexpr float FLOOR_WIDTH = 8.0f + LANE_EXPAND;
constexpr float FLOOR_CENTER_X = 4.0f + LANE_EXPAND * 0.5f;
constexpr float GUTTER_OFFSET_LEFT = -0.50f - LANE_EXPAND * 0.5f;
constexpr float GUTTER_OFFSET_RIGHT = 8.50f + LANE_EXPAND * 0.5f;
constexpr float FLOOR_HALF_WIDTH = FLOOR_WIDTH * 0.5f;
// 床の左右端（世界座標基準）
constexpr float FLOOR_LEFT_X = FLOOR_CENTER_X - FLOOR_HALF_WIDTH;
constexpr float FLOOR_RIGHT_X = FLOOR_CENTER_X + FLOOR_HALF_WIDTH;

// ガーター幅
constexpr float GUTTER_WIDTH = 1.5f;

// ガーター中心位置（床にぴったり接する）
constexpr float GUTTER_CENTER_LEFT = FLOOR_LEFT_X - GUTTER_WIDTH * 0.5f;
constexpr float GUTTER_CENTER_RIGHT = FLOOR_RIGHT_X + GUTTER_WIDTH * 0.5f;
std::vector<Block> g_Blocks;



const Block& MapGetBlock(int index)
{
    return g_Blocks[index];
}

int MapGetBlockCount()
{
    return static_cast<int>(g_Blocks.size());
}

const AABB& GetCollision(int index)
{
    return g_Blocks[index].GetAABB();
}

void Map_Initialize()
{
    // ステージのブロック配置、モデル、当たり判定を初期化する。


    for (int lane = -LEFT_LANES; lane <= RIGHT_LANES; ++lane)
    {
        float xOffset = BASE_X + lane * LANE_WIDTH;

        // 床
        g_Blocks.push_back({
            { FLOOR_CENTER_X + xOffset, 1.0f, 10.0f },
            { FLOOR_WIDTH, 1.0f, 160.0f },
               Block::Wood
            });
        // 左ガーター
        g_Blocks.push_back({
            { GUTTER_CENTER_LEFT + xOffset, 0.9f, -10.0f },
            { GUTTER_WIDTH, 1.0f, 160.0f },
            Block::Gutter
            });

        // 右ガーター
        g_Blocks.push_back({
            { GUTTER_CENTER_RIGHT + xOffset, 0.9f, -10.0f },
            { GUTTER_WIDTH, 1.0f, 160.0f },
            Block::Gutter
            });



        // 奥の壁
        g_Blocks.push_back({
            { FLOOR_CENTER_X + xOffset, 1.0f, 32.0f },
            { FLOOR_WIDTH+3, 20.0f, 30.0f },
            Block::WALL
            });

        // 手前側の壁（奥の壁より少し手前）
        g_Blocks.push_back({
            { FLOOR_CENTER_X + xOffset, 1.0f, 20.0f },
            { FLOOR_WIDTH+4, 100.0f, 2.0f },   // 薄めでOK
            Block::WALL3
            });

     /*   g_Blocks.push_back({
            { 3.0f + xOffset, 1.0f, 32.0f },
            { 15.0f, 10.0f, 30.0f },
            Block::Moon
            });*/

        // ===== ガーター外側の進入禁止壁 =====
        constexpr float wallHeight = 2.0f;
        constexpr float wallThickness = 0.5f;
        constexpr float wallLength = 160.0f;
        constexpr float zPos = -10.0f;

        constexpr float WALL2_OFFSET = GUTTER_WIDTH * 0.5f + wallThickness * 0.5f;

        // 左 WALL2
        g_Blocks.push_back({
            { GUTTER_CENTER_LEFT - WALL2_OFFSET + xOffset,
              wallHeight * 0.5f,
              zPos },
            { wallThickness, wallHeight, wallLength },
            Block::WALL2
            });

        // 右 WALL2
        g_Blocks.push_back({
            { GUTTER_CENTER_RIGHT + WALL2_OFFSET + xOffset,
              wallHeight * 0.5f,
              zPos },
            { wallThickness, wallHeight, wallLength },
            Block::WALL2
            });


    }



    g_MapTexId[0] = Texture_Load(L"rom\\Texture\\kankyaku.jpg");
    g_MapTexId[1] = Texture_Load(L"rom\\Texture\\jimen.jpg");
    g_MapTexId[2] = Texture_Load(L"rom\\Texture\\kuro.png");
    //sillber
    g_MapTexId[3] = Texture_Load(L"rom\\Texture\\sillber.jpg");


    g_MapModels[0] = ModelLoad("rom\\Model\\jimen.fbx");
    g_MapModels[1] = ModelLoad("rom\\Model\\GoalPost.fbx");
    g_MapModels[2] = ModelLoad("rom\\Model\\UFO.fbx");
    g_MapModels[3] = ModelLoad("rom\\Model\\Star.fbx",2);
    g_MapModels[4] = ModelLoad("rom\\Model\\StarCore.fbx", 1);

    g_Blocks.push_back({ {1,1,1}, { 10, 10, 10 }, Block::Normal});

    for (Block& block : g_Blocks) {
        switch (block.GetType())
        {
        case Block::Normal:
            block.SetAABB(AABB::Make(block.GetPosition(), { 1.0, 1.0, 1.0 }));
            break;



        case Block::Wood:
            block.SetAABB(AABB::Make(block.GetPosition(), block.GetScale()));
            break;

        case Block::WALL:
        {
            XMFLOAT3 blockScale = block.GetScale();
            blockScale.z -= 5;
            block.SetAABB(AABB::Make(block.GetPosition(), blockScale));
        }
            break;
      /*  case Block::Gutter:
        {
            XMFLOAT3 blockScale = block.GetScale();
            blockScale.y += 5;
            block.SetAABB(AABB::Make(block.GetPosition(), blockScale));
        }
            break;*/


        case Block::WALL2:

        {
            XMFLOAT3 blockScale = block.GetScale();
            blockScale.y += 5;
            block.SetAABB(AABB::Make(block.GetPosition(), blockScale));
        }
            break;
        case Block::GOAL:
           // block.SetAABB(AABB::Make(block.GetPosition(), { 1.0, 1.0, 1.0 }));
            break;

        case Block::UFO:
            block.SetAABB(AABB::Make(block.GetPosition(), { 1.0, 1.0, 1.0 }));
            break;



        default:
            break;
        }
    }
}

void Map_Finalize()
{
    for (int i = 0; i < MODEL_MAX; i++)
    {
        if (g_MapModels[i]) {
            ModelRelease(g_MapModels[i]);
            g_MapModels[i] = nullptr;
        }
    }
}

void Map_Draw()
{
    // ステージに配置されたブロックやモデルを順に描画する。
    for (auto& block : g_Blocks)
    {
        block.Draw();
    }
   // DebugDraw_Draw();
}

void Block::Draw() const
{
    XMMATRIX pos = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    XMMATRIX scale = XMMatrixScaling(
        m_Scale.x,
        m_Scale.y,
        m_Scale.z
    );

    XMMATRIX mtxworld = scale*pos ;

    switch (m_Type)
    {
    case Block::EMPTY:
        break;
    case Block::Normal:
        //ModelDraw(g_MapModels[5], XMMatrixIdentity());
        break;

    case Block::WALL2:
        Cube_Draw(mtxworld, g_MapTexId[3]);
        break;
    case Block::WALL3:
        Cube_Draw(mtxworld, 0);
        break;
    case Block::Audience:
        Cube_Draw(mtxworld, g_MapTexId[0]);
        break;
    case Block::Wood:
        Cube_Draw(mtxworld, g_MapTexId[1]);
        //ModelDraw(g_MapModels[0],mtxworld);
        break;
    case Block::WALL:
    case Block::Gutter:

        Cube_Draw(mtxworld, g_MapTexId[2]);
        break;

    case Block::Tree:
    case Block::Rock:

    case Block::START:
    case Block::GOAL:
        ModelDraw(g_MapModels[1], mtxworld);
        break;
    case Block::Sand:
    case Block::TYPE_MAX:
    case Block::UFO:
        ModelDraw(g_MapModels[2], mtxworld);
        break;

    case Block::Star:
        ModelDraw(g_MapModels[3], mtxworld);
        break;
    case Block::StarCore:
        ModelDraw(g_MapModels[4], mtxworld);

        break;
	case Block::Moon:
        Cube_Draw(mtxworld, g_MapTexId[0]);
		break;

    default:
        break;
    }
    // デバッグ描画：AABB を赤いラインで表示
    DebugDraw_AddAABB(m_Aabb, { 1.0f, 0.0f, 0.0f, 1.0f });

}
