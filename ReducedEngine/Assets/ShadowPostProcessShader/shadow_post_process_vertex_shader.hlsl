struct vs_in
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

vs_out main(vs_in vIn)
{
    vs_out vOut;
    vOut.position = vIn.position;
    vOut.texcoord = vIn.texcoord;
    return vOut;
}