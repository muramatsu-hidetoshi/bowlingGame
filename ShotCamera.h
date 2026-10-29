/*==========================================================================


Shotカメラインターフェイス[shotcamera.h]

---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <DirectXMath.h>
#include "camera.h"
class ShotCamera : public Camera
{

private:

	DirectX::XMFLOAT3 m_Front{ 0.0,0.0,1.0 };
	DirectX::XMFLOAT3 m_right{ 1.0,0.0,0.0 };
	DirectX::XMFLOAT3 m_up{ 0.0,1.0,0.0 };
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_target_a{};
	DirectX::XMFLOAT3 m_target_b{};
	float m_distance{};
	float m_fov = 1.0;

public:
	ShotCamera( const DirectX::XMFLOAT3& target_a, const DirectX::XMFLOAT3& target_b,float distance );
	//virtual ~Camera() = default;
	//void Update(double elapsed_time)override;//キー入力待ち
	void SetMatrix() override;

	void SetTarget(const DirectX::XMFLOAT3& target_b) {
		m_target_b = target_b;
	}
	//DirectX::XMFLOAT3 GetFront()const { return m_Front; }
};