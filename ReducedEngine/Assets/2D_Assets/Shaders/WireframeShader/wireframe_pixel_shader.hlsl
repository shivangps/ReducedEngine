struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 albedo : SV_TARGET0;
};

cbuffer LocalPixelData : register(b1)
{
    float3 color;
    int Pad0;
}

ps_out main(in ps_in IN)
{
    ps_out OUT;
    OUT.albedo = float4(color, 1.0f);
    return OUT;
}