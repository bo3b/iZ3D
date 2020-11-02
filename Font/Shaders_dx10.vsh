matrix 				tmView;

struct VS_Input
{
	float3	m_Pos	:	POSITION;
	int4  	m_Blend :	BLENDINDICES;
	float2	m_Tex 	: 	TEXCOORD0;
};

struct PS_Input
{
	float4	m_Pos	:	SV_POSITION;
    	float2	m_Tex 	: 	TEXCOORD0;
	//int4	m_Blend	:	TEXCOORD1;
};

//------------------------------------------------------------------------
// Name: VSFont
// Desc: 
//------------------------------------------------------------------------
PS_Input VSFont( VS_Input _In )
{

 	PS_Input Out;

	//Out.m_Blend 	= _In.m_Blend;
	//Out.m_Pos	= float4( _In.m_Pos, 1.0f );

	Out.m_Tex 	= _In.m_Tex;
	Out.m_Pos 	= mul( float4( _In.m_Pos, 1.0f ), tmView );	

	return Out;
 
};