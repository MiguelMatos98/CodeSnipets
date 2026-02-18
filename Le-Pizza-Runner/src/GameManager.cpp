#include "GameManager.h"
#include <fstream>
#include <iostream>
#include "ColliderManager.h"

// Definiting the static members
uint8_t GameManager::PlayerIndex{ 0 };
std::vector<GameObject*> GameManager::GameObjects{};
Renderer* GameManager::GameRenderer{ nullptr };
std::vector<size_t> GameManager::NameIndexes{};
// Indices defining default object grouping thresholds
const std::vector<size_t> GameManager::DefaultNumberForLevelObjects{ 139, 140, 141, 153 };

void GameManager::Initialize(Renderer* renderer)
{
    // Initialize renderer on the heap in case where other external renderer is not provided 
    GameRenderer = renderer ? renderer : new Renderer();
}

void GameManager::Shutdown()
{
    // Cleaning up all game objects by deleting their allocated memory
    for (const GameObject* DestroyableObject : GameObjects)
    {
        delete DestroyableObject;
    }
    GameObjects.clear();
    NameIndexes.clear();

    if (GameRenderer)
    {
        delete GameRenderer;
        GameRenderer = nullptr;
    }
}

void GameManager::LoadGame()
{
    json TransformsFile;

    // Opening the JSON file containing transform data for all game objects
    std::ifstream File("Transforms.json");
    
	// Checking whether the file opened successfully
    if (!File.is_open())
    {
        std::cerr << "Could not open Transforms.json!" << std::endl;
        return;
    }

    // Parsing JSON content into TransformsFile
    File >> TransformsFile;

    // Reserving memory in vectors to improve performance and avoid reallocations
    GameObjects.reserve(TransformsFile.size());
    NameIndexes.reserve(TransformsFile.size());

    ID3D11Device& Device = GameRenderer->GetDevice();
    ID3D11DeviceContext& DeviceContext = GameRenderer->GetDeviceContext();

    float X, Y, Z;
    std::string ObjectType;
    Transform NewWorldTransform;
    size_t NameIndex{ 0 };
    ObjFileModel* NewModel{ nullptr };
    for (size_t CurrentIndex = 0; CurrentIndex < TransformsFile.size(); ++CurrentIndex)
    {
        const json& entry = TransformsFile[CurrentIndex];

		// Manually extract position, rotation, and scale from the JSON entry

        X = entry["position"]["x"];
        Y = entry["position"]["y"];
        Z = entry["position"]["z"];

        NewWorldTransform.Position = XMVectorSet(X, Y, Z, 0.0f);

        X = entry["rotation"]["x"];
        Y = entry["rotation"]["y"];
        Z = entry["rotation"]["z"];

        NewWorldTransform.Rotation = XMVectorSet(X, Y, Z, 1.0f);

        X = entry["scale"]["x"];
        Y = entry["scale"]["y"];
        Z = entry["scale"]["z"];

        NewWorldTransform.Scale = XMVectorSet(X, Y, Z, 1.0f);

		// Setting frist transform as the player transform
        if (CurrentIndex == PlayerIndex)
        {
            ObjectType = "Player";
            
            NameIndex = PlayerIndex;
            
            NameIndexes.push_back(NameIndex);

            // Creating new player object with the extracted transform and a unique name
            Player* NewPlayerObject = new Player(NewWorldTransform, ObjectType + std::to_string(NameIndex));

            // Loading skybox model and assign it to player's camera
            NewModel = new ObjFileModel("PerfectSphere.obj", &Device, &DeviceContext);
            NewPlayerObject->GetCamera()->SetSkyboxModel(NewModel);

            RegisterGameObject(NewPlayerObject);

            ColliderManager::SetColliderAtributes(NewWorldTransform, PlayerIndex);

			// Skiping the rest of the loop for player object 
            continue;
        }

		if (CurrentIndex <= DefaultNumberForLevelObjects[0]) // Pizza object is at index 1 until 139
        {
            ObjectType = "Pizza ";

            NewModel = new ObjFileModel((char*)"Pizza.obj", &Device, &DeviceContext);

			// Reseting name index for proper pizza name indexng (For Potential Lookup)
            NameIndex = (CurrentIndex == 1 ? 0 : NameIndexes.back() + 1);
        }
		else if (CurrentIndex == DefaultNumberForLevelObjects[1]) // Floor object is at index 140
        {
            ObjectType = "Floor ";

            NewModel = new ObjFileModel((char*)"Floor.obj", &Device, &DeviceContext);
            
            // Reseting name index for proper floor name indexng (For Potential Lookup)
            NameIndex = 0;
        }
        else if (CurrentIndex >= DefaultNumberForLevelObjects[2] && CurrentIndex <= DefaultNumberForLevelObjects[3] - 1) // Crate object is at index 141 until 152
        {
            ObjectType = "Crate ";

            NewModel = new ObjFileModel((char*)"Crate.obj", &Device, &DeviceContext);
            
            // Reseting name index for proper crate name indexng (For Potential Lookup)
            NameIndex = (CurrentIndex == DefaultNumberForLevelObjects[2] ? 0 : NameIndexes.back() + 1);
        }
        else // Last existing collider at index 153
        {
            ObjectType = "Collider ";

            NewModel = new ObjFileModel((char*)"cube.obj", &Device, &DeviceContext);

            // Reseting name index for proper crate name indexng (For Potential Lookup)
            NameIndex = NameIndexes.back() + 1;
        }

		// Passing the name index to the container for unique naming
        NameIndexes.push_back(NameIndex);

		// Creating new game object with the extracted transform, a unique name and a model
        GameObject* NewSceneObject = new GameObject(NewWorldTransform, ObjectType + std::to_string(NameIndex), nullptr, NewModel);
        
        RegisterGameObject(NewSceneObject);
        
        // Setting up collider for the object based on their transform and index
        ColliderManager::SetColliderAtributes(NewWorldTransform, CurrentIndex);
    }

    // Initializing scene point lights
    GameRenderer->PointLights[0] = { {2.5f, 1.0f, 0.0f}, 1.0f, {0.0f, 1.0f, 0.0f}, 0.1f, 0.2f, 0.01f, 0.0f, 0.0f };
    GameRenderer->PointLights[1] = { {0.0f, 1.0f, 20.0f}, 1.0f, {0.0f, 0.0f, 1.0f}, 0.1f, 0.2f, 0.01f, 0.0f, 0.0f };
}

