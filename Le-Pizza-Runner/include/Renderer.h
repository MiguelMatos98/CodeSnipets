#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <iterator>
#include "ReadData.h"
#include "text2D.h"
#include "objFileModel.h"
#include <Mouse.h>
#include <Keyboard.h>
#include "Camera.h"
#include <wrl/client.h>
#include <memory>
#include "SpriteFont.h"
#include <DirectXColors.h>
#include <SimpleMath.h>

using Microsoft::WRL::ComPtr;

#define MAX_POINT_LIGHTS 2
#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900

class Renderer
{
private:
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT4 Color;
		XMFLOAT2 UV;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;   
		XMFLOAT3 Bitangent; 
	};

	struct PointLight
	{
		DirectX::XMFLOAT3 position; // 12 bytes
		float padding;              // 4 bytes (padding)

		DirectX::XMFLOAT3 colour;   // 12 bytes
		
		float strength;             // 4 bytes
		float Kc;                  // 4 bytes
		float Kl;                  // 4 bytes
		float Kq;                  // 4 bytes
		float padding2;             // 4 bytes to make total 48 bytes
	};

	struct SkyboxTransformBuffer
	{
		XMMATRIX WVP = XMMatrixIdentity(); // World-View-Projection matrix
	};

	struct TransformBuffer
	{
		XMMATRIX WorldMatrix = XMMatrixIdentity(); // World-Matrix 
		XMMATRIX WVP = XMMatrixIdentity(); // World-View-Projection matrix (For Camera Clip Space)
	};

	struct MaterialLightingBuffer
	{
		XMFLOAT4 CameraPositionWS;
		XMFLOAT4 AmbientColor;
		XMFLOAT4 DirectionLightColour;
		XMFLOAT4 DirectionLightDirection;

		PointLight PointLights[MAX_POINT_LIGHTS];

		XMFLOAT3 Diffuse;
		float    Opacity;

		XMFLOAT3 Specular;
		float    Metallic;

		float    Emission;
		float    Roughness;
		float    RefractionIndex;
		float    Padding5; // Padding to ensure alignment
	};

	const wchar_t* WindowHeaderTitle;

	XMVECTOR AmbientLightColour;
	
	XMVECTOR DirectionalLightDirection;
	
	XMVECTOR DirectionaLightColour;
	
	DirectX::SimpleMath::Vector2 IconPosition;
	
	DirectX::SimpleMath::Vector2 FontPosition;

	HWND WindowHandle;

	ComPtr<ID3D11Device> D3DDevice;

	ComPtr<ID3D11DeviceContext> D3DDeviceContext;
	
	ComPtr<ID3D11Buffer> VertexBuffer;
	
	ComPtr<ID3D11Buffer> IndexBuffer;
	
	ComPtr<ID3D11Buffer> ObjectTransformConstantBuffer;
	
	ComPtr<ID3D11Buffer> MaterialLightingConstantBuffer;
	
	ComPtr<ID3D11Buffer> SkyboxTransformConstantBuffer;
	
	ComPtr<ID3D11SamplerState> TiledWrapSampler;

	ComPtr<ID3D11SamplerState> ClampEdgeSampler;
	
	ComPtr<ID3D11RasterizerState> AbsorbentRasterizerState;
	
	ComPtr<ID3D11RasterizerState> ReflectiveRasterizerState;
	
	ComPtr<ID3D11DepthStencilState> AbsorbentDepthStencilState;
	
	ComPtr<ID3D11DepthStencilState> ReflectiveDepthStencilState;
	
	ComPtr<ID3D11VertexShader> SkyboxVertexShader;
	
	ComPtr<ID3D11PixelShader> SkyboxPixelShader;
	
	ComPtr<ID3D11InputLayout> SkyboxInputLayout;
	
	ComPtr<ID3D11VertexShader> VertexShader;
	
	ComPtr<ID3D11PixelShader> PixelShader;
	
	ComPtr<ID3D11InputLayout> InputLayout;
	
	ComPtr<IDXGISwapChain> SwapChain;
	
	ComPtr<ID3D11RenderTargetView> RenderTargetViewBackBuffer;
	
	ComPtr<ID3D11DepthStencilView> DepthBufferStencilView;
	
	ComPtr<ID3D11BlendState> AlphaBlendStateEnabled;
	
	ComPtr<ID3D11BlendState> AlphaBlendStateDisabled;
	
	ComPtr<ID3D11ShaderReflection> VertexShaderReflection;
	
	TransformBuffer ObjectTransformData;
	
	SkyboxTransformBuffer  SkyboxTransformData;
	
	MaterialLightingBuffer  MaterialLightingData;

	std::unique_ptr<DirectX::SpriteBatch>        SpriteBatch;

	std::unique_ptr<DirectX::SpriteFont>         SpriteFont;
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> IconTexture;

	static const UINT MaxPixelShaderResourceSlots;
public:
	Renderer();
	~Renderer();
	PointLight PointLights[MAX_POINT_LIGHTS];
	void InitializeGraphics();
	HRESULT InitializeWindow(const HINSTANCE InstanceHandle, const int CmdShow);
	HRESULT InitializeD3D();
	HRESULT LoadPixelShader(const wchar_t* const FileName, ID3D11PixelShader** PixelShader);
	HRESULT LoadVertexShader(const wchar_t* const FileName, ID3D11VertexShader** VertexShader, ID3D11InputLayout** InputLayout);
	// Load shaders and input layouts
	HRESULT InitializePipeline();
	void DrawSkyBox(Transform& Transform, XMMATRIX& View, XMMATRIX& Projection);
	void RenderFrame();
	ID3D11Device& GetDevice();
	ID3D11DeviceContext& GetDeviceContext();
	void RenderUI();
	HWND& GetWindowHandler();
	void ShutdownGraphics();
};
