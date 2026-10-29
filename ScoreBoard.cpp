
/*==============================================================================
    スコアボードの管理 [scoreBoard.cpp]

    投球結果をフレームごとに記録し、数字やストライク・スペアの記号を
    スコアボードへ描画する。
==============================================================================*/

#include "scoreBoard.h"
#include "debug_text.h"
#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"
#include "direct3d.h"

// スコア情報を保持する
Score score;

// スコアボードと数字画像のテクスチャID
static int g_ScoreBoardTexId = -1;
static int g_NumberTexId = -1;

// フレーム数と数字画像の表示設定
static constexpr int FRAME_COUNT = 4;
static constexpr float DIGIT_W = 64.0f;
static constexpr float DIGIT_H = 64.0f;
static constexpr int DIGIT_COLS = 5;
static constexpr float CELL_W = 200.0f;
static constexpr float CELL_H_NUM = 180.0f; // 数字0～9の画像の高さ
static constexpr float CELL_H_SYM = 185.0f; // X・スペア記号の画像の高さ

constexpr float SCALE_THROW = 0.45f;
constexpr float SCALE_SUM = 0.55f;

static constexpr int SHEET_COLS = 5;
constexpr float startX = 0.0f;
constexpr float startY = 230.0f;
constexpr float frameW = 430.0f / FRAME_COUNT;
constexpr float throwSpacing = CELL_W * SCALE_THROW + 6.0f;
constexpr float lowerRowHeight = CELL_H_NUM * SCALE_THROW + 10.0f;


// 数字画像内の表示記号を表す
enum DigitSymbol
{
    DIGIT_0 = 0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,
    SYMBOL_X,       // ストライク
    SYMBOL_SPARE,   // スペア
    SYMBOL_NONE,    // 空白
};

// 1フレーム分の投球情報
struct Frame
{
    int throw1 = -1;    // 1投目。-1は未記録
    int throw2 = -1;    // 2投目。-1は未記録
    int frameScore = 0; // フレームの得点
};

// 各フレームの投球情報と現在の入力位置
static Frame g_Frames[FRAME_COUNT];
static int g_CurrentFrame = 0;
static int g_CurrentThrow = 0;


// スコアボードを初期化する
void ScoreBoard_Initialize()
{
    // 表示に使用するテクスチャを読み込む
    g_ScoreBoardTexId = Texture_Load(L"rom\\Texture\\bord.jpg");
    g_NumberTexId = Texture_Load(L"rom\\Texture\\Number.png");

    // 全フレームの投球情報を初期状態に戻す
    for (int i = 0; i < FRAME_COUNT; ++i)
    {
        g_Frames[i] = {};
    }

    // 1フレーム目の1投目から記録を開始する
    g_CurrentFrame = 0;
    g_CurrentThrow = 0;

    // 合計得点を0に戻す
    score.SetTotalScore(0);
}








// 数字画像から指定した記号を切り出して描画する
void DrawSymbol(int texId, int symbol, float x, float y, float scale)
{
    // 未設定の記号や空白は描画しない
    if (symbol < 0 || symbol == SYMBOL_NONE)
        return;

    // 横方向の列番号を計算する
    int col = symbol % SHEET_COLS;

    // 記号に応じて画像内の行を決定する
    int row = 0;

    if (symbol >= DIGIT_0 && symbol <= DIGIT_4)
    {
        row = 0;
    }
    else if (symbol >= DIGIT_5 && symbol <= DIGIT_9)
    {
        row = 1;
    }
    else if (symbol == SYMBOL_X || symbol == SYMBOL_SPARE)
    {
        row = 2;
    }
    else
    {
        return;
    }

    // 切り出す画像領域の位置と高さを計算する
    float srcY = row * CELL_H_NUM;
    float srcH = (row == 2) ? CELL_H_SYM : CELL_H_NUM;

    // 指定した画像領域をスプライトとして描画する
    Sprite_Draw(
        texId,
        x,
        y,
        CELL_W * scale,
        srcH * scale,
        col * CELL_W,
        srcY,
        CELL_W,
        srcH,
        { 0, 0, 0, 1 }
    );
}


// スコア情報へのポインタを取得する
Score* GetScore()
{
    return &score;
}


// 合計得点に指定された点数を加算する
void ScoreSet(int a)
{
    score.Totalscore += a;
}


