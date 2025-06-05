cbuffer ViewMatrix
{
    float4x4 gWorldViewProj;
};

struct VS_Input
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_Output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_Output vs_main(VS_Input input)
{
    VS_Output output;
    output.position = float4(input.pos, 1.0f);
    output.color = input.color;
    
    // transform position
    output.position = mul(float4(input.pos, 1.0f), gWorldViewProj);

    return output;
}