struct VS_Input
{
    float2 pos : POSITION;
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
    output.position = float4(input.pos, 0.0f, 1.0f);
    output.color = input.color;

    return output;
}