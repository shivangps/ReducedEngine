struct ps_in
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 fragPosition : FRAGPOSITION;
    float4 fragViewPosition : FRAGVIEW;
    float2 texCoord : TEXCOORD;
    float3x3 tbnMatrix : TBNMAT;
};

struct ps_out
{
    float4 fragmentPosition : SV_TARGET0;
    float4 fragmentViewPosition : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 albedoSpec : SV_TARGET3;
};

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float3 coreColor = colorTexture.Sample(mainSampler, IN.texCoord).rgb;
    
    OUT.fragmentPosition.rgb = IN.fragPosition;
    OUT.fragmentViewPosition = IN.fragViewPosition;
    OUT.albedoSpec.rgb = coreColor;
    
    // Normals calculations based on normal map texture.
    float3 normalTexColor = normalTexture.Sample(mainSampler, IN.texCoord).rgb;
    normalTexColor = normalTexColor * 2.0f - 1.0f;
    OUT.normal.rgb = normalize(mul(normalTexColor, IN.tbnMatrix));
}