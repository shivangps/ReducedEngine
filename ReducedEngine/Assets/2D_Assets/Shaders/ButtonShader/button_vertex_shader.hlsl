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

cbuffer TransformationData : register(b0)
{
    float4x4 cameraMatrix;
}

void main(in vs_in IN, out vs_out OUT)
{
    //OUT.position = mul(cameraMatrix, float4(IN.position.xy, 0.0f, 1.0f));
    OUT.position = mul(float4(IN.position.xy * 0.5f, 0.0f, 1.0f), cameraMatrix);
    //OUT.position = float4(IN.position.xy * 0.5f, 0.0f, 1.0f);
    OUT.texCoord = IN.texCoord;
}