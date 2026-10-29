/*==========================================================================
演出用カメラ[DirectionCamera.h]




---------------------------------------------------------------------------



==========================================================================*/
#pragma once
#include <DirectXMath.h>
#include "camera.h"
class DirectionCamera : public Camera
{

private:
	DirectX::XMFLOAT3 m_Front{ 0.0,0.0,1.0 };
	DirectX::XMFLOAT3 m_right{ 1.0,0.0,0.0 };
	DirectX::XMFLOAT3 m_up{ 0.0,1.0,0.0 };
	DirectX::XMFLOAT3 m_Position{0,0,0};
	DirectX::XMFLOAT3 m_Target{ 0,0,0 };
	DirectX::XMFLOAT3 m_Current_Target{ 0,0,0 };

	float m_fov = 1.0;
	DirectX::XMFLOAT3 m_InitPosition;
	DirectX::XMFLOAT3 m_InitFront;
	DirectX::XMFLOAT3 m_InitUp;



	// Yイージング用
	float m_YEaseTime = 0.0f;
	bool  m_YEaseActive = false;

	float m_YStart = 0.0f;
	float m_YDown = -3.0f;   // どれだけ下げるか
	float m_YUp = 1.5f;     // どれだけ持ち上げるか
	float m_YEaseDuration = 0.6f; // 全体時間（秒）

public:
	DirectionCamera(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target);
	//virtual ~Camera() = default;
	void Update(double elapsed_time)override;//キー入力待ち
	void SetMatrix() override;
	DirectX::XMFLOAT3 GetFront()const { return m_Front; }
	void ResetCamera();
	void StartYEase();
};
float EaseInOut(float t);
