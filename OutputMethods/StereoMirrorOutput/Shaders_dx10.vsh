//fxc.exe shaders.psh /Tvs_4_0 /EshaderS3D

float4  g_InvertUV = { 0.0f, 0.0f, 0.0f, 0.0f };

struct VS_OUTPUT
{
	float2 tex1	: TEXCOORD0;
	float2 tex2	: TEXCOORD1;
	float4 pos	: SV_Position;
};

VS_OUTPUT shaderFullScreenQuadVS( uint id : SV_VertexID )
{
	VS_OUTPUT o	= ( VS_OUTPUT )0.0f;
	o.tex1		= float2( (id << 1) & 2, id & 2 );
	o.pos		= float4( o.tex1 * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f ) , 0.0f, 1.0f );
	o.tex2		= o.tex1;
	
	if ( g_InvertUV.x > 0.0f )
		o.tex2.x	= 1.0f - o.tex2.x;
	if ( g_InvertUV.y > 0.0f )
		o.tex2.y	= 1.0f - o.tex2.y;
	if ( g_InvertUV.z > 0.0f )
		o.tex1.x	= 1.0f - o.tex1.x;
	if ( g_InvertUV.w > 0.0f )
		o.tex1.y	= 1.0f - o.tex1.y;

	return o;
}