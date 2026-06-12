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
    float3 normal;
    float3 color;
    float2 uv : TEXCOORD0;
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
    float proxMult;
};

[shader("vertex")]
VertexOut vertexMain(VertexIn in: VertexIn)
{
    VertexOut out;
    float4 worldPos = mul(in.model, float4(in.position, 1.0f));
    out.position = mul(vp, worldPos);
    out.normal = mul(in.model, float4(in.normal, 0.0f)).xyz;
    out.color = color;
    out.uv = in.uv;
    return out;
}

[shader("pixel")]
float4 fragmentMain(VertexOut in) : SV_Target0
{
    return float4(in.color, 1.0f);
}
