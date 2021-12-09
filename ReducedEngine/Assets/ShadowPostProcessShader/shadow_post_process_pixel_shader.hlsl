struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float color : SV_TARGET;
};

cbuffer ShadowPostProcessData
{
    uint width;
    uint height;
    int Pad0;
    int Pad1;
};

Texture2D inputShadowTexture : register(t0); // 4

SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float2 modTexCoord = float2(IN.texCoord.x, 1.0f - IN.texCoord.y);
    
    float colorValue = 0.0f;
    
    // Post process the shadow results.
    // Blur it.
    float2 texelSize = float2(1.0f / width, 1.0f / height);
    float2 kernels[9] =
    {
        float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f),
        float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f),
        float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f),
    };
    
    for (uint i = 0; i < 9; i++)
    {
        colorValue += inputShadowTexture.Sample(mainSampler, modTexCoord + (kernels[i] * texelSize)).r;
    }
    colorValue = colorValue / 9.0f;
    
    //if (colorValue > 0.8f)
    //    colorValue = 1.0f;
    
    colorValue = inputShadowTexture.Sample(mainSampler, modTexCoord).r;
    
    OUT.color = colorValue;
    }