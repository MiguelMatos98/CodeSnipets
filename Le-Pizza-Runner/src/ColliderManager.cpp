#include "ColliderManager.h"
#include "GameManager.h"
#include <string>

vector<BoundingOrientedBox*> ColliderManager::Colliders{};

void ColliderManager::Initialize()
{
}

void ColliderManager::Shutdown()
{
	for (size_t i = 0; i < Colliders.size(); i++)
	{
		delete Colliders[i];
		Colliders[i] = nullptr;
	}
	Colliders.clear();
}

void ColliderManager::RunCollisionEvent(function<void()> Callback)
{
	if (Callback)
		Callback();
}

void ColliderManager::CheckForCollision()
{
    size_t playerIndex = GameManager::GetPlayerIndex();

    // Start from 1 to skip the player's own collider at index 0
    for (size_t ColliderID = 1; ColliderID < Colliders.size(); ++ColliderID)
    {
        if (Colliders[playerIndex]->Intersects(*Colliders[ColliderID]))
        {
            string name = GameManager::GetGameObjects()[ColliderID]->GetName();

            // ——— Pizza pickup ———
            if (strncmp(name.c_str(), "Pizza", 5) == 0)
            {
                // Remove that game object and its collider
                GameManager::UnregisterGameObject(ColliderID);
                delete Colliders[ColliderID];
                Colliders.erase(Colliders.begin() + ColliderID);

                // Award score and pause forward movement
                GameManager::GetPlayer().IncreaseScore(1);
            }
            // ——— Crate or collider hit ———
            else if (strncmp(name.c_str(), "Crate", 5) == 0 || strncmp(name.c_str(), "Collider", 8) == 0)
            {

                // Completely restart the level when playeer hits a crate or the last collider
                GameManager::RestartLevel();
            }

            break; 
		}
	}
}

void ColliderManager::SetColliderAtributes(Transform& WorldTransform, size_t ColliderIndex)
{
	size_t CurrentColliderIndex = ColliderIndex;

	// Get rotation axis from transform
	XMVECTOR Yaw = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorGetY(WorldTransform.Rotation));
	XMVECTOR Pitch = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMVectorGetX(WorldTransform.Rotation));
	XMVECTOR Roll = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), XMVectorGetZ(WorldTransform.Rotation));

	XMVECTOR combinedQuat = XMQuaternionMultiply(Yaw, XMQuaternionMultiply(Pitch, Roll));

	// Checking for no repetive indexes 
	if (CurrentColliderIndex >= 0 && CurrentColliderIndex <= Colliders.size())
		Colliders.push_back(new BoundingOrientedBox);

	SetColliderPosition(WorldTransform.Position, CurrentColliderIndex);
	SetColliderRotation(XMQuaternionNormalize(combinedQuat), CurrentColliderIndex);
	SetColliderScale(WorldTransform.Scale, CurrentColliderIndex);
}

vector<BoundingOrientedBox*>& ColliderManager::GetColliders()
{
	return Colliders;
}

void ColliderManager::SetColliderPosition(const XMVECTOR& NewPosition, size_t ColliderIndex)
{
	XMStoreFloat3(&Colliders[ColliderIndex]->Center, NewPosition);
}

void ColliderManager::SetColliderRotation(const XMVECTOR& NewRotation, size_t ColliderIndex)
{
	XMStoreFloat4(&Colliders[ColliderIndex]->Orientation, NewRotation);
}

void ColliderManager::SetColliderScale(const XMVECTOR& NewScale, size_t ColliderIndex)
{
	XMStoreFloat3(&Colliders[ColliderIndex]->Extents, NewScale);
}