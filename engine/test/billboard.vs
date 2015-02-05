float4 main(float3 pos: POSITION) : SV_POSITION { 
    float4 result;
	result.xyz = pos.xyz;
	result.w = 1.0f;
	return result;
}