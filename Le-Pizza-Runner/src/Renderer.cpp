#include "Renderer.h"
#include <Windows.h>
#include <DDSTextureLoader.h>
#include <dxgidebug.h>
#include "WICTextureLoader.h"
#include "GameManager.h"
#include "ColliderManager.h"
#include "Material.h"
#include <sstream>

const UINT Renderer::MaxPixelShaderResourceSlots = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;

Renderer::Renderer()
	: WindowHeaderTitle{ nullptr },
	  AmbientLightColour{ 1.0f, 1.0f, 1.0f, 1.0f },
	  DirectionalLightDirection{ 0.0f, 1.0f, 0.0f },
	  DirectionaLightColour{ 1.0f, 1.0f, 1.0f, 1.0f },
	  IconPosition{ 0.0f, 0.0f },
	  FontPosition{ 0.0f, 0.0f },
	  WindowHandle{ nullptr }, 
	  D3DDevice{ nullptr }, 
	  D3DDeviceContext{ nullptr }, 
	  VertexBuffer{ nullptr }, 
	  IndexBuffer{ nullptr }, 
	  ObjectTransformConstantBuffer{ nullptr }, 
	  MaterialLightingConstantBuffer{ nullptr }, 
	  SkyboxTransformConstantBuffer{ nullptr }, 
	  TiledWrapSampler{ nullptr },
	  ClampEdgeSampler{nullptr},
	  AbsorbentRasterizerState{ nullptr }, 
	  ReflectiveRasterizerState{ nullptr }, 
	  AbsorbentDepthStencilState{ nullptr }, 
	  ReflectiveDepthStencilState{ nullptr },  
	  SkyboxVertexShader{ nullptr }, 
	  SkyboxPixelShader{ nullptr }, 
	  SkyboxInputLayout{ nullptr },
	  VertexShader{ nullptr }, 
	  PixelShader{ nullptr },
	  InputLayout{ nullptr }, 
	  SwapChain{ nullptr }, 
	  RenderTargetViewBackBuffer{ nullptr }, 
	  DepthBufferStencilView{ nullptr }, 
	  AlphaBlendStateEnabled{ nullptr }, 
	  AlphaBlendStateDisabled{ nullptr }, 
	  VertexShaderReflection{ nullptr },
	  ObjectTransformData{},
	  MaterialLightingData{},
	  SpriteBatch{nullptr},
	  SpriteFont{ nullptr },
	  IconTexture{ nullptr }
{
}

Renderer::~Renderer()
{
	// If ShutdownGraphics wasn't already called, call it now
	ShutdownGraphics();

	// Other cleanup
	VertexShaderReflection.Reset();
	D3DDevice.Reset();
	D3DDeviceContext.Reset();

	WindowHandle = nullptr;
	WindowHeaderTitle = nullptr;
}

