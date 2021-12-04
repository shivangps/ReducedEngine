struct ps_in
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 position : SV_TARGET0;
    float4 fragmentViewPosition : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 albedoSpec : SV_TARGET3;
};

TextureCube cubeMap : register(t0);
SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float4 resultingColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    resultingColor = cubeMap.Sample(mainSampler, normalize(IN.texCoord));
    
    OUT.albedoSpec.rgb = resultingColor.rgb;
    return;
}