cbuffer FrameConstants : register(b0)
{
    float4x4 uWorldViewProjection;
    float4   uFogColor;    // rgb = fog colour, a = fog amount (0 = off)
    float4   uAlphaTest;   // x = reference value, y = enabled (1 or 0)
};

struct VSInput
{
    float2 pos   : POSITION;
    float4 color : COLOR0;
    float2 uv    : TEXCOORD0;
};

struct VSOutput
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
    float2 uv    : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput o;
    float4 p = mul(uWorldViewProjection, float4(input.pos, 0.0f, 1.0f));
    p.z = 0.5f * p.w;   // 2D only: sidesteps GL's [-1,1] vs D3D's [0,1] depth range
    o.pos = p;
    o.color = input.color;
    o.uv = input.uv;
    return o;
}