void Renderer::InitializeGraphics()
{
	HRESULT HResult;

	// Creating a linear wrap-mode sampler (For tiled textures)
	D3D11_SAMPLER_DESC WrapSamplerDescription{};
	WrapSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	WrapSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	WrapSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	WrapSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	WrapSamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
	WrapSamplerDescription.MinLOD = 0;
	WrapSamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	HResult = D3DDevice->CreateSamplerState(&WrapSamplerDescription, &TiledWrapSampler);
	if (FAILED(HResult))
	{
		OutputDebugString(L"Failed to create TextureSampler\n");
		return;
	}

	ID3D11ShaderResourceView* NormalMapView = nullptr;
	HResult = CreateWICTextureFromFile(D3DDevice.Get(), D3DDeviceContext.Get(), L"NormalMap.png", nullptr, &NormalMapView);
	if (FAILED(HResult))
	{
		OutputDebugString(L"Could not load NormalMap - this is OK if not using normal mapping\n");
		NormalMapView = nullptr;
	}

	// Cube vertices assigned with position, color, uv, normal, tangent and bitangent
	static const Vertex Vertices[] = {
		// Front vertices
		{{-0.5f,-0.5f,-0.5f},{1,1,1,1},{0,0},{ 0,0,-1},{},{}},
		{{ 0.5f,-0.5f,-0.5f},{1,1,1,1},{1,0},{ 0,0,-1},{},{}},
		{{-0.5f, 0.5f,-0.5f},{1,1,1,1},{0,1},{ 0,0,-1},{},{}},
		{{ 0.5f, 0.5f,-0.5f},{1,1,1,1},{1,1},{ 0,0,-1},{},{}},
		// Back vertices
		{{ 0.5f,-0.5f, 0.5f},{1,1,1,1},{0,0},{ 0,0, 1},{},{}},
		{{-0.5f,-0.5f, 0.5f},{1,1,1,1},{1,0},{ 0,0, 1},{},{}},
		{{ 0.5f, 0.5f, 0.5f},{1,1,1,1},{0,1},{ 0,0, 1},{},{}},
		{{-0.5f, 0.5f, 0.5f},{1,1,1,1},{1,1},{ 0,0, 1},{},{}},
		// Left vertices
		{{-0.5f,-0.5f, 0.5f},{1,1,1,1},{0,0},{-1,0, 0},{},{}},
		{{-0.5f,-0.5f,-0.5f},{1,1,1,1},{1,0},{-1,0, 0},{},{}},
		{{-0.5f, 0.5f, 0.5f},{1,1,1,1},{0,1},{-1,0, 0},{},{}},
		{{-0.5f, 0.5f,-0.5f},{1,1,1,1},{1,1},{-1,0, 0},{},{}},
		// Right vertices
		{{ 0.5f,-0.5f,-0.5f},{1,1,1,1},{0,0},{ 1,0, 0},{},{}},
		{{ 0.5f,-0.5f, 0.5f},{1,1,1,1},{1,0},{ 1,0, 0},{},{}},
		{{ 0.5f, 0.5f,-0.5f},{1,1,1,1},{0,1},{ 1,0, 0},{},{}},
		{{ 0.5f, 0.5f, 0.5f},{1,1,1,1},{1,1},{ 1,0, 0},{},{}},
		// Top vertices
		{{-0.5f, 0.5f, 0.5f},{1,1,1,1},{0,0},{ 0,1, 0},{},{}},
		{{ 0.5f, 0.5f, 0.5f},{1,1,1,1},{1,0},{ 0,1, 0},{},{}},
		{{-0.5f, 0.5f,-0.5f},{1,1,1,1},{0,1},{ 0,1, 0},{},{}},
		{{ 0.5f, 0.5f,-0.5f},{1,1,1,1},{1,1},{ 0,1, 0},{},{}},
		// Bottom vertices
		{{-0.5f,-0.5f, 0.5f},{1,1,1,1},{0,0},{ 0,-1,0},{},{}},
		{{ 0.5f,-0.5f, 0.5f},{1,1,1,1},{1,0},{ 0,-1,0},{},{}},
		{{-0.5f,-0.5f,-0.5f},{1,1,1,1},{0,1},{ 0,-1,0},{},{}},
		{{ 0.5f,-0.5f,-0.5f},{1,1,1,1},{1,1},{ 0,-1,0},{},{}},
	};
	const UINT VerticesCount = _countof(Vertices);

	static const UINT Indices[] =
	{
		0,1,2, 2,1,3, 4,5,6, 6,5,7,
		8,9,10, 10,9,11, 12,13,14, 14,13,15,
		16,17,18, 18,17,19, 20,21,22, 22,21,23
	};
	const UINT IndicesCount = _countof(Indices);

	// Immutable vertex buffer
	D3D11_BUFFER_DESC VertexBufferDescription{};
	VertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	VertexBufferDescription.ByteWidth = sizeof(Vertices);
	VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA VertexData{ Vertices };
	HResult = D3DDevice->CreateBuffer(&VertexBufferDescription, &VertexData, &VertexBuffer);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create VertexBuffer\n"); return; }

	// Immutable index buffer
	D3D11_BUFFER_DESC IndexBufferDescritpion{};
	IndexBufferDescritpion.Usage = D3D11_USAGE_IMMUTABLE;
	IndexBufferDescritpion.ByteWidth = sizeof(Indices);
	IndexBufferDescritpion.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA IndexBufferInitialData{ Indices };
	HResult = D3DDevice->CreateBuffer(&IndexBufferDescritpion, &IndexBufferInitialData, &IndexBuffer);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create IndexBuffer\n"); return; }

	// Constant buffers
	D3D11_BUFFER_DESC ConstantBufferDescritpion{};
	ConstantBufferDescritpion.Usage = D3D11_USAGE_DEFAULT;
	ConstantBufferDescritpion.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDescritpion.CPUAccessFlags = 0;

	ConstantBufferDescritpion.ByteWidth = (sizeof(TransformBuffer) + 15) & ~15u;
	HResult = D3DDevice->CreateBuffer(&ConstantBufferDescritpion, nullptr, &ObjectTransformConstantBuffer);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create ObjectTransformConstantBuffer\n"); return; }

	ConstantBufferDescritpion.ByteWidth = (sizeof(MaterialLightingBuffer) + 15) & ~15u;
	HResult = D3DDevice->CreateBuffer(&ConstantBufferDescritpion, nullptr, &MaterialLightingConstantBuffer);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create MaterialLightingConstantBuffer\n"); return; }

	ConstantBufferDescritpion.ByteWidth = (sizeof(SkyboxTransformBuffer) + 15) & ~15u;
	HResult = D3DDevice->CreateBuffer(&ConstantBufferDescritpion, nullptr, &SkyboxTransformConstantBuffer);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create SkyboxTransformConstantBuffer\n"); return; }

	vector<GameObject*>& SceneObjects = GameManager::GetGameObjects();
	for (size_t SceneObjectIndex = 0; SceneObjectIndex < SceneObjects.size(); ++SceneObjectIndex)
	{
		const wchar_t* TextureName = nullptr;

		if (SceneObjectIndex < 140) TextureName = L"PizzaBox.png";
		else if (SceneObjectIndex == 140) TextureName = L"roadTexture.png";
		else if (SceneObjectIndex >= 141 && SceneObjectIndex <= 152) TextureName = L"TrollyTexture.png";

		ID3D11ShaderResourceView* DiffuseMapView = nullptr; 

		if (TextureName != nullptr)
		{
			HResult = CreateWICTextureFromFile(D3DDevice.Get(), D3DDeviceContext.Get(), TextureName, nullptr, &DiffuseMapView);
			if (SUCCEEDED(HResult) && DiffuseMapView)
			{
				D3DDeviceContext->GenerateMips(DiffuseMapView);
			}
			else
			{
				DiffuseMapView = nullptr;
			}
		}

		Material& SceneObjectMaterial = SceneObjects[SceneObjectIndex]->GetMaterial();
		SceneObjectMaterial.SetMaterialTexture(DiffuseMapView); 
		SceneObjectMaterial.SetNormalMap(NormalMapView);
		SceneObjectMaterial.SetMaterialSamplerState(TiledWrapSampler.Get());
	}


	if (GameObject* Camera = dynamic_cast<GameObject*>(GameManager::GetPlayer().GetCamera()))
	{
		ID3D11Resource* SkyboxResource = nullptr;
		ID3D11ShaderResourceView* SkyboxMapView = nullptr;

		// Loading skybox texture
		HResult = CreateWICTextureFromFile(
			D3DDevice.Get(),
			D3DDeviceContext.Get(),
			L"NightSky.jpg",
			&SkyboxResource,
			&SkyboxMapView
		);

		if (FAILED(HResult))
		{
			OutputDebugString(L"Failed to load skybox texture NightSky.jpg\n");
			return;
		}

		if (SkyboxResource && SkyboxMapView)
		{
			D3DDeviceContext->GenerateMips(SkyboxMapView);
			SkyboxResource->Release();
		}

		// Wrapping horizontally (U) and clamping vertically (V) for equirectangular textures
		D3D11_SAMPLER_DESC SkyboxSamplerDesc{};
		SkyboxSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		SkyboxSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // Wrap around longitude
		SkyboxSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;  // Clamp at poles
		SkyboxSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SkyboxSamplerDesc.MaxAnisotropy = 16;
		SkyboxSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SkyboxSamplerDesc.MinLOD = 0;
		SkyboxSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HResult = D3DDevice->CreateSamplerState(&SkyboxSamplerDesc, &ClampEdgeSampler);
		if (FAILED(HResult))
		{
			OutputDebugString(L"Failed to create SkyboxSampler\n");
			return;
		}

		// Set the skybox texture and sampler to the camera's material
		Camera->GetMaterial().SetMaterialTexture(SkyboxMapView);
		Camera->GetMaterial().SetMaterialSamplerState(ClampEdgeSampler.Get());
	}
	else
	{
		OutputDebugString(L"Failed to get camera for skybox setup\n");
	}

	// Sprite batch and font setup
	SpriteBatch = std::make_unique<DX11::SpriteBatch>(D3DDeviceContext.Get());
	SpriteFont = std::make_unique<DX11::SpriteFont>(D3DDevice.Get(), L"myfile.spritefont");

	HResult = CreateWICTextureFromFile(D3DDevice.Get(), D3DDeviceContext.Get(), L"PizzaBoxUI.png", nullptr, &IconTexture);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to load icon texture!\n"); return; }

	IconPosition = { 10.0f, 20.0f };
	FontPosition = { 120.0f, 40.0f };
}


