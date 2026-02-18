Texture2D SkyTexture : register(t0);
SamplerState SkyboxSampler : register(s0);

struct PSInput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0; 
    float3 Normal : NORMAL0; 
    float3 Tangent : TANGENT0; 
    float3 Bitangent : BINORMAL0; 
};

float4 main(PSInput input) : SV_Target
{
    // Sample the equirectangular skybox texture
    float4 sampledSky = SkyTexture.Sample(SkyboxSampler, input.UV);
    
    // Multiply by vertex color
    return sampledSky * input.Color;
}