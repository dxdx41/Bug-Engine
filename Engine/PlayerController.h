#pragma once
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;

class PlayerController {
public:
	XMVECTOR GetView() {
		float yawRadians = XMConvertToRadians(m_Rotation.x);
		float pitchRadians = XMConvertToRadians(m_Rotation.y);

		XMVECTOR forward = XMVectorSet(
			cosf(pitchRadians) * sinf(yawRadians),
			sinf(pitchRadians),
			cosf(pitchRadians) * cosf(yawRadians),
			0.0f
		);
		return forward;
	}
	XMVECTOR GetForward() {
		float yawRadians = XMConvertToRadians(m_Rotation.x);

		XMVECTOR forward = XMVectorSet(
			sinf(yawRadians),
			0.0f,
			cosf(yawRadians),
			0.0f
		);
		return forward;
	}

	XMFLOAT3 m_Pos{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Rotation{ 0.0f, 0.0f, 0.0f };
};