void GameManager::PlayGame()
{
    GameRenderer->RenderFrame();
}

void GameManager::RegisterGameObject(GameObject* gameObject)
{
    GameObjects.emplace_back(gameObject);
}

void GameManager::UnregisterGameObject(size_t index)
{
    // Ensuring index is valid before removal
    if (index < GameObjects.size())
    {
        // Erasing game object and it's name index
        GameObjects.erase(GameObjects.begin() + index);
        NameIndexes.erase(NameIndexes.begin() + index);
    }
}

void GameManager::UnregisterAllGameObjects()
{
    // Erasing all game objects and their name indexes
    GameObjects.clear();
    NameIndexes.clear();
}

void GameManager::RestartLevel()
{
    // Tear down existing scene
    ColliderManager::Shutdown();        // clear all collider objects
    UnregisterAllGameObjects();         // deletes all game objects and models

    // Rebuild the scene
    LoadGame();                         // reload transforms & models
    GetRenderer()->InitializeGraphics(); // re-create GPU resources
    GetRenderer()->RenderFrame();       // call update function

    // Allow the player to move again
    GetPlayer().SetIsMovingForward(true);
}

LRESULT GameManager::WindowProc(const HWND window_Handle, const UINT message, const WPARAM w_Param, const LPARAM l_Param)
{
    // Routing input messages to appropriate handlers
    switch (message)
    {
    case WM_DESTROY: 
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        Keyboard::ProcessMessage(message, w_Param, l_Param);
        break;
    case WM_KEYUP:
        Keyboard::ProcessMessage(message, w_Param, l_Param);
        break;
    default:
        Mouse::ProcessMessage(message, w_Param, l_Param);
        break;
    }
    return DefWindowProc(window_Handle, message, w_Param, l_Param);
}

std::vector<GameObject*>& GameManager::GetGameObjects()
{
    return GameObjects;
}

Renderer* GameManager::GetRenderer()
{
    return GameRenderer;
}

Player& GameManager::GetPlayer()
{
	// Performing a dynamic cast from GameObject at index 0 to Player
    return *(dynamic_cast<Player*>(GameObjects[PlayerIndex]));
}

uint8_t GameManager::GetPlayerIndex()
{
    return PlayerIndex;
}

const std::vector<size_t>& GameManager::GetDefaultNumberForLevelObjects()
{
    return DefaultNumberForLevelObjects;
}