HRESULT Renderer::InitializeWindow(const HINSTANCE InstanceHandle, const int CmdShow)
{
	// Defining and registing the main window with it's propetes	
	WNDCLASSEX Window = {}; 
	Window.cbSize = sizeof(WNDCLASSEX); 	// Size of this structure
	Window.style = CS_HREDRAW | CS_VREDRAW; // Redrawing window on horizontal or vertical resize
	Window.lpfnWndProc = GameManager::WindowProc; 	// Setting up the window procedure function
	Window.hInstance = InstanceHandle; // Setting the window's handle instance
	Window.lpszClassName = L"MainWindow"; // Setting window's Name
	Window.hbrBackground = (HBRUSH)COLOR_WINDOW;	// Setting background color

	// Checking if windows as been registered properly 
	if (!RegisterClassEx(&Window))
	{
		return E_FAIL; 
	}

	// Adjusting the window dimensions with the title header so no pixels are taken away from our app
	RECT WindowTitleRectangle = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };

	// Geting the size of window and ajusting the tile header rectangle
	GetClientRect(WindowHandle, &WindowTitleRectangle); 
	AdjustWindowRect(&WindowTitleRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	// Getting the window title header dimentions 
	UINT width = WindowTitleRectangle.right - WindowTitleRectangle.left;
	UINT height = WindowTitleRectangle.bottom - WindowTitleRectangle.top;

	WindowHeaderTitle = L"Le-Pizza-Runner!";

	// Setting up the window handle for manipulation of its properties
	WindowHandle = CreateWindowEx(NULL,
				   Window.lpszClassName, // Name of the window
				   WindowHeaderTitle, // Window Title Header
				   WS_OVERLAPPED | WS_MINIMIZE | WS_SYSMENU, // Window style with no resizing and maxamizing 
				   100, // x-position of the window
			       100, // Y-position of the window
				   width, // Width of the window
				   height, // Height of the window
		           NULL, // No parent window
				   NULL, // No menus
				   InstanceHandle, // Instance handle 
				   NULL); // Not used with multiple windoww

	if (WindowHandle == NULL) return E_FAIL;

	ShowWindow(WindowHandle, CmdShow);

	return S_OK;
}

