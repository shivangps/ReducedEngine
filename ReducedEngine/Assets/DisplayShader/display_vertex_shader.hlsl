struct vs_in
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

vs_out main(vs_in vIn)
{
    vs_out vOut;
    vOut.position = float4(vIn.position, 1.0f);
    vOut.texCoord = vIn.texCoord;
    return vOut;
}