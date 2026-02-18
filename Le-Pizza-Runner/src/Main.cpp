#include "Windows.h"
#include "Transform.h"
#include "GameObject.h"
#include "GameManager.h"
#include "ColliderManager.h"
#include <iostream>
#include <vector>
#include <chrono>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// Declaring time variables
	steady_clock::time_point PreviousTime = steady_clock::now();
	steady_clock::time_point CurrentTime;
	duration<double> DeltaTime;

	// Decleration of the mouse and keyboard objects
	Mouse GameMouse;
	Keyboard keyboard;

	// Declering message to hold windows event messages
	MSG MessageContainer = {};

	// Initialize the core managers and game systems
	GameManager::Initialize();
	ColliderManager::Initialize();

	// Setting up the game window
	GameManager::GetRenderer()->InitializeWindow(hInstance, nCmdShow);

	// Initializing the Direct3D rendering context
	GameManager::GetRenderer()->InitializeD3D();
	
	// Loading game data and assets
	GameManager::LoadGame();     

	// Initialize rendering resources and its properties
	GameManager::GetRenderer()->InitializeGraphics();

	// Setting mouse setting for relative movement and window association
	Mouse::Get().SetWindow(GameManager::GetRenderer()->GetWindowHandler());
	Mouse::Get().SetMode(Mouse::MODE_RELATIVE);

	// Main game loop
	while (MessageContainer.message != WM_QUIT)
	{
		if (PeekMessage(&MessageContainer, NULL, 0, 0, PM_REMOVE))
		{
			// Translating virtual keys into character messages
			TranslateMessage(&MessageContainer);

			// Dispatching the message to the WindowProc function
			DispatchMessage(&MessageContainer);

			// Exiting loop once quit message is received
			if (MessageContainer.message == WM_QUIT)
				break;
		}
		else
		{
			// Calculating time elapsed since the last frame
			CurrentTime = std::chrono::steady_clock::now();
			DeltaTime = CurrentTime - PreviousTime;

			// Performing collision detection
			ColliderManager::CheckForCollision();

			// Processing player input
			GameManager::GetPlayer().HandleInput(DeltaTime.count());
			
			// Updating player position between lanes while moving
			GameManager::GetPlayer().Update(DeltaTime.count());
			
			// Rendering all the scen obkjects, including lights, skybox ad UI
			GameManager::GetRenderer()->RenderFrame();

			// Setting previous time to new current time for next frame calculation
			PreviousTime = CurrentTime;
		}
	}

	return 0;
}