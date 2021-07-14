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
    float3 fragPosition : FRAGPOSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer TransformationData : register(b0)
{
    float4x4 cameraMatrix;
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 normalMatrix;
}

void main(in vs_in IN, out vs_out OUT)
{
    OUT.position = mul(cameraMatrix, float4(IN.position, 1.0f));
    OUT.normal = normalize(mul(IN.normal, (float3x3)normalMatrix));
    OUT.fragPosition = mul(float4(IN.position, 1.0f), modelMatrix).rgb;
    OUT.texCoord = IN.texCoord;
}