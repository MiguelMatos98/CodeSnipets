#pragma once
#include "d3d11.h"
#include "DirectXMath.h"

using namespace DirectX;

class Material
{
private:
    ID3D11ShaderResourceView* Texture;
    ID3D11ShaderResourceView* NormalMap;
    ID3D11SamplerState* SamplerState;
    XMVECTOR DiffuseColor;
    XMVECTOR SpecularColor;
    float Shininess;
    float Opacity;
    float ReflectionStrength;
    float Metallic;
    float Roughness;
    float Glossiness;
    float RefractionIndex;
public:
    Material();
    XMVECTOR GetDiffuseColor() const;
    XMVECTOR GetSpecularColor() const;
    float GetShininess() const;
    float GetOpacity() const; 
    float GetReflectionStrength() const;
    float GetMetallic() const;
    float GetRoughness() const;
    float GetGlossiness() const;
    float GetRefractionIndex() const;
    ID3D11ShaderResourceView* GetMaterialTexture();
    void SetMaterialTexture(ID3D11ShaderResourceView* Testure);
    ID3D11SamplerState* GetMaterialSamplerState();
    ID3D11ShaderResourceView* GetNormalMap() const;
    void SetNormalMap(ID3D11ShaderResourceView* NormalMap);
    void SetMaterialSamplerState(ID3D11SamplerState* SamplerState);
};

