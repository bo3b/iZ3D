//fxc.exe shaders.psh /Tvs_4_0 /EshaderS3D

struct VS_OUTPUT
{
	float2 tex : TEXCOORD0;
	float4 pos : SV_Position;
};

VS_OUTPUT shaderFullScreenQuadVS( uint id : SV_VertexID )
{
    VS_OUTPUT output = (VS_OUTPUT)0.0f;

    output.tex = float2( (id << 1) & 2, id & 2 );
    output.pos = float4( output.tex * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f) , 0.0f, 1.0f );

    return output;
}