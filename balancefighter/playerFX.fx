float fSize = 5.0f;

cbuffer cbPerFrame
{
    float3 gEyePosW;
};

bool gSpeculLevelbled;

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWVP;
};

cbuffer QuadVerts
{
    float3 quadPositions[4] = 
	{
        float3(-0.65f, 1.0f, 0.0f),
		float3(0.65f, 1.0f, 0.0f),
		float3(-0.65f, -1.0f, 0.0f),
		float3(0.65f, -1.0f, 0.0f),
	};
};

Texture2D gDiffuseMap;

SamplerState gTriLinearSam
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    //AddressU = Clamp;
	//AddressV = Clamp;
};

//BlendState blend
//{
//	BlendEnable[0] = TRUE;
//	SrcBlend = ONE;
//	DestBlend = ONE;
//};

BlendState blend
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_COLOR;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

struct VS_IN
{
    float3 posL			: POSITION;
    float3 directionL	: DIRECTION;
    float4 colour		: COLOUR;
	float fSize			: SIZE;
	float fRotAngle		: ANGLE;
};

struct VS_OUT
{
    float4 posH			: SV_POSITION;
    float3 posW			: POSITION;
    float3 directionW	: DIRECTION;
	float4 colour		: COLOUR;
	float fSize			: SIZE;
	float fRotAngle		: ANGLE;
};

VS_OUT VS(VS_IN vIn)
{
    VS_OUT vOut;
    // Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
    vOut.directionW = mul(float4(vIn.directionL, 0.0f), gWorld);
    // Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	vOut.fSize = vIn.fSize;
	vOut.colour = vIn.colour;
	vOut.fRotAngle = vIn.fRotAngle;

    return vOut;
}

//Geometry Shader
[maxvertexcount(4)]
void GS(point VS_OUT input[1], inout TriangleStream<VS_OUT> triangleStream)
{
    float2 Origin = float2(0.0f,0.0f);
    VS_OUT psInput;
    for (int i = 0; i < 4; i++)
	{
        //move the quad verts based on the pos
		psInput.posW.x = input[0].posW.x + (quadPositions[i].x) * input[0].fSize;
        psInput.posW.y = input[0].posW.y + (quadPositions[i].y) * input[0].fSize;
        psInput.posW.z = -1.0f;

        //multiply the new verts by the projection matrix
		psInput.posH = mul(float4(psInput.posW, 1.0), gWVP);
        psInput.directionW = input[0].directionW;
        psInput.colour = input[0].colour;
        psInput.fSize = input[0].fSize;
        psInput.fRotAngle = input[0].fRotAngle;
        //add this triangle to the triangle stream
		triangleStream.Append(psInput);
    }

	triangleStream.RestartStrip();
}



//Pixel shader
float4 PS(VS_OUT pIn) : SV_Target
{
	//float4 tempColor = {0.8f,0.2f,0.4f, 1.0f};
	
	// Interpolating normal can make it not be of unit length so normalize it.
    //float3 normalW = normalize(pIn.normalW);
	return (pIn.colour);
}

technique10 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader(gs_4_0, GS()) );
        //SetBlendState (blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}