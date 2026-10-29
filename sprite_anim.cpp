
/*==========================================================================

	スプライトシートを使った2Dアニメーション管理機能[sprite_anim.cpp]




---------------------------------------------------------------------------



==========================================================================*/

#include "sprite_anim.h"
#include"texture.h"
#include "sprite.h"
//#include "system_timer.h"
#include "BillBord.h"
using namespace DirectX;


//static int g_TextureID = -1;
//static constexpr int ANIM_PATTERN_MAX = 8; // アニメーションパターン数
static int g_pattern = 0; // 現在のアニメーションパターン
static double g_time = 0.0; // 時間管理用
static AnimPattern* pAP=nullptr ;




void AnimPattern::Draw(float dx, float dy, float dw,float dh,int pattern_num, DirectX::XMFLOAT4 color)
{

	//int pattern= g_pattern = pattern_num % m_pattern_max; // パターンを進める
	Sprite_Draw(m_texid, dx, dy, dw,dh,
		m_start_position.x + m_pattern_size.x * (pattern_num% m_horizontal_pattern_max),
		m_start_position.y + m_pattern_size.y * (pattern_num / m_horizontal_pattern_max), //?140*200   //00 100*200
		m_pattern_size.x, m_pattern_size.y,color); // スプライトの描画

}

void AnimPattern::BillboardDraw(
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT2& scal,
	int pattern_num, const DirectX::XMFLOAT2& pivot,
	DirectX::XMFLOAT4 color
)
{
	Billboard_Draw(
		m_texid,
		//XMMatrixTranslation(position.x, position.y, position.z),
		XMMatrixTranslationFromVector(XMLoadFloat3(&position)),
		{(float)m_start_position.x+m_pattern_size.x*(pattern_num%m_horizontal_pattern_max),
		 (float)m_start_position.y + m_pattern_size.y * (pattern_num / m_horizontal_pattern_max)
		},
		{
		   (float)m_pattern_size.x,
		   (float)m_pattern_size.y
		},
		scal,
		color,
		pivot
	);


	/*
	int texId,
	const DirectX::XMMATRIX& mtxWorld,
	const DirectX::XMFLOAT2& uv_offset,
	const DirectX::XMFLOAT2& uv_size,
	const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT4& color,
	const DirectX::XMFLOAT2& pivot

	*/



}

void AnimPatternPlayer::Update(double elapsed_time)
{
    // 経過時間をフレーム時間へ加算し、必要な回数だけアニメーションを進める。
	double a;
	m_accumulated_time += elapsed_time; // 時間を更新

	a = m_pAnimPattern->GetSecondPerPattern();

	if (m_accumulated_time >= a) {

		m_pattern++;


		if(m_pAnimPattern->IsLoop())
		{
			m_pattern = (m_pattern ) % m_pAnimPattern->GetPatternMax(); // パターンを進める

		}
		else
		{
			if (m_pattern == m_pAnimPattern->GetPatternMax())
			{
				m_pattern = m_pAnimPattern->GetPatternMax() - 1;
				m_is_stopped = true;
			}
		}

		m_accumulated_time -= a; // 時間をリセット
	}




}

void AnimPatternPlayer::Draw(float dx, float dy, float dw, float dh, DirectX::XMFLOAT4 color)
{

	m_pAnimPattern->Draw(dx, dy, dw, dh, m_pattern, color); // AnimPatternの描画


}

void AnimPatternPlayer::BillboardDraw(
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT2& scal,
	const DirectX::XMFLOAT2& pivot,
	DirectX::XMFLOAT4 color

)
{
	m_pAnimPattern->BillboardDraw(position, scal, m_pattern, pivot, color);
	//Billboard_Draw(m_texid,dx,dy,dw,dh,);
}