HRESULT Renderer::InitializeD3D()
{
	HRESULT HResult;

	// Creating Swap chain description
	DXGI_SWAP_CHAIN_DESC SwapChainDescription = {};
	SwapChainDescription.BufferCount = 2; // double buffering
	SwapChainDescription.BufferDesc.Width = SCREEN_WIDTH;
	SwapChainDescription.BufferDesc.Height = SCREEN_HEIGHT;
	SwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit color
	SwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescription.OutputWindow = WindowHandle;
	SwapChainDescription.SampleDesc.Count = 1; // no MSAA
	SwapChainDescription.SampleDesc.Quality = 0;
	SwapChainDescription.Windowed = TRUE;
	SwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDescription.Flags = 0;

	// Creating the device and swap chain
	HResult = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&SwapChainDescription,
		&SwapChain,
		&D3DDevice,
		NULL,
		&D3DDeviceContext
	);
	if (FAILED(HResult)) return HResult;

	
	// Creating Back buffer render target
	ID3D11Texture2D* BackBufferTexture = nullptr;
	HResult = SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBufferTexture));
	if (FAILED(HResult)) return HResult;

	HResult = D3DDevice->CreateRenderTargetView(BackBufferTexture, NULL, &RenderTargetViewBackBuffer);
	BackBufferTexture->Release();
	if (FAILED(HResult)) return HResult;

	// Creating Depth buffer and view
	ID3D11Texture2D* DepthBufferTexture = nullptr;
	D3D11_TEXTURE2D_DESC Texture2DDescription = {};
	Texture2DDescription.Width = SCREEN_WIDTH;
	Texture2DDescription.Height = SCREEN_HEIGHT;
	Texture2DDescription.MipLevels = 1;
	Texture2DDescription.ArraySize = 1;
	Texture2DDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Texture2DDescription.SampleDesc.Count = 1;
	Texture2DDescription.SampleDesc.Quality = 0;
	Texture2DDescription.Usage = D3D11_USAGE_DEFAULT;
	Texture2DDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	HResult = D3DDevice->CreateTexture2D(&Texture2DDescription, NULL, &DepthBufferTexture);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create depth buffer 2D texture!"); return HResult; }

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDescription = {};
	DepthStencilViewDescription.Format = Texture2DDescription.Format;
	DepthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HResult = D3DDevice->CreateDepthStencilView(DepthBufferTexture, &DepthStencilViewDescription, &DepthBufferStencilView);
	DepthBufferTexture->Release();
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create depth buffer stencil view!"); return HResult; }

	// Setting Sampler state
	D3D11_SAMPLER_DESC SamplerDescription = {};
	SamplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	SamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerDescription.MipLODBias = 0.0f;
	SamplerDescription.MaxAnisotropy = 16;
	SamplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDescription.MinLOD = 0.0f;
	SamplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	HResult = D3DDevice->CreateSamplerState(&SamplerDescription, &TiledWrapSampler);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create sampler state!"); return HResult; }

	// Setting Blend states
	D3D11_BLEND_DESC BlendModeDescription = {};
	BlendModeDescription.RenderTarget[0].BlendEnable = TRUE;
	BlendModeDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendModeDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendModeDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendModeDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendModeDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendModeDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendModeDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HResult = D3DDevice->CreateBlendState(&BlendModeDescription, &AlphaBlendStateEnabled);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create blend state for pixel transparency!"); return HResult; }

	BlendModeDescription.RenderTarget[0].BlendEnable = FALSE;
	HResult = D3DDevice->CreateBlendState(&BlendModeDescription, &AlphaBlendStateDisabled);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create blend state for pixel opacity!"); return HResult; }

	// Setting Rasterizer states
	D3D11_RASTERIZER_DESC RasterizerDescription = {};
	RasterizerDescription.CullMode = D3D11_CULL_BACK;
	RasterizerDescription.FillMode = D3D11_FILL_SOLID;
	RasterizerDescription.FrontCounterClockwise = TRUE;
	RasterizerDescription.DepthClipEnable = TRUE;

	HResult = D3DDevice->CreateRasterizerState(&RasterizerDescription, &AbsorbentRasterizerState);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create solid rasterizer state!"); return HResult; }

	RasterizerDescription.CullMode = D3D11_CULL_FRONT; 
	RasterizerDescription.FrontCounterClockwise = TRUE;
	HResult = D3DDevice->CreateRasterizerState(&RasterizerDescription, &ReflectiveRasterizerState);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create reflective rasterizer state!"); return HResult; }

	// Defining the Depth-stencil states
	D3D11_DEPTH_STENCIL_DESC DepthStencilDescription = {};
	DepthStencilDescription.DepthEnable = TRUE;
	DepthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDescription.StencilEnable = TRUE;
	DepthStencilDescription.StencilReadMask = 0xFF;
	DepthStencilDescription.StencilWriteMask = 0xFF;
	DepthStencilDescription.FrontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
	DepthStencilDescription.BackFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_DECR, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };

	HResult = D3DDevice->CreateDepthStencilState(&DepthStencilDescription, &AbsorbentDepthStencilState);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create depth stencil state for absorbent objects!"); return HResult; }

	DepthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // skybox won't write
	DepthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;     // skybox passes at far plane
	HResult = D3DDevice->CreateDepthStencilState(&DepthStencilDescription, &ReflectiveDepthStencilState);
	if (FAILED(HResult)) { OutputDebugString(L"Failed to create depth stencil state for reflective objects!"); return HResult; }

	// Setting the Viewport
	D3D11_VIEWPORT Viewport = {};
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = FLOAT(SCREEN_WIDTH);
	Viewport.Height = FLOAT(SCREEN_HEIGHT);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	D3DDeviceContext->RSSetViewports(1, &Viewport);

	// Bind render targets for back buffer and depth buffer
	D3DDeviceContext->OMSetRenderTargets(
		1,
		RenderTargetViewBackBuffer.GetAddressOf(),
		DepthBufferStencilView.Get()
	);

	InitializePipeline();

	return S_OK;
}

