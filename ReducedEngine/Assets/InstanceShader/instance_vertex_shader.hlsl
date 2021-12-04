struct vs_in
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint instanceID : SV_InstanceID;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 fragPosition : FRAGPOSITION;
    float2 texCoord : TEXCOORD;
};

struct InstanceData
{
    float3 position;
    uint Pad0;
};

cbuffer TransformationData : register(b0)
{
    float4x4 cameraMatrix;
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 normalMatrix;
}

StructuredBuffer<InstanceData> instanceData : register(t0);

void main(in vs_in IN, out vs_out OUT)
{
    InstanceData data = instanceData[IN.instanceID];
    
    float3 newPosition = IN.position + data.position;
    
    OUT.position = mul(cameraMatrix, float4(newPosition, 1.0f));
    OUT.normal = normalize(mul(IN.normal, (float3x3) normalMatrix));
    OUT.fragPosition = mul(float4(newPosition, 1.0f), modelMatrix).rgb;
    OUT.texCoord = IN.texCoord;
}