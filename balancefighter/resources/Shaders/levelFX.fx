float fSize = 1.0f;

cbuffer cbPerFrame
{
    float3 gEyePosW;
};

bool gSpeculLevelbled;

cbuffer cbPerObject
{
    float4x4 gWorld;
    float4x4 gWVP;
	float4x4 gTexMtx;
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
    float3 posL    : POSITION;
    float2 texC    : TEXCOORD;
};

struct VS_OUT
{
    float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float2 texC    : TEXCOORD;
};

VS_OUT VS(VS_IN vIn)
{
    VS_OUT vOut;
    // Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
    // Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
    // Output vertex attributes for interpolation across triangle.
	vOut.texC  = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);
	//Rupert!  Watch this jic
	//vOut.texC = vIn.texC;
    return vOut;
}

//Pixel shader
float4 PS(VS_OUT pIn) : SV_Target
{
    // Get materials from texture maps.
	float4 diffuse = gDiffuseMap.Sample( gTriLinearSam, pIn.texC);
	return (diffuse);
}

technique10 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        //SetBlendState (blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}