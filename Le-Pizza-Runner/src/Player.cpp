#include "Player.h"
#include "ColliderManager.h"

Player::Player(const Transform& WorldTransform, string Name, ObjFileModel* Model) 
	: GameObject(WorldTransform, Name, nullptr, Model),
	  ForwardSpeed{ 3.25f },
	  Score{ 0 }, 
	  MainCamera{ nullptr },
	  IsChangingLanes{ false },
	  IsMovingForward{ false }, 
	  CurrentLane{ MIDDLE_LANE }, 
	  TargetLane{ MIDDLE_LANE }, 
	  LaneWidth{ 2.5f }, 
	  LaneTransitionDuration{ 0.05f },
	  ElapsedTimeSinceLaneWasChanged{ 0.0f }
{
	// Keeping track of the initial position of the player
	StartPosition = WorldTransform.Position;

	// Creating an offset for the camera
	float ForwardOffset = 5.0f;

	// Setting the camera offset based on the player's position
	float Z = XMVectorGetZ(WorldTransform.Position) + ForwardOffset;
	float X = XMVectorGetX(WorldTransform.Position);
	float Y = XMVectorGetY(WorldTransform.Position);

	CameraOffset = { X, Y, Z };

	MainCamera = new Camera(CameraOffset, WorldTransform.Rotation);
}

Player::~Player()
{
	if (MainCamera) 
	{ 
		delete MainCamera;
		MainCamera = nullptr;
    };
}

void Player::Update(const double DeltaTime)
{

	// If the player is not moving forward already then it will start moving
	if (!IsMovingForward)
		IsMovingForward = true;

	// Once player as started to move forward 
	if (IsMovingForward)
	{
		// Moving player forward on the z-axis
		WorldTransform.Position += { 0.0f, 0.0f, ForwardSpeed * float(DeltaTime), 0.0f};

		// Hadnling lane transition when player is changing lanes
		if (IsChangingLanes)
		{
			// Incrementing elapsed time for lane transition
			ElapsedTimeSinceLaneWasChanged += float(DeltaTime);

			// Calculating the smooth interpolation factor from 0 to 1 using smoothstep
			float SmoothFactor = Smoothstep(0.0f, LaneTransitionDuration, ElapsedTimeSinceLaneWasChanged);

			// Calculating the X position to snap the player to the target lane
			float SnappingX = TargetLane * LaneWidth;

			// Getting the current X position of the player
			float CurrentX = XMVectorGetX(WorldTransform.Position);

			// Calculating the player's new X position from currentX to the Target X,
			// using the smooth interpolation factor 
			float NewX = Lerp(CurrentX, SnappingX, SmoothFactor);

			// Updating player position for this cxurrent frame with the new interpolated X
			WorldTransform.Position = { NewX, XMVectorGetY(WorldTransform.Position), XMVectorGetZ(WorldTransform.Position), 0.0f };

			// When elapsed time reaches duration, the lane change is complete
			// and the player can be snapped to the target lane
			if (ElapsedTimeSinceLaneWasChanged >= LaneTransitionDuration)
			{
				// Resetting the lane change state to default
				IsChangingLanes = false;

				// Finalizing player movement by snaping it to the target lane
				WorldTransform.Position = { SnappingX, XMVectorGetY(WorldTransform.Position), XMVectorGetZ(WorldTransform.Position), 0.0f };

				// Setting previous target as new current lane
				CurrentLane = TargetLane;
			}
		}


		// Recalculating the camera offset based on the new player's position
		CameraOffset = { XMVectorGetX(WorldTransform.Position), XMVectorGetY(WorldTransform.Position), XMVectorGetZ(WorldTransform.Position) + 5.0f };

		MainCamera->GetTransform().Position = { CameraOffset };

		// Updating the collider position based on the new camera position
		ColliderManager::SetColliderPosition(CameraOffset, 0);
	}
	else
	{
		// If the player is not moving forward anymore, then reset player position 
		PositionToReset();
	}
}

void Player::SetSpeed(const float Speed)
{
	ForwardSpeed = Speed;
}

void Player::SetCamera(Camera& Camera)
{
	MainCamera = &Camera;
}

void Player::SetScore(int Score)
{
	this->Score = Score;
}

void Player::IncreaseScore(unsigned int IncrementValue)
{
	Score += IncrementValue;
}

void Player::HandleInput(const double DeltaTime)
{
	// Getting current keyboard and mouse states
	Keyboard::State KeyboardState = Keyboard::Get().GetState();
	Mouse::State MouseState = Mouse::Get().GetState();

	// Updating the keyboard and mouse trackers
	KeyboardTracker.Update(KeyboardState);
	MouseTracker.Update(MouseState);

	// Creating a sensitivity value for mouse movement
	float sensitivity = XM_2PI * 0.05f;

	float Yaw = MainCamera->GetYaw(), Pitch = MainCamera->GetPitch(); // Player

	// Setting the yaw and pitch for free camera rotation
	MainCamera->SetYaw(Yaw - MouseState.x * sensitivity * float(DeltaTime));
	MainCamera->SetPitch(Pitch + MouseState.y * sensitivity * float(DeltaTime)); 

	// Checking if the player is pressing A or D
	if (KeyboardTracker.pressed.A)
		BeginLaneChange(CurrentLane + 1);

	if (KeyboardTracker.pressed.D)
		BeginLaneChange(CurrentLane - 1);
}

void Player::BeginLaneChange(int TargetLane)
{
	// return early when target lane is not valid
	if (TargetLane < LEFT_LANE || TargetLane > RIGHT_LANE)
	{
		return;
	}

	// Marking that a lane change is in progress
	IsChangingLanes = true;

	// Setting the new target lane for the player to move towards
	this->TargetLane = TargetLane;

	// Resetting the lane change timer so that interpolation from zero
	ElapsedTimeSinceLaneWasChanged = 0.0f;
}

int Player::GetScore()
{
	return Score;
}

Camera* Player::GetCamera()
{
	return MainCamera;
}

float Player::Smoothstep(float StartX, float EndX, float OutputX)
{
	// Normalize OutputX as a 0 to 1 range and clamp it to avoid overshooting
	OutputX = Clamp((OutputX - StartX) / (EndX - StartX));

	// This will return a smoth easing between the start and end values
	// with a slow start and end
	return OutputX * OutputX * (3.0f - 2.0f * OutputX);
}

float Player::Clamp(float ClampedValue, float Min, float Max)
{
	// Making sure the value is within the Min and Max range
	if (ClampedValue < Min) return Min;
	if (ClampedValue > Max) return Max;

	// If the value is within the range above, retuninit as is
	return ClampedValue;
}

void Player::PositionToReset()
{
	WorldTransform.Position = StartPosition;
}

void Player::SetIsMovingForward(bool IsMovingForward)
{
	this->IsMovingForward = IsMovingForward;
}

float Player::Lerp(float StarX, float EndX, float OutputX)
{
	// // Returning the blending between StartX and EndX based on the OutputX (Smooth Factor)
	return StarX * (1.0f - OutputX) + (EndX * OutputX);
}

float Player::GetSpeed()
{
	return ForwardSpeed;
}
