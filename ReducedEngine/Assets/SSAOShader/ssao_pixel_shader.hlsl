struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float color : SV_TARGET;
};

cbuffer SSAOSampleDescription : register(b0)
{
    uint samples;
    uint width;
    uint height;
    uint noiseWidth;
    uint noiseHeight;
    uint Pad0;
    uint Pad1;
    uint Pad2;
    float4x4 projection;
}

struct SampleData
{
    float3 samplePosition;
    int Pad0;
};

Texture2D fragmentPositionTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D noiseTexture : register(t2);
StructuredBuffer<SampleData> sampleKernels : register(t3);

SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float radius = 0.5f;
    float bias = 0.025f;
    
    // get input for SSAO algorithm
    float2 modTexCoord = float2(IN.texCoord.x, 1.0f - IN.texCoord.y);
    
    float3 fragPos = fragmentPositionTexture.Sample(mainSampler, modTexCoord).xyz;
    float3 normal = normalize(normalTexture.Sample(mainSampler, modTexCoord).xyz);
    
    // tile noise texture over screen based on screen dimensions divided by noise size
    float2 noiseScale = float2(float(width) / float(noiseWidth), float(height) / float(noiseHeight));
    
    float3 randomVector = normalize(noiseTexture.Sample(mainSampler, modTexCoord * noiseScale).rgb);
    
    // create TBN change-of-basis matrix: from tangent-space to view-space
    float3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0f;
    
    for (uint i = 0; i < samples; i++)
    {
        // get sample position
        float3 currentSamplePosition = sampleKernels[i].samplePosition;
        currentSamplePosition = mul(currentSamplePosition, TBN); // from tangent to view-space
        currentSamplePosition = fragPos + currentSamplePosition * radius;
        
        // project sample position (to sample texture) (to get position on screen/texture)
        float4 offset = float4(currentSamplePosition, 1.0f);
        offset = mul(offset, projection); // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.x = 0.5f + offset.x * 0.5f;
        offset.y = 0.5f - offset.y * 0.5f;

        // get sample depth
        float sampleDepth = fragmentPositionTexture.Sample(mainSampler, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));
        occlusion += ((sampleDepth <= currentSamplePosition.z + bias) ? 1.0f : 0.0f) * rangeCheck;
    }
    occlusion = 1.0f - (occlusion / float(samples));
    
    OUT.color = occlusion;
}