struct vs_in
{
    float2 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer TransformationData : register(b0)
{
    float4x4 cameraMatrix;
}

void main(in vs_in IN, out vs_out OUT)
{
    OUT.position = mul(cameraMatrix, float4(IN.position, 0.0f, 1.0f));
    OUT.texCoord = IN.texCoord;
}