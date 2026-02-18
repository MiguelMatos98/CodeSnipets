#pragma once
#include "GameObject.h"
#include "Camera.h"
#include <chrono>
#include <Mouse.h>
#include <Keyboard.h>

using namespace std::chrono_literals;
using namespace std::chrono;

class Player : public GameObject
{
private:
	static constexpr int LEFT_LANE = -1;
	static constexpr int MIDDLE_LANE = 0;
	static constexpr int RIGHT_LANE = 1;
	int Score;
	float ForwardSpeed;
	Camera* MainCamera;
	XMVECTOR CameraOffset;
	bool IsChangingLanes;
	bool IsMovingForward;
	int CurrentLane;
	int TargetLane;
	float LaneWidth;
	float LaneTransitionDuration;
	float ElapsedTimeSinceLaneWasChanged;
	Keyboard::KeyboardStateTracker KeyboardTracker;
	Mouse::ButtonStateTracker MouseTracker;
	XMVECTOR StartPosition;
public:
	Player(const Transform& WorldTransform, string Name = "Default Player", ObjFileModel* Model = nullptr); // Add camera to player here
	~Player();
	int GetScore();
	void SetScore(int Score);
	float GetSpeed();
	void SetSpeed(const float Speed);
	Camera* GetCamera();
	void SetCamera(Camera& Camera);
	void Update(const double DeltaTime) override; // Fucntion pointer to this
	void IncreaseScore(unsigned int IncrementValue);
	void HandleInput(const double DeltaTime);
	void BeginLaneChange(int TargetLane);
	float Smoothstep(float StartX, float EndX, float OutputX);
	float Lerp(float StarX, float EndX, float OutputX);
	float Clamp(float ClampedValue, float Min = 0.0f, float Max = 1.0f);
	void PositionToReset();
	void SetIsMovingForward(bool IsMovingForward);
};