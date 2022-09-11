struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 color : SV_TARGET0;
};

cbuffer TransofrmationData : register(b1)
{
    float3 spriteColor;
    int Pad0;
}

cbuffer TransformationData : register(b0)
{
    float4x4 cameraMatrix;
}

Texture2D characterTexture : register(t0); //4

SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float2 modTexCoord = float2(IN.texCoord.x, 1 - IN.texCoord.y);
    float4 color = characterTexture.Sample(mainSampler, modTexCoord).rgba;

    //if (color.r < 0.01f)
    //{
    //    discard;
    //}
    color.a = 1.0f;
    OUT.color = float4(spriteColor, 1.0f);
}