#pragma once
#include <DirectXCollision.h>
#include "SimpleMath.h"
#include "GameObject.h"
#include <vector>

class ColliderManager
{
private:
	static vector<BoundingOrientedBox*> Colliders;
public:
	// Constructor and destructor type methods
	static void Initialize();
	static void Shutdown();
	// Runs a collision event immediately (no lambda retention)
	static void RunCollisionEvent(function<void()> Callback);
	static void CheckForCollision();
	// This method bellow sets all atrbutes for a single collider based on an index
	static void SetColliderAtributes(Transform& WorldTransform, size_t ColliderIndex);
	static vector<BoundingOrientedBox*>& GetColliders();
	// Functions use for setting collider single atributes
	static void SetColliderPosition(const XMVECTOR& NewPosition, size_t ColliderIndex);
	static void SetColliderRotation(const XMVECTOR& NewRotation, size_t ColliderIndex);
	static void SetColliderScale(const XMVECTOR& NewScale, size_t ColliderIndex);
	//void SetColliderToObject(GameObject& object, BoundingOrientedBox* collider = nullptr);
};


