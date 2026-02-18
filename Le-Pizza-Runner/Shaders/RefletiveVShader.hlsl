cbuffer TransformBuffer : register(b0)
{
    matrix Matrix_W;   
    matrix Matrix_WVP; 
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
    float4 PositionClipSpace : SV_Position;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 Tangent : TEXCOORD1; 
    float3 Bitangent : TEXCOORD2;
    float3 Normal : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
};

VOut main(VIn input)
{
    VOut output;

    // Transforming position to world space and to clip space
    float4 worldPos = mul(Matrix_W, float4(input.Position, 1.0f));
    output.WorldPosition = worldPos.xyz; // Pass world position
    
    output.PositionClipSpace = mul(Matrix_WVP, float4(input.Position, 1.0f));
    
    // Passing color & UV
    output.Color = input.Color;
    output.UV = input.UV;
    
    // Calculating world-space normal
    float3 worldNormal = normalize(mul((float3x3) Matrix_W, input.Normal));
    float3 worldTangent = normalize(mul((float3x3) Matrix_W, input.Tangent));
    float3 worldBitangent = normalize(mul((float3x3) Matrix_W, input.Bitangent));
    

    output.Normal = worldNormal;
    output.Tangent = worldTangent;
    output.Bitangent = worldBitangent;

    return output;
}
