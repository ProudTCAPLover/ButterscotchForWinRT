cbuffer FrameConstants : register(b0)
{
    float4x4 uWorldViewProjection;
    float4   uFogColor;
    float4   uAlphaTest;
};

Texture2D    uTexture : register(t0);
SamplerState uSampler : register(s0);

struct PSInput
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
    float2 uv    : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 c = uTexture.Sample(uSampler, input.uv) * input.color;
    float keep = (uAlphaTest.y < 0.5f || c.a > uAlphaTest.x) ? 1.0f : -1.0f;
    clip(keep);   // matches the GL shader: discard when ref >= alpha
    c.rgb = lerp(c.rgb, uFogColor.rgb, uFogColor.a);
    return c;
}
