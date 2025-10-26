#define blocksize 128

struct Vertex
{
    float3 position;
    float2 tcoord;
	float3 normal;
	float3 tangent;
	float3 binormal;
};

StructuredBuffer<Vertex> inputBuffer : register(t0);     // SRV
StructuredBuffer<uint> boneId : register(t1);
StructuredBuffer<float4x4> boneMat : register(t2);  
RWStructuredBuffer<Vertex> outputBuffer : register(u0);   // UAV

[numthreads(blocksize, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{   
    //outputBuffer[DTid.x] = inputBuffer[DTid.x];
    
    uint vert = DTid.x;
	uint bone = boneId[vert];
	
	float3x3 normMat = boneMat[bone];
	
	if(bone == -1)
	{
		outputBuffer[vert] = inputBuffer[vert];
	}
	else
	{
        outputBuffer[vert].position = mul(boneMat[bone], float4(inputBuffer[vert].position, 1.0));
        outputBuffer[vert].tcoord = inputBuffer[vert].tcoord;
		outputBuffer[vert].normal = mul(normMat, inputBuffer[vert].normal);
        outputBuffer[vert].tangent = mul(normMat, inputBuffer[vert].tangent);
		outputBuffer[vert].binormal = mul(normMat, inputBuffer[vert].binormal);
	}
}