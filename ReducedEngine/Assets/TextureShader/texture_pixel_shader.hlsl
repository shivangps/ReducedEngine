struct ps_in
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 position : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 albedoSpec : SV_TARGET2;
};

Texture2D colorTexture : register(t0);
SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float3 coreColor = colorTexture.Sample(mainSampler, IN.texCoord).rgb;
    
    OUT.position = IN.position;
    OUT.normal.rgb = IN.normal;
    OUT.albedoSpec.rgb = coreColor;
}