/*==========================================================================


debugカメラインターフェイス[debugcamera.h]

---------------------------------------------------------------------------






=========================================================================*/

#pragma once
#include <DirectXMath.h>
#include "camera.h"
class DebugCamera : public Camera
{

private:
	DirectX::XMFLOAT3 m_Front{0.0,0.0,1.0};
	DirectX::XMFLOAT3 m_right{1.0,0.0,0.0};
	DirectX::XMFLOAT3 m_up{0.0,1.0,0.0};
	//DirectX::XMFLOAT3 m_Position;
	float m_fov = 1.0;

public:
	DebugCamera(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target);
	//virtual ~Camera() = default;
	 void Update(double elapsed_time)override;//キー入力待ち
	 void SetMatrix() override;
	 DirectX::XMFLOAT3 GetFront()const { return m_Front; }
};