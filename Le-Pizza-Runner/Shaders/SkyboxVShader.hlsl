cbuffer SkyboxTransformBuffer : register(b0)
{
    matrix WVP;
};

struct VIn
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BINORMAL;
};

struct VOut
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0; 
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : BINORMAL0; 
};

VOut main(VIn input)
{
    VOut output;
    
    // Transform vertex position to clip space
    output.Position = mul(float4(input.Position, 1.0f), WVP);
    
    // Push to far plane to always render behind scene
    output.Position.z = output.Position.w;
    
    // Pass UVs directly (Unreal-style equirectangular mapping)
    output.UV = input.UV;
    
    // Pass vertex color
    output.Color = input.Color;
    
    // Pass normal, tangent, bitangent (even though skybox doesn't need lighting)
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;
    output.Bitangent = input.Bitangent;
    
    return output;
}