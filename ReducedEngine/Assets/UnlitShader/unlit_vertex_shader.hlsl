struct vs_in
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

cbuffer TransformationData : register(b0)
{
    float4x4 MVPmat;
}

void main(in vs_in IN, out vs_out OUT)
{
    OUT.position = float4(IN.position, 1.0f);
    OUT.normal = IN.normal;
    OUT.texCoord = IN.texCoord;
}