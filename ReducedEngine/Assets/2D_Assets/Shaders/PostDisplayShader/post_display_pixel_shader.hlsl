struct ps_in
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 color : SV_TARGET;
};

Texture2D displayTexture : register(t0); //4
Texture2D uiTexture : register(t1); //4

SamplerState mainSampler : register(s0);

ps_out main(in ps_in IN)
{
    ps_out OUT;
    float2 modTexCoord = float2(IN.texCoord.x, 1.0f - IN.texCoord.y);
    // Retrieve data of pixel from G-Buffer.
    float3 color = displayTexture.Sample(mainSampler, modTexCoord).rgb;
    float4 uiColor = uiTexture.Sample(mainSampler, modTexCoord).rgba;

    // Print the UI on top of the game scene based on the alpha value of the UI frame buffer.
    color = (color * (1.0f - uiColor.a)) + (uiColor.rgb * uiColor.a);
    
    OUT.color = float4(color, 1.0f);
    return OUT;
}