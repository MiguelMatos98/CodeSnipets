#pragma once
#include "Transform.h"
#include "ObjFileModel.h"
#include "GameObject.h"
#include "directxmath.h"

class Camera : public GameObject
{
private:
	float Pitch;
	float Yaw;
	ObjFileModel* SkyboxModel;
	float SkyboxScaleX;
	float SkyboxScaleY;
	float SkyboxScaleZ;
	Transform SkyboxTransform;
public:
	Camera(const XMVECTOR& Position, const XMVECTOR& Rotation);
	~Camera();
	void Update(const double DeltaTime) override;
	ObjFileModel& GetSkyBoxModel();
	void SetSkyboxModel(ObjFileModel* SkyboxModel); // Parameters should be snake_case
	float GetYaw();
	void SetYaw(const float Yaw);
	float GetPitch();
	void SetPitch(const float Pitch);
	Transform& GetSkyboxTransform();
	XMMATRIX GetViewMatrix();
	XMVECTOR GetForwardDirection();
	XMVECTOR GetRightDirection();
	XMVECTOR GetUpDirection();
};

