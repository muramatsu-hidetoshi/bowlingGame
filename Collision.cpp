/*==========================================================================
コリジョン判定[collision.cpp]



---------------------------------------------------------------------------




衝突してきたやつがAABB

=========================================================================*/

#include "collision.h"
#include <algorithm>
#include "DirectXMath.h"
using namespace DirectX;


void AABB::Move(const XMFLOAT3& position)
{
    // 中心座標を更新し、中心と半径から最小点・最大点を再計算する。
	m_Center = position;
	m_Min = { m_Center.x - m_Half.x, m_Center.y - m_Half.y, m_Center.z - m_Half.z };
	m_Max = { m_Center.x + m_Half.x, m_Center.y + m_Half.y, m_Center.z + m_Half.z };
}

void AABB::Move(const XMVECTOR& position)
{
	XMStoreFloat3(&m_Center, position);
	XMStoreFloat3(&m_Min, position - XMLoadFloat3(&m_Half));
	XMStoreFloat3(&m_Max, position + XMLoadFloat3(&m_Half));
}


bool AABB::IsOverlap(const AABB& aabb)const
{
    // 3軸すべてで区間が重なる場合に、2つのAABBが重なっていると判定する。

	return    m_Min.x<aabb.GetMax().x
		   && m_Max.x>aabb.GetMin().x
		   && m_Min.y<aabb.GetMax().y
		   && m_Max.y>aabb.GetMin().y
		   && m_Min.z<aabb.GetMax().z
		   && m_Max.z>aabb.GetMin().z;

}

Hit AABB::IsHit(const AABB& aabb)const
{
    // 重なりがある場合、最小の押し戻し軸を衝突法線として返す。
	if (!IsOverlap(aabb)) {
		return {};
	}
	float depth_x = 
		std::min(GetMax().x, aabb.GetMax().x) - std::max(GetMin().x, aabb.GetMin().x);
	float depth_y = 
		std::min(GetMax().y, aabb.GetMax().y) - std::max(GetMin().y, aabb.GetMin().y);
	float depth_z =
		std::min(GetMax().z, aabb.GetMax().z) - std::max(GetMin().z, aabb.GetMin().z);

	int shallow_axis = 0;//x..0,y...1,z...2

	if (depth_x < depth_y){


		if (depth_x < depth_z)
		{

			shallow_axis = 0;
		}
		else
		{
			shallow_axis = 2;
		}
	}
	else{
		if (depth_y < depth_z)
		{
			shallow_axis = 1;
		}
		else
		{
			shallow_axis = 2;
		}
	}
	Hit hit{ true,{} };
	XMVECTOR difference = XMLoadFloat3(&aabb.GetCenter()) - XMLoadFloat3(&GetCenter());
	XMFLOAT3 normal{};
	switch (shallow_axis)
	{
	case 0:
		normal = aabb.GetCenter().x - GetCenter().x < 0.0f ? XMFLOAT3{ -1.0f, 0.0f, 0.0f } : XMFLOAT3{1.0f,0.0f,0.0f };
		break;

	case 1:

		normal = aabb.GetCenter().y - GetCenter().y < 0.0f ? XMFLOAT3{ 0.0f, -1.0f, 0.0f } : XMFLOAT3{ 0.0f,1.0f,0.0f };
		break;

	case 2:
		normal = aabb.GetCenter().z - GetCenter().z < 0.0f ? XMFLOAT3{ 0.0f, 0.0f, -1.0f } : XMFLOAT3{ 0.0f,0.0f,1.0f };

		break;

	default:
		break;
	}

	return { true,normal };

}

bool AABB::IsOverlap(const Point& point) const
{
	return    m_Min.x<point.GetPositin().x
		&& m_Max.x>point.GetPositin().x
		&& m_Min.y<point.GetPositin().y
		&& m_Max.y>point.GetPositin().y
		&& m_Min.z<point.GetPositin().z
		&& m_Max.z>point.GetPositin().z;
}


