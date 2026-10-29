
/*==========================================================================
当たり判定[collision.h]



---------------------------------------------------------------------------






=========================================================================*/
#pragma once

#include <DirectXMath.h>


class AABB;
class Point;

class Hit
{
private:
	bool m_isHit{false};
	DirectX::XMFLOAT3 m_Normal{};


public:
	Hit() = default;
	Hit(bool isHit, const DirectX::XMFLOAT3& normal)
		:m_isHit(isHit), m_Normal(normal)
	{

	}
	bool IsHit() const { return m_isHit; }
	const DirectX::XMFLOAT3& GetNormal() const { return m_Normal; }
	void SetNormal(const DirectX::XMFLOAT3& normal)
	{
		m_Normal = normal;
	}

};

class Collision
{
public:
	//他の形状が増えたらガンガン増やしていく
	//とりあえずFalse返しとく
	virtual ~Collision() = default;
	virtual bool IsOverlap(const AABB& aabb)const { return false; }//重なっているか
	virtual Hit  IsHit(const AABB& aabb)const { return {}; }
	virtual bool IsOverlap(const Point& aabb)const { return false; }//重なっているか
	virtual Hit  IsHit(const Point& aabb)const { return {}; }


};
class AABB : public Collision
{
private:
	DirectX::XMFLOAT3 m_Half{};
	DirectX::XMFLOAT3 m_Center{};
	DirectX::XMFLOAT3 m_Min{};
	DirectX::XMFLOAT3 m_Max{};

public:
	AABB() = default;
	AABB(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
		: m_Half((max.x - min.x) * 0.5f, (max.y - min.y) * 0.5f, (max.z - min.z) * 0.5f)
		, m_Center(min.x + m_Half.x, min.y + m_Half.y, min.z + m_Half.z)
		, m_Min(min), m_Max(max) {
	}

	static AABB Make(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& size) {
		return {
		 { center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f },
		 { center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f }
		};
	}

	void Move(const DirectX::XMFLOAT3& position);
	void Move(const DirectX::XMVECTOR& position);

	const DirectX::XMFLOAT3& GetMin() const { return m_Min; }
	const DirectX::XMFLOAT3& GetMax() const { return m_Max; }
	const DirectX::XMFLOAT3& GetCenter() const { return m_Center; }
	void SetCenter(const DirectX::XMFLOAT3& center) { m_Center = center; }
	const DirectX::XMFLOAT3& GetHalfSize() const { return m_Half; }

	bool IsOverlap(const AABB&) const override;
	Hit IsHit(const AABB&) const override;
	virtual bool IsOverlap(const Point& point)const override;//重なっているか

};

class Point: public Collision
{
private:
	DirectX::XMFLOAT3 m_pos;

public:
	Point() = default;
	Point(const DirectX::XMFLOAT3& position)
		:m_pos(position)
	{
	}

	const DirectX::XMFLOAT3& GetPositin()const { return m_pos; }




	bool IsOverlap(const AABB& aabb) const override {
		return aabb.IsOverlap(*this);
	}
};

