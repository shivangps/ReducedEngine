struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float color : SV_TARGET;
};

cbuffer ShadowRenderData : register(b0)
{
    uint shadowWidth;
    uint shadowHeight;
    int Pad0;
    int Pad1;
    float4x4 lightSpaceMatrix;
}

Texture2D fragmentPositionTexture : register(t0);   // 4
Texture2D shadowDepthTexture : register(t1);        // 2
Texture2D normalTexture : register(t2);             // 4

SamplerState mainSampler : register(s0);

float CalculateShadow(float4 fragmentPositionLightSpace)
{
    float shadow = 0.5f;
    
    // perform perspective divide
    float3 projectionCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
    
    // bring the coordinates to Normalized Device Coordinates.
    projectionCoords.x = 0.5f + projectionCoords.x * 0.5f;
    projectionCoords.y = 0.5f - projectionCoords.y * 0.5f;
    
    //float2 trueCoord;
    //trueCoord.x = ((projectionCoords.x * (float)shadowWidth) / shadowWidth);
    //trueCoord.y = ((projectionCoords.y * (float)shadowHeight) / shadowHeight);
    
    // get the closest depth to light
    float closestDepth = shadowDepthTexture.Sample(mainSampler, projectionCoords.xy).r;
    
    // get the depth of the pixel from light.
    float currentDepth = projectionCoords.z;
    
    // bias consists to remove shadow acne.
    float bias = 0.004f;
    //float bias = max(0.007f * (1.0f - dot(normalize(lightDir), normalize(surfaceNormal))), 0.002f);
    // compare the current pixel depth to the one thats closest and see if its behind it, 
    // and darken it as a shadow if its behind.
    //shadow = ((currentDepth - bias) > closestDepth) ? 0.0f : 1.0f;
    
    // softening the shadows using PCF method 1.
    float2 texelSize = 1.0f / float2(shadowWidth, shadowHeight);
    int sampleRadii = 2;
    for (int x = -sampleRadii; x <= sampleRadii; x++)
    {
        for (int y = -sampleRadii; y <= sampleRadii; y++)
        {
            float pcfDepth = shadowDepthTexture.Sample(mainSampler, projectionCoords.xy + float2(x, y) * texelSize).r;
            shadow += ((currentDepth - bias) > pcfDepth) ? 0.0f : 1.0f;
        }
    }
    float sampleSize = pow((2 * (float) sampleRadii) + 1, 2);
    shadow = shadow / sampleSize;
    
    return shadow;
}

void main(in ps_in IN, out ps_out OUT)
{
    float2 modTexCoord = float2(IN.texCoord.x, 1.0f - IN.texCoord.y);
    float3 fragmentPosition = fragmentPositionTexture.Sample(mainSampler, modTexCoord).xyz;
    float3 normal = normalTexture.Sample(mainSampler, modTexCoord).xyz;
    
    float4 fragmentPositionLightSpace = mul(lightSpaceMatrix, float4(fragmentPosition, 1.0f));
    
    float monoColor = 1.0f;
    
    if(length(normal) > 0.1f)
    {
        monoColor = CalculateShadow(fragmentPositionLightSpace);
    }
    
    OUT.color = monoColor;
}