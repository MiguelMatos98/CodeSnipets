#define MAX_POINT_LIGHTS 8

struct PointLight
{
    float4 position; // 16 bytes
    
    float4 colour; // 16 bytes
    
    float strength; // 4 bytes
    
    bool enabled; // 4 bytes
    
    float2 padding; // 8 bytes
};

struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer CBuffer0
{
    matrix WVP; // 64 bytes
    //float3 pos;
    //float padding;
    float4 amnbientLightCol; // 16 bytes
    float4 directionLightDir; // 16 bytes
    float4 directionalLightCol; // 16 bytes
    // SINGLE POINTLIGHT BUFFERING
    //float4 pointLightPos; // 16 bytes
    //float4 PointLightCol; // 16 bytes
    //float pointLightStrength; // 4 bytes
	// MULTIPLE POINTLIGHT BUFFERING
    PointLight pointLights[MAX_POINT_LIGHTS]; // 384 bytes
};

VOut main(VIn input)
{
    VOut output;
    //output.position = float4(input.position + pos, 1);
    output.position = mul(WVP, float4(input.position, 1));
    
    // Texture coords
    output.uv = input.uv;
    
    // OVERALL LIGHTING
    float diffuseAmount = dot(directionLightDir.xyz, input.norm);
   // Clapping Ligth to 0-1 INSTEAD of -1-1
    diffuseAmount = saturate(diffuseAmount);
    // To briten or darken the light contribution on each polygon face (difuse)
    float3 directionalFinal = directionalLightCol.rgb * diffuseAmount;
    
    // POINTLIGHT LIGHTING
    float3 pointFinal = float3(0,0,0);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (!pointLights[i].enabled)
            continue; // Skip disabled lights

        // Multiple Point Light Set Up
        float3 pointLightDir = normalize(pointLights[i].position.xyz - input.position);
        float pointLightDistance = length(pointLights[i].position.xyz - input.position);
        float pointLightAttenuation = pointLights[i].strength / (pointLightDistance * pointLightDistance + pointLights[i].strength); // A = Strength / d^2+Strength
        float pointAmount = dot(pointLightDir.xyz, input.norm) * pointLightAttenuation;
        pointAmount = saturate(pointAmount);
        pointFinal += pointLights[i].colour.rgb * pointAmount * 5.0f;
    }
    
    // Single Point Light Set Up
    //float3 pointLightDir = normalize(pointLightPos.xyz - input.position);
    //float pointLightDistance = length(pointLightPos.xyz - input.position);
    //float pointLightAttenuation = pointLightStrength / (pointLightDistance * pointLightDistance + pointLightStrength); // A = Strength / d^2+Strength
    //float pointAmount = dot(float3(pointLightDir.xyz), input.norm) * pointLightAttenuation;
    //pointAmount = saturate(pointAmount);
    //float3 pointFinal = PointLightCol * pointAmount;
    
    // Final colour
    output.colour = saturate(amnbientLightCol + float4(directionalFinal, 1) + float4(pointFinal, 1));
    return output;
}