struct VertexIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4x4 model;
};

struct VertexOut
{
    float4 position : SV_Position;
    float3 worldPos;
    float3 normal;
    float3 color;
    float2 uv;
};

[[vk::binding(0)]]
cbuffer CameraBuffer
{
    float4x4 vp;
    float3 cameraPos;
    float proxMult;
};

[[vk::binding(1)]]
cbuffer UniformBuffer
{
    float3 color;
};

[shader("vertex")]
VertexOut vertexMain(VertexIn in: VertexIn)
{
    VertexOut out;
    float4 worldPos = mul(in.model, float4(in.position, 1.0f)) + float4(0, cameraPos.y, 0, 0);
    out.position = mul(vp, worldPos);
    out.worldPos = worldPos.xyz;
    out.normal = mul(in.model, float4(in.normal, 0.0f)).xyz;
    out.color = color;
    out.uv = in.uv;
    return out;
}

[shader("pixel")]
float4 fragmentMain(VertexOut in) : SV_Target0
{
    float pi = 3.1415926538;
    float rad = in.uv.x * 2 * pi;
    float mult = 1 - cos(abs(in.worldPos.y) / 10 + rad);

    float3 result = in.color * in.uv.y * mult * proxMult;

    return float4(result, 1.0f);
}
