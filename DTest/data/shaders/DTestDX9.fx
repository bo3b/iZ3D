float4x4 View;
float4x4 Projection;
float4x4 World;

float3 Eye;
float3 LightPos;
float4 Diffuse;
float4 Specular;

struct VS_INPUT
{
    float3 Pos		: POSITION;
    float3 Norm		: NORMAL;        
    float2 Tex		: TEXCOORD0; 
};

struct PS_INPUT
{
    float4 Pos		: SV_POSITION;
	float3 WorldPos : TEXCOORD0;
    float3 Norm		: TEXCOORD1;
    float2 Tex		: TEXCOORD2;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
	
    output.Pos		= mul( float4(input.Pos, 1.0), World );
    output.WorldPos = output.Pos.xyz;
    output.Pos		= mul( output.Pos, View );
    output.Pos		= mul( output.Pos, Projection );
    output.Norm		= mul( input.Norm, World );
    output.Tex		= input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : COLOR0
{
	float3 lightDir = normalize(input.WorldPos - LightPos);
	float3 halfVec  = normalize(normalize(Eye - input.WorldPos) - lightDir);

	float diffuseLighting  = saturate(dot(input.Norm, -lightDir));
    float specularLighting = pow(saturate(dot(halfVec, input.Norm)), 3.0);

    return Diffuse * diffuseLighting + Specular * specularLighting;
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique Render
{
    pass P0
    {
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}

