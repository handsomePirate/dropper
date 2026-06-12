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
    float maxDist;
};

[shader("vertex")]
VertexOut vertexMain(VertexIn in: VertexIn)
{
    VertexOut out;
    float4 worldPos = mul(in.model, float4(in.position, 1.0f));
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
    float dist = cameraPos.y - in.worldPos.y - 5;
    float distMask = max(maxDist - dist, 0) / maxDist;
    float playerDist = distance(float2(cameraPos.x, cameraPos.z), float2(in.worldPos.x, in.worldPos.z)) * 0.5;
    float shadowDistMask = distMask * distMask;
    float playerShadow = shadowDistMask * min(playerDist + 0.75, 1) + (1 - shadowDistMask);
    float3 result = proxMult * distMask * playerShadow * in.color;

    return float4(result, 1.0f);
}
