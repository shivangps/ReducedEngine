struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 color : SV_TARGET;
};

cbuffer DirectionalLightCharacteristics : register(b0)
{
    float4 lightDirection;
    
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    
    float4 viewDirection;
    
    float4x4 lightSpaceMatrix;
    
    float shadowWidth;
    float shadowHeight;
};

Texture2D fragmentPositionTexture : register(t0);   // 4
Texture2D normalTexture : register(t1);             // 4
Texture2D albedoSpecTexture : register(t2);         // 4
Texture2D shadowDepthTexture : register(t3);        // 2
Texture2D ssaoTexture : register(t4);               // 4

SamplerState mainSampler : register(s0);

float CalculateDirectionalDiffuseFactor(float3 lightDir, float3 surfaceNormal)
{
    // calculate if the suface is facing towards the light or away 
    // so as to darken it if its facing away using the surface normal.
    return max(dot(normalize(lightDir), normalize(surfaceNormal)), 0.0f);
}

float CalculateShadow(float4 fragmentPositionLightSpace, float3 surfaceNormal, float3 lightDir)
{
    // perform perspective divide
    float3 projectionCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
    
    // bring the coordinates to Normalized Device Coordinates.
    projectionCoords.x = 0.5f + projectionCoords.x * 0.5f;
    projectionCoords.y = 0.5f - projectionCoords.y * 0.5f;
    
    // get the closest depth to light
    float closestDepth = shadowDepthTexture.Sample(mainSampler, projectionCoords.xy).r;
    
    // get the depth of the pixel from light.
    float currentDepth = projectionCoords.z;
    
    // bias consists to remove shadow acne.
    float bias = 0.005f;
    //float bias = max(0.007f * (1.0f - dot(normalize(lightDir), normalize(surfaceNormal))), 0.002f);
    // compare the current pixel depth to the one thats closest and see if its behind it, 
    // and darken it as a shadow if its behind.
    float shadow = 0.0f;
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
    
    // Collect Data.
    // Retrieve data of pixel from G-Buffer.
    float3 normal = normalTexture.Sample(mainSampler, modTexCoord).rgb;
    float3 color = albedoSpecTexture.Sample(mainSampler, modTexCoord).rgb;
    float specular = albedoSpecTexture.Sample(mainSampler, modTexCoord).a;
    float3 fragmentPosition = fragmentPositionTexture.Sample(mainSampler, modTexCoord).rgb;
    float ambientOcclusion = ssaoTexture.Sample(mainSampler, modTexCoord).r;
    
    float4 fragmentPositionLightSpace = mul(lightSpaceMatrix, float4(fragmentPosition, 1.0f));
    
    float3 resultColor = color;
    
    // Calculate.
    if(length(normal) > 0.1f)
    {
        float ambientFactor = 0.3f;
        float shadowFactor = CalculateShadow(fragmentPositionLightSpace, normal, lightDirection.rgb);
        float diffuseFactor = CalculateDirectionalDiffuseFactor(lightDirection.rgb, normal);
        if (diffuseFactor < ambientFactor)
            diffuseFactor = ambientFactor * ambientOcclusion * 2.0f;
        if (shadowFactor < ambientFactor)
            shadowFactor = ambientFactor * ambientOcclusion * 2.0f;
        float3 diffuse = lightDiffuse.rgb * color * (diffuseFactor + shadowFactor) / 2.0f * 1.5f;
        
        resultColor = diffuse;
    }
    
    OUT.color = float4(resultColor, 1.0f);
}