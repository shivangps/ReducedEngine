struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 color : SV_TARGET;
};

Texture2D displayTexture : register(t0); //4

SamplerState mainSampler : register(s0);

ps_out main(in ps_in IN)
{
    ps_out OUT;
    // Retrieve data of pixel from G-Buffer.
    float3 color = displayTexture.Sample(mainSampler, IN.texCoord).xyz;

    OUT.color = float4(color, 1.0f);
    return OUT;
}