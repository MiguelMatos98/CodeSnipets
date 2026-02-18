#include "Camera.h"
#include <cmath>
#include "GameManager.h"

using namespace DirectX;

Camera::Camera(const XMVECTOR& Position, const XMVECTOR& Rotation) 
	: GameObject(Position, Rotation), 
	  Pitch{ XM_PIDIV2 /*90° in radians*/}, 
	  Yaw{0.0f}, 
	  SkyboxModel{nullptr}, 
	  SkyboxScaleX{1000.0f},
	  SkyboxScaleY{1000.0f},
	  SkyboxScaleZ{1000.0f},
	  SkyboxTransform{}

{
	// Setting up the skybox transform
	XMVECTOR SkyboxPosition = XMVectorZero();;
	XMVECTOR SkyboxRotation = XMQuaternionIdentity();
	XMVECTOR SkyboxScale = XMVectorSet(SkyboxScaleX, SkyboxScaleY, SkyboxScaleZ, 1.0f);

	SkyboxTransform = { SkyboxPosition, SkyboxRotation, SkyboxScale };
}

Camera::~Camera()
{
	if (SkyboxModel != nullptr) { delete SkyboxModel; SkyboxModel = nullptr; }
}

void Camera::Update(const double DeltaTime)
{
}

XMMATRIX Camera::GetViewMatrix()
{
	// Calculationg the camera forward and up vectors 
	XMVECTOR forward = GetForwardDirection();
	XMVECTOR up = GetUpDirection();

	// Calculating normalized view matrix using the camera's position, forward, and up vectors 
	return XMMatrixLookToLH({ XMVectorGetX(WorldTransform.Position), XMVectorGetY(WorldTransform.Position), XMVectorGetZ(WorldTransform.Position), 1.0f }, forward, up);
}

XMVECTOR Camera::GetForwardDirection()
{
	// Calculating the forward direction with tilting for both x and z values, and inverting pitch to match the mouse movement
	return XMVector3Normalize(XMVectorSet(sinf(Yaw) * cosf(-Pitch), sinf(-Pitch), cosf(Yaw) * cosf(-Pitch), 1.0f));
}

XMVECTOR Camera::GetRightDirection()
{
	// Calculating the right direction by crossing the world up vector with the forward vector
	XMVECTOR WorldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	return XMVector3Normalize(XMVector3Cross(WorldUp, GetForwardDirection()));
}

XMVECTOR Camera::GetUpDirection()
{
	// Calculating the up direction by crossing the forward vector with the =right vector
	return XMVector3Normalize(XMVector3Cross(GetForwardDirection(), GetRightDirection()));
}

void Camera::SetYaw(const float Yaw)
{
	this->Yaw = Yaw;
}

void Camera::SetPitch(const float Pitch)
{
	this->Pitch = Pitch;
}

float Camera::GetYaw()
{
	return Yaw;
}

float Camera::GetPitch()
{
	return Pitch;
}

ObjFileModel& Camera::GetSkyBoxModel()
{
	return *SkyboxModel;
}

void Camera::SetSkyboxModel(ObjFileModel* SkyboxModel)
{
	this->SkyboxModel = SkyboxModel;
}

Transform& Camera::GetSkyboxTransform()
{
	return SkyboxTransform;
}