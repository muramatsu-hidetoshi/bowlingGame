
/*==========================================================================
スプライトアニメーションの描画



---------------------------------------------------------------------------






=========================================================================*/
#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H
#include <DirectXMath.h>




class AnimPattern 

{

private:

	int m_texid; // テクスチャID
	int m_pattern_max; // パターン数
	int m_horizontal_pattern_max; // 垂直パターン数（オプション、必要なら使用）
	double m_second_per_pattern; // パターンごとの秒数
	DirectX::XMUINT2 m_start_position; // 開始位置（テクスチャ内の座標）
	DirectX::XMUINT2 m_pattern_size; // パターンのサイズ（幅と高さ）
	bool m_is_loop; // ループフラグ
	bool m_is_Stopped{false};
	//int currentPattern; // 現在のパターン
	//double timeElapsed; // 経過時間
	//double frameDuration; // フレームの持続時間

public:

	AnimPattern() = delete;
	AnimPattern(
		int texid,
		int pattern_max,
		int horizontal_pattern_max, // 垂直パターン数（オプション、必要なら使用）
		double second_per_pattern,
		DirectX::XMUINT2 start_position,
		DirectX::XMUINT2 pattern_size,
		bool is_loop = true
	) 
		:m_texid(texid),
		m_pattern_max(pattern_max),
		m_horizontal_pattern_max(horizontal_pattern_max), // 垂直パターン数（オプション、必要なら使用）
		m_second_per_pattern(second_per_pattern),
		m_start_position(start_position),
		m_pattern_size(pattern_size),
		m_is_loop(is_loop)
	{
	}
	virtual ~AnimPattern() = default;


	virtual void Draw(
		float dx, float dy, 
		float dw, float dh, 
		int pattern_num, 
		DirectX::XMFLOAT4 color);

	virtual void BillboardDraw(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& scal,
		int pattern_num,
		const DirectX::XMFLOAT2& pivot,
		DirectX::XMFLOAT4 color
		);


	int GetPatternMax() const { return m_pattern_max; } // パターン数を取得
	double GetSecondPerPattern() { return m_second_per_pattern; } // パターンごとの秒数を取得
	bool IsLoop()  { return m_is_loop; } // ループフラグを取得
	bool IsStoped() { return m_is_Stopped; }


};

//class AnimPatternBillborld :public  AnimPattern
//{
//
//
//public:
//	AnimPatternBillborld(
//		int texid,
//		int pattern_max,
//		int horizontal_pattern_max, // 垂直パターン数（オプション、必要なら使用）
//		double second_per_pattern,
//		DirectX::XMUINT2 start_position,
//		DirectX::XMUINT2 pattern_size,
//		bool is_loop = true
//	)AnimPattern(texid, pattern_max, horizontal_pattern_max, 
//		second_per_pattern, start_position, pattern_size, is_loop)
//	{
//
//	}
//	void Draw(
//		float dx, float dy,
//		float dw, float dh,
//		int pattern_num,
//		DirectX::XMFLOAT4 color)override;
//
//};


class AnimPatternPlayer
{

private:

	int m_pattern = 0; // 現在のアニメーションパターン
	double m_accumulated_time = 0.0; // 累積時間
	AnimPattern* m_pAnimPattern = nullptr; // AnimPatternのポインタ
	bool m_is_stopped{ false };

public:

	AnimPatternPlayer() = delete;
	AnimPatternPlayer(AnimPattern* pAnimPattern)
		: m_pAnimPattern(pAnimPattern)
	{

	}

	void Update(double elapsed_time);
	void Draw(float dx, float dy, float dw, float dh,
		DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });

	void BillboardDraw(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT2& scal,
		const DirectX::XMFLOAT2& pivot,
		DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }

	);
	bool IsAnimStopeed()const { return m_is_stopped; }


};


#endif