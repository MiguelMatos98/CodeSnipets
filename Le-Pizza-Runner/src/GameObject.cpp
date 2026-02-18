#include "GameObject.h"

GameObject::GameObject(const Transform& WorldTransform, string Name, ID3D11ShaderResourceView* Texture, ObjFileModel* Model)
{
	this->WorldTransform = WorldTransform;
	this->Name = Name;
	this->Model = Model;

	DefaultMaterial.SetMaterialTexture(Texture);
}

GameObject::GameObject(const XMVECTOR& WorldPosition, const XMVECTOR& WorldRotation, string Name, ID3D11ShaderResourceView* Texture, ObjFileModel* Model)
{
	WorldTransform.Position = WorldPosition;
	WorldTransform.Rotation = WorldRotation;
	this->Name = Name;
	this->Model = Model;

	DefaultMaterial.SetMaterialTexture(Texture);
}

GameObject::~GameObject()
{
	if (Model)
	{
		delete Model;
		Model = nullptr;
	}
}

void GameObject::Update(const double delta_Time)
{

}

Transform& GameObject::GetTransform()
{
	return WorldTransform;
}

ObjFileModel* GameObject::GetModel()
{
	return Model;
}

void GameObject::SetModel(char* ModelName, ID3D11Device& Device, ID3D11DeviceContext& DeviceContext)
{
	// Creatng a model object on the heap
	Model = new ObjFileModel(ModelName, &Device, &DeviceContext);
}

void GameObject::SetModel(ObjFileModel* Model)
{
	// Checking for a valid model before assigning it
	if (!Model)
	{
		cout << "Gameobject model has not been seet" << endl;
		return;

	}
	else
		Model = Model;
}

Material& GameObject::GetMaterial()
{
	return DefaultMaterial;
}

string GameObject::GetName()
{
	return Name;
}