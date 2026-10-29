/*==========================================================================
エフェクト制御[game_effect.h]



---------------------------------------------------------------------------






=========================================================================*/

#include "gameeffect.h"
#include "texture.h"
#include "sprite.h"
#include "direct3d.h"
using namespace DirectX;

struct Effect
{
	XMFLOAT2 position;
	float size;
	double lifeTime;
	double birthTime;
	XMFLOAT4 color;

};

static constexpr int EFFECT_MAX = 128;
static Effect g_Effects[EFFECT_MAX];
static int g_EffectCount{0};
static double g_AccumulatedTime = 0.0;
static int g_TexId = -1;


void Effect_Initialize()
{
	g_TexId = Texture_Load(L"rom\\aa.png");
	for (Effect& e : g_Effects)
	{
		e.lifeTime = 0.0;
	}
	g_EffectCount = 0.0;
	g_AccumulatedTime = 0.0;

}

void Effect_Update(double elapsed_time)
{

	g_AccumulatedTime += elapsed_time;
	for (int i = g_EffectCount - 1; i >= 0; i--)
	{
		double time = g_AccumulatedTime - g_Effects[i].birthTime;
		if (g_Effects[i].lifeTime <= time) {
			g_Effects[i].lifeTime = 0.0f;
			g_Effects[i] = g_Effects[--g_EffectCount];
		}
	}
}

void Effect_Draw()
{

	Direct3D_SetAlphaBlend(BLEND_ADD);

	for (int i = 0; i < g_EffectCount; i++)
	{
		double time = g_AccumulatedTime - g_Effects[i].birthTime;
		float ratio = time / g_Effects[i].lifeTime;
		float alpha = 1.0f - ratio;
		float size = g_Effects[i].size * (1.0f - ratio);
		float half = g_Effects[i].size * 0.5;
		float dx = g_Effects[i].position.x + half - size*0.5f;
		float dy = g_Effects[i].position.y + half - size * 0.5f;

		XMFLOAT4 color = g_Effects[i].color;
		color.w = alpha;
		Sprite_Draw(g_TexId, dx, dy, size, size, 0.0f, 0.0f, Texture_GetWidht(g_TexId), Texture_GetHeight(g_TexId), color);


	}

	Direct3D_SetAlphaBlend(BLEND_TRANSPARENT);
}

void Effect_Finalize()
{


}

void Effect_Create(const DirectX::XMFLOAT2& position, float size, double lifeTime, const DirectX::XMFLOAT4& color)
{

	if (g_EffectCount >= EFFECT_MAX)return;
	g_Effects[g_EffectCount].position = position;
	g_Effects[g_EffectCount].size = size;
	g_Effects[g_EffectCount].lifeTime = lifeTime;
	g_Effects[g_EffectCount].birthTime = g_AccumulatedTime;
	g_Effects[g_EffectCount].color = color;
	g_EffectCount++;
}