// 投球結果を記録し、次の投球位置へ進める
void Score_AddThrow(int pins)
{
    // 入力された値と現在の記録位置をデバッグ出力する
    hal::dout << "Score_AddThrow called: pins=" << pins
        << " currentFrame=" << g_CurrentFrame
        << " currentThrow=" << g_CurrentThrow << std::endl;

    // すべてのフレームが記録済みなら処理しない
    if (g_CurrentFrame >= FRAME_COUNT)
        return;

    // 投球本数が不正な範囲にならないように補正する
    if (pins < 0) pins = 0;

    if (pins > 10) {
        hal::dout << "  Warning: pins clamped from "
            << pins << " to 10" << std::endl;
        pins = 10;
    }

    // 現在記録中のフレームを参照する
    Frame& f = g_Frames[g_CurrentFrame];

    // 1投目の記録
    if (g_CurrentThrow == 0)
    {
        f.throw1 = pins;

        hal::dout << "  Assigned throw1=" << f.throw1
            << " (frame " << g_CurrentFrame << ")" << std::endl;

        // 10本倒した場合はストライクとして次のフレームへ進む
        if (pins == 10)
        {
            f.throw2 = -1;
            g_CurrentFrame++;
            g_CurrentThrow = 0;
        }
        else
        {
            // ストライクでなければ2投目を受け付ける
            g_CurrentThrow = 1;
        }
    }
    // 2投目の記録
    else
    {
        // 1投目が未記録の場合は、不整合を防ぐため0として扱う
        if (f.throw1 < 0) {
            hal::dout << "  Warning: throw1 was not set for frame "
                << g_CurrentFrame << std::endl;
            f.throw1 = 0;
        }

        // 1投目と2投目の合計が10本を超えないようにする
        int maxSecond = 10 - f.throw1;

        if (pins > maxSecond) {
            hal::dout << "  Warning: second throw pins (" << pins
                << ") exceed frame limit, clamped to "
                << maxSecond << std::endl;
            pins = maxSecond;
        }

        f.throw2 = pins;

        hal::dout << "  Assigned throw2=" << f.throw2
            << " (frame " << g_CurrentFrame << ")" << std::endl;

        // 2投目まで記録したら次のフレームへ進む
        g_CurrentFrame++;
        g_CurrentThrow = 0;
    }

    // 今回の投球で倒した本数を合計得点へ加算する
    ScoreSet(pins);
}





// スコアボードと各フレームの投球結果を描画する
void ScoreBoard_Draw()
{
#ifdef _DEBUG
    // デバッグ時は、各フレームの投球記録を出力する
    {
        std::ostringstream oss;
        oss << "ScoreBoard frames:";

        for (int i = 0; i < FRAME_COUNT; ++i) {
            oss << " [" << i << "]("
                << g_Frames[i].throw1 << ","
                << g_Frames[i].throw2 << ")";
        }

        hal::dout << oss.str() << std::endl;
    }
#endif // _DEBUG

    // スコアボードの背景を描画する
    Sprite_Draw(
        g_ScoreBoardTexId,
        0.0f,
        230.0f, 145.0f,
        430.0f, 145.0f,
        0.0f, 0.0f,
        430.0f, 145.0f
    );

    // 各フレーム内の数字の配置設定
    constexpr float startX = 0.0f;
    constexpr float startY = 100.0f;
    constexpr float frameW = 430.0f / FRAME_COUNT;
    constexpr float throwSpacing = 11.0f;
    constexpr float lowerRowHeight = 64.0f;
    constexpr float THROW_SCALE = 0.3f;

    float throwDigitW = CELL_W * THROW_SCALE;

    // 全フレームを順番に描画する
    for (int i = 0; i < FRAME_COUNT; ++i)
    {
        const Frame& f = g_Frames[i];
        float frameX = startX + frameW * i;

        // 1投目の記号を描画する
        if (f.throw1 >= 0)
        {
            int symbol = (f.throw1 == 10) ? SYMBOL_X : f.throw1;

            DrawSymbol(
                g_NumberTexId,
                symbol,
                frameX + 16.0f,
                startY,
                0.3f
            );
        }

        // 2投目の記号を描画する
        if (f.throw2 >= 0)
        {
            // 1投目と2投目の合計が10ならスペア記号を表示する
            int symbol =
                (f.throw1 < 10 && f.throw1 + f.throw2 == 10)
                ? SYMBOL_SPARE
                : f.throw2;

            DrawSymbol(
                g_NumberTexId,
                symbol,
                frameX + 5.0f + throwDigitW + 6.0f,
                startY,
                0.3f
            );
        }

        // 投球結果が記録されている場合、フレームの合計点を表示する
        if (f.throw1 >= 0)
        {
            int sum = f.throw1 + (f.throw2 >= 0 ? f.throw2 : 0);

            constexpr float SUM_SCALE = 0.32f;
            float digitW = CELL_W * SUM_SCALE;

            // 10以上の場合だけ、10の位を描画する
            if (sum >= 10)
            {
                DrawSymbol(
                    g_NumberTexId,
                    sum / 10,
                    frameX + 24.0f,
                    startY + lowerRowHeight,
                    SUM_SCALE
                );
            }

            // 1の位を描画する
            DrawSymbol(
                g_NumberTexId,
                sum % 10,
                frameX + 24.0f + (sum >= 10 ? (digitW - 30) : 0.0f),
                startY + lowerRowHeight,
                SUM_SCALE
            );
        }
    }
}