HRESULT Renderer::LoadPixelShader(const wchar_t* const FileName, ID3D11PixelShader** PixelShader)
{

	std::vector<uint8_t> pixelShadersBytecode = DX::ReadData(FileName);

	D3DDevice->CreatePixelShader(pixelShadersBytecode.data(), pixelShadersBytecode.size(), NULL, PixelShader);
	
	return S_OK;
}

HRESULT Renderer::LoadVertexShader(const wchar_t* const FileName, ID3D11VertexShader** VertexShader, ID3D11InputLayout** InputLayout)
{
	HRESULT HResult;

	std::vector<uint8_t> vertexShaderBytecode = DX::ReadData(FileName);

	HResult = D3DDevice->CreateVertexShader(
		vertexShaderBytecode.data(),
		vertexShaderBytecode.size(),
		nullptr,
		VertexShader
	);

	// Creating input layout to match all the shader and CPU semantics
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0,	offsetof(Vertex, Position),		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	offsetof(Vertex, Color),		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0,	offsetof(Vertex, UV),			D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	offsetof(Vertex, Normal),		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	offsetof(Vertex, Tangent),		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,	offsetof(Vertex, Bitangent),	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HResult = D3DDevice->CreateInputLayout(
		layout,
		_countof(layout),
		vertexShaderBytecode.data(),
		vertexShaderBytecode.size(),
		InputLayout
	);

	return S_OK;
}

