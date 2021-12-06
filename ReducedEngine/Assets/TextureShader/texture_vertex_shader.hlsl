struct vs_in
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 fragPosition : FRAGPOSITION;
    float4 fragViewPosition : FRAGVIEW;
    float2 texCoord : TEXCOORD;
    float3x3 tbnMatrix : TBNMAT;
};

cbuffer TransformationData : register(b0)
{
    float4x4 cameraMatrix;
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 normalMatrix;
    float4x4 modelViewMatrix;
}

void main(in vs_in IN, out vs_out OUT)
{
    OUT.position = mul(cameraMatrix, float4(IN.position, 1.0f));
    OUT.fragViewPosition = mul(float4(IN.position, 1.0f), modelViewMatrix);
    OUT.fragPosition = mul(float4(IN.position, 1.0f), modelMatrix);
    OUT.texCoord = IN.texCoord;
    
    // Tangent, Bitangent and Normal matrix construction.
    OUT.normal = normalize(mul(IN.normal, (float3x3) normalMatrix));
    float3 T = normalize(float3(mul(float4(IN.tangent.x, IN.tangent.y, IN.tangent.z, 0.0f), normalMatrix).rgb));
    float3 B = normalize(float3(mul(float4(IN.bitangent.x, IN.bitangent.y, IN.bitangent.z, 0.0f), normalMatrix).rgb));
    float3 N = normalize(float3(mul(float4(IN.normal.x, IN.normal.y, IN.normal.z, 0.0f), normalMatrix).rgb));
    
    OUT.tbnMatrix = float3x3(T, B, N);

}