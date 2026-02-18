#pragma once
#include "Transform.h"
#include "ObjFileModel.h"
#include <DirectXCollision.h>
#include <iostream>
#include "Material.h"

class GameObject
{
protected:
	virtual void Update(const double DeltaTime);
	ObjFileModel* Model;
	Transform WorldTransform;
	Material DefaultMaterial;
	string Name;
public:
	GameObject(const Transform& WorldTransform, string Name = "Default Object", ID3D11ShaderResourceView* Texture = nullptr, ObjFileModel* Model = nullptr);
	GameObject(const XMVECTOR& WorldPosition, const XMVECTOR& WorldRotation, string Name = "Default Object",ID3D11ShaderResourceView* Texture = nullptr, ObjFileModel* Model = nullptr);
	virtual ~GameObject();
	ObjFileModel* GetModel();
	void SetModel(char* ModelName, ID3D11Device& Device, ID3D11DeviceContext& DeviceContext);
	void SetModel(ObjFileModel* Model);
	Transform& GetTransform();
	Material& GetMaterial();
	string GetName();
};