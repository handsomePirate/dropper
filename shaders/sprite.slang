struct VertexIn
{
    float2 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 transform;
};

struct VertexOut
{
    float4 position : SV_Position;
    float2 uv;
};

[[vk::binding(0, 0)]]
Sampler2D sampler : register(t0) : register(s0);

[shader("vertex")]
VertexOut vertexMain(VertexIn in: VertexIn)
{
    VertexOut out;
    float2 transformedPosition = float2((in.position.x * in.transform.z + in.transform.x) * 2 - 1, (in.position.y * in.transform.w + in.transform.y) * 2 - 1);
    out.position = float4(transformedPosition.x, transformedPosition.y, 0, 1);
    out.uv = in.uv;
    return out;
}

[shader("pixel")]
float4 fragmentMain(VertexOut in) : SV_Target0
{
    float4 sample = sampler.Sample(in.uv);
    return sample;
}
