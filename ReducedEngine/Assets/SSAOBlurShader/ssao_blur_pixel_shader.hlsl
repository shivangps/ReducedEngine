struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float color : SV_TARGET;
};

cbuffer Blur_CB : register(b0)
{
    unsigned int width = 0;
    unsigned int height = 0;
    int Pad0 = 0;
    int Pad1 = 0;
};

Texture2D inputTexture : register(t0);

SamplerState mainSampler : register(s0);

ps_out main(in ps_in IN)
{
    ps_out OUT;
    float2 modTexCoord = float2(IN.texCoord.x, 1.0f - IN.texCoord.y);
    
    float2 texelSize = float2(1.0f / width, 1.0f / height);
    
    float2 kernels[9] =
    {
        float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f),
        float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f),
        float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f),
    };
    
    float colorValue = 0.0f;
    for (uint i = 0; i < 9; i++)
    {
        colorValue += inputTexture.Sample(mainSampler, modTexCoord + (kernels[i] * texelSize)).r;
    }
    colorValue = colorValue / 9.0f;
    
    OUT.color = colorValue;
    
    return OUT;
}