HRESULT Renderer::InitializePipeline()
{
	LoadVertexShader(L"Compiled Shaders/SkyboxVShader.cso", &SkyboxVertexShader, &SkyboxInputLayout);
	LoadPixelShader(L"Compiled Shaders/SkyboxPShader.cso", &SkyboxPixelShader);

	LoadVertexShader(L"Compiled Shaders/RefletiveVShader.cso", &VertexShader, &InputLayout);
	LoadPixelShader(L"Compiled Shaders/RefletivePShader.cso", &PixelShader);

	return S_OK;
}

void Renderer::DrawSkyBox(Transform& Transform, XMMATRIX& View, XMMATRIX& Projection)
{
	// Set skybox-specific render states
	D3DDeviceContext->OMSetDepthStencilState(ReflectiveDepthStencilState.Get(), 0);
	D3DDeviceContext->RSSetState(ReflectiveRasterizerState.Get());
	D3DDeviceContext->OMSetBlendState(AlphaBlendStateDisabled.Get(), nullptr, 0xFFFFFFFF);

	// Set skybox shaders
	D3DDeviceContext->VSSetShader(SkyboxVertexShader.Get(), nullptr, 0);
	D3DDeviceContext->PSSetShader(SkyboxPixelShader.Get(), nullptr, 0);
	D3DDeviceContext->IASetInputLayout(SkyboxInputLayout.Get());

	// Get and bind skybox texture
	ID3D11ShaderResourceView* SkyTexture = GameManager::GetPlayer().GetCamera()->GetMaterial().GetMaterialTexture();
	D3DDeviceContext->PSSetShaderResources(0, 1, &SkyTexture);

	// Bind skybox sampler (wrap U and clamp V for equirectangular)
	ID3D11SamplerState* Sampler = ClampEdgeSampler.Get();
	D3DDeviceContext->PSSetSamplers(0, 1, &Sampler);

	// Remove translation from view matrix to keep skybox centered on camera
	XMMATRIX SkyboxView = View;
	SkyboxView.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	// Scale skybox large enough to encompass the scene
	XMMATRIX World = XMMatrixScaling(500.0f, 500.0f, 500.0f);

	// Compute world-view-projection matrix
	SkyboxTransformData.WVP = XMMatrixTranspose(World * SkyboxView * Projection);

	// Update and bind constant buffer
	D3DDeviceContext->UpdateSubresource(SkyboxTransformConstantBuffer.Get(), 0, nullptr, &SkyboxTransformData, 0, 0);
	D3DDeviceContext->VSSetConstantBuffers(0, 1, SkyboxTransformConstantBuffer.GetAddressOf());

	// Get skybox model and draw
	ObjFileModel& SkyboxModel = GameManager::GetPlayer().GetCamera()->GetSkyBoxModel();
	SkyboxModel.Draw();

	// Restore previous render states
	D3DDeviceContext->OMSetDepthStencilState(AbsorbentDepthStencilState.Get(), 1);
	D3DDeviceContext->RSSetState(AbsorbentRasterizerState.Get());
	D3DDeviceContext->OMSetBlendState(AlphaBlendStateDisabled.Get(), nullptr, 0xFFFFFFFF);
}

