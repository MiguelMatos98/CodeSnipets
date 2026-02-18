#include "Material.h"

Material::Material()
    : Texture{ nullptr },
      NormalMap{ nullptr },
      SamplerState{ nullptr },
      Opacity{ 1.0f },
      RefractionIndex{ 0.0f },
      DiffuseColor{ 1.0f, 1.0f, 1.0f, 1.0f },
      SpecularColor{ 1.0f, 1.0f, 1.0f, 1.0f },
      Shininess{ 0.0f },
      Metallic{ 0.0f },
      Roughness{ 1.0f },
      Glossiness{ 0.0f },
      ReflectionStrength{ 0.0f }
{
}

XMVECTOR Material::GetDiffuseColor() const
{
    return DiffuseColor;
}

XMVECTOR Material::GetSpecularColor() const
{
    return SpecularColor;
}

float Material::GetShininess() const
{
    return Shininess;
}

float Material::GetOpacity() const
{
    return Opacity;
}

float Material::GetReflectionStrength() const
{
    return ReflectionStrength;
}

float Material::GetMetallic() const
{
    return Metallic;
}

float Material::GetRoughness() const
{
    return Roughness;
}

float Material::GetGlossiness() const
{
    return Glossiness;
}

float Material::GetRefractionIndex() const
{
    return RefractionIndex;
}

ID3D11ShaderResourceView* Material::GetMaterialTexture()
{
    return Texture;
}

void Material::SetMaterialTexture(ID3D11ShaderResourceView* Testure)
{
	this->Texture = Testure;
}

ID3D11SamplerState* Material::GetMaterialSamplerState()
{
    return SamplerState;
}

ID3D11ShaderResourceView* Material::GetNormalMap() const
{
    return NormalMap;
}

void Material::SetNormalMap(ID3D11ShaderResourceView* Normalmap)
{
	this->NormalMap = Normalmap;
}

void Material::SetMaterialSamplerState(ID3D11SamplerState* SamplerState)
{
    this->SamplerState = SamplerState;
}
