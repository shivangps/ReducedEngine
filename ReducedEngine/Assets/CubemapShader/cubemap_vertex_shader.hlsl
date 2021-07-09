struct vs_in
{
    float3 position : POSITION;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

cbuffer transformMatrix : register(b0)
{
    float4x4 model;
    float4x4 projection;
    float4x4 view;
    float4x4 viewProj;
};

void main(in vs_in IN, out vs_out OUT)
{
    OUT.texCoord = IN.position;
    
    float4 pos = float4(1.0f, 1.0f, 1.0f, 1.0f);
    pos = float4(IN.position, 1.0f);
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    OUT.position = pos.xyww;
    return;
}