void Renderer::RenderFrame()
{
	Camera* const Camera = GameManager::GetPlayer().GetCamera();

	XMMATRIX View = Camera->GetViewMatrix();
	XMMATRIX Projection = XMMatrixPerspectiveFovRH(XMConvertToRadians(60.0f), float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.1f, 150.0f);

	// Clearing the back buffer (Render Target) and depth stencil view
	D3DDeviceContext->ClearRenderTargetView(RenderTargetViewBackBuffer.Get(), Colors::Black);
	D3DDeviceContext->ClearDepthStencilView(DepthBufferStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	D3DDeviceContext->OMSetRenderTargets(1, RenderTargetViewBackBuffer.GetAddressOf(), DepthBufferStencilView.Get());
	D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DrawSkyBox(Camera->GetSkyboxTransform(), View, Projection);

	// Preparing the constant buffers for absorbent rendering
	D3DDeviceContext->IASetInputLayout(InputLayout.Get());
	D3DDeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
	D3DDeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
	D3DDeviceContext->RSSetState(AbsorbentRasterizerState.Get());
	D3DDeviceContext->OMSetBlendState(AlphaBlendStateDisabled.Get(), nullptr, 0xFFFFFFFF);
	D3DDeviceContext->OMSetDepthStencilState(AbsorbentDepthStencilState.Get(), 1);


	D3DDeviceContext->PSSetSamplers(0, 1, TiledWrapSampler.GetAddressOf());

	std::vector<GameObject*>& SceneObject = GameManager::GetGameObjects();
	for (size_t ObjectIndex = 1; ObjectIndex < GameManager::GetGameObjects().size(); ++ObjectIndex)
	{
		Material& Material = SceneObject[ObjectIndex]->GetMaterial();

		// Setting diffuse material texture to slot 1
		ID3D11ShaderResourceView* DiffuseMaterialTexture = Material.GetMaterialTexture();
		D3DDeviceContext->PSSetShaderResources(0, 1, &DiffuseMaterialTexture);

		// Passing the camera position through the material lighting data to the shader 
		XMStoreFloat4(&MaterialLightingData.CameraPositionWS, Camera->GetTransform().Position);

		// Passing the ambient light color and directional light color and direction properties
		XMStoreFloat4(&MaterialLightingData.AmbientColor, AmbientLightColour);
		XMStoreFloat4(&MaterialLightingData.DirectionLightColour, DirectionaLightColour);
		XMVECTOR NormalizedDirection = XMVector3Normalize(DirectionalLightDirection);
		XMStoreFloat4(&MaterialLightingData.DirectionLightDirection, NormalizedDirection);

		// Passing the point lights through the material lighting data to the shader 
		for (size_t PointlightIndex = 0; PointlightIndex < MAX_POINT_LIGHTS; ++PointlightIndex)
		{
			MaterialLightingData.PointLights[PointlightIndex] = PointLights[PointlightIndex];
		}

		// Passing the materials through the material lighting data to the shader 
		XMStoreFloat3(&MaterialLightingData.Diffuse, Material.GetDiffuseColor());
		MaterialLightingData.Opacity = Material.GetOpacity();
		XMStoreFloat3(&MaterialLightingData.Specular, Material.GetSpecularColor());
		MaterialLightingData.Metallic = Material.GetMetallic();
		MaterialLightingData.Emission = Material.GetShininess();
		MaterialLightingData.Roughness = Material.GetRoughness();
		MaterialLightingData.RefractionIndex = Material.GetRefractionIndex();

		// Updating transformation buffer and its subresources
		XMMATRIX World = SceneObject[ObjectIndex]->GetTransform().GetWorldMatrix();
		ObjectTransformData.WorldMatrix = World;
		ObjectTransformData.WVP = World * View * Projection;
		D3DDeviceContext->UpdateSubresource(ObjectTransformConstantBuffer.Get(), 0, nullptr, &ObjectTransformData, 0, 0);
		D3DDeviceContext->VSSetConstantBuffers(0, 1, ObjectTransformConstantBuffer.GetAddressOf());

		// Updating material lighting constant buffer and its subresources
		D3DDeviceContext->UpdateSubresource(MaterialLightingConstantBuffer.Get(), 0, nullptr, &MaterialLightingData, 0, 0);
		D3DDeviceContext->PSSetConstantBuffers(0, 1, MaterialLightingConstantBuffer.GetAddressOf());

		SceneObject[ObjectIndex]->GetModel()->Draw();

		D3DDeviceContext->PSSetShaderResources(0, 1, &DiffuseMaterialTexture);
	}

	// Rendering the UI elements on the top left
	RenderUI();

	SwapChain->Present(0, 0);
}

void Renderer::RenderUI()
{
	// Begining the SpriteBatch for 2D drawing
	SpriteBatch->Begin();

	// Creating string to hold player score on pizza boxes collected
	std::wstring ScoreText = L"x" + std::to_wstring(GameManager::GetPlayer().GetScore());

	// Drawing the icon and then the score text
	SpriteBatch->Draw(IconTexture.Get(), IconPosition);
	SpriteFont->DrawString(SpriteBatch.get(), ScoreText.c_str(), FontPosition, Colors::BlanchedAlmond);

	// Flush all 2D draws
	SpriteBatch->End();
}

ID3D11Device& Renderer::GetDevice()
{
	return *D3DDevice.Get();
}

ID3D11DeviceContext& Renderer::GetDeviceContext()
{
	return *D3DDeviceContext.Get();
}

HWND& Renderer::GetWindowHandler()
{
	return WindowHandle;
}

void Renderer::ShutdownGraphics()
{
	// Release texture/sampler resources
	TiledWrapSampler.Reset();
	ClampEdgeSampler.Reset();
	IconTexture.Reset();

	// Reset sprite objects
	SpriteBatch.reset();
	SpriteFont.reset();

	// Release buffers
	VertexBuffer.Reset();
	IndexBuffer.Reset();
	ObjectTransformConstantBuffer.Reset();
	MaterialLightingConstantBuffer.Reset();
	SkyboxTransformConstantBuffer.Reset();

	// Release shaders and input layouts
	VertexShader.Reset();
	PixelShader.Reset();
	InputLayout.Reset();
	SkyboxVertexShader.Reset();
	SkyboxPixelShader.Reset();
	SkyboxInputLayout.Reset();

	// Release rasterizer/depth-stencil/blend states
	AbsorbentRasterizerState.Reset();
	ReflectiveRasterizerState.Reset();
	AbsorbentDepthStencilState.Reset();
	ReflectiveDepthStencilState.Reset();
	AlphaBlendStateEnabled.Reset();
	AlphaBlendStateDisabled.Reset();

	// Swapchain and views
	RenderTargetViewBackBuffer.Reset();
	DepthBufferStencilView.Reset();
	SwapChain.Reset();
}
