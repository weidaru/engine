RWTexture2D<float4> output;

void plot(int x, int y)
{
   output[uint2(x, y)] = float4(0, 0, 1, 1);
}

void draw_line(float2 start, float2 end)
{
    float dydx = (end.y - start.y) / (end.x - start.x);
    float y = start.y;
    for (int x = start.x; x <= end.x; x++) 
    {
        plot(x, round(y));
        y = y + dydx;
    }
}

[numthreads(32, 32, 1)]
void main( uint3 thread_id : SV_DispatchThreadID )
{
    output[thread_id.xy] = float4(thread_id.xy / 1024.0f, 0, 1);

	if (thread_id.x == 1023 && thread_id.y == 1023)
	{
		draw_line(float2(0, 0), float2(1024, 1024));
		draw_line(float2(0, 1023), float2(1023, 0));
	}
}