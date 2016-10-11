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
        float3(-0.80f, 1.0f, 0.0f),
		float3(0.80f, 1.0f, 0.0f),
		float3(-0.80f, -1.0f, 0.0f),
		float3(0.80f, -1.0f, 0.0f),
	};

	float3 quadBalMetrePos[4] = 
	{
        float3(-1.2f, 0.15f, 0.0f),
		float3(1.2f, 0.15f, 0.0f),
		float3(-1.2f, -0.15f, 0.0f),
		float3(1.2f, -0.15f, 0.0f),
	};

	float3 quadBalArrowPos[4] = 
	{
        float3(-0.1f, 0.25f, 0.0f),
		float3(0.1f, 0.25f, 0.0f),
		float3(-0.1f, -0.25f, 0.0f),
		float3(0.1f, -0.25f, 0.0f),
	};
};

//Players JAKE
Texture2D gJake_Attack1_DifMap;
Texture2D gJake_Attack2_DifMap;
Texture2D gJake_Attack3_DifMap;
Texture2D gJake_Block1_DifMap;
Texture2D gJake_Fall1_DifMap;
Texture2D gJake_Fall2_DifMap;
Texture2D gJake_Fall3_DifMap;
Texture2D gJake_Fall4_DifMap;
Texture2D gJake_Hurt1_DifMap;
Texture2D gJake_Jump1_DifMap;
Texture2D gJake_Jump2_DifMap;
Texture2D gJake_Stand1_DifMap;
Texture2D gJake_Walk1_DifMap;
Texture2D gJake_Walk2_DifMap;
Texture2D gJake_Walk3_DifMap;
Texture2D gJake_Walk4_DifMap;

//Texture2D gJake_Select1_DifMap;
//Texture2D gJake_Whoopsie_DifMap;

//Players ALEX
Texture2D gAlex_Attack1_DifMap;
Texture2D gAlex_Attack2_DifMap;
Texture2D gAlex_Attack3_DifMap;
Texture2D gAlex_Block1_DifMap;
Texture2D gAlex_Fall1_DifMap;
Texture2D gAlex_Fall2_DifMap;
Texture2D gAlex_Fall3_DifMap;
Texture2D gAlex_Fall4_DifMap;
Texture2D gAlex_Hurt1_DifMap;
Texture2D gAlex_Jump1_DifMap;
Texture2D gAlex_Jump2_DifMap;
Texture2D gAlex_Stand1_DifMap;
Texture2D gAlex_Walk1_DifMap;
Texture2D gAlex_Walk2_DifMap;
Texture2D gAlex_Walk3_DifMap;
Texture2D gAlex_Walk4_DifMap;

//Texture2D gAlex_Whoopsie_DifMap;
//Texture2D gAlex_Select1_DifMap;

//Metre + Arrow
Texture2D gBalanceMetreDifMap;
Texture2D gBalanceArrowDifMap;


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
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

struct VS_IN
{
    float3 posL				: POSITION;
    float3 directionL		: DIRECTION;
    float2 textureUV		: TEXTURE;
	float fSize				: SIZE;
	unsigned int uiTexIndex	: TEXINDEX;
	float fBalStatus		: BALANCESTATUS;
	int iFacingDirection	: FACINGDIRECTION;
};

struct VS_OUT
{
    float4 posH				: SV_POSITION;
    float3 posW				: POSITION;
    float3 directionW		: DIRECTION;
	float2 textureUV		: TEXTURE;
	float fSize				: SIZE;
	unsigned int uiTexIndex	: TEXINDEX;
	float fBalStatus		: BALANCESTATUS;
	int iFacingDirection	: FACINGDIRECTION;
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
	vOut.textureUV = vIn.textureUV;
	vOut.uiTexIndex = vIn.uiTexIndex;
	vOut.fBalStatus = vIn.fBalStatus;
	vOut.iFacingDirection = vIn.iFacingDirection;
    return vOut;
}

//Geometry Shader
[maxvertexcount(12)]
void GS(point VS_OUT input[1], inout TriangleStream<VS_OUT> triangleStream)
{
	float2 texturesUV[4];
	texturesUV[0] = float2(0.0f, 0.0f);
	texturesUV[2] = float2(0.0f, 1.0f);
	texturesUV[1] = float2(1.0f, 0.0f);
	texturesUV[3] = float2(1.0f, 1.0f);

    float2 Origin = float2(0.0f,0.0f);
    VS_OUT psInput;
    for (int i = 0; i < 4; i++)
	{
		//make sure alpha works
		if (input[0].uiTexIndex > 50)
		{
			psInput.posW.z = -1.1f;
		}
		else
		{
			psInput.posW.z = -1.0f;
		}
        //move the quad verts based on the pos
		psInput.posW.x = input[0].posW.x + (quadPositions[i].x) * input[0].fSize;
        psInput.posW.y = input[0].posW.y + (quadPositions[i].y) * input[0].fSize;
		psInput.textureUV = texturesUV[i];
		psInput.iFacingDirection = input[0].iFacingDirection;

		//Change Facing Direction
		psInput.textureUV.x = (texturesUV[i].x * input[0].iFacingDirection);

        //multiply the new verts by the projection matrix
		psInput.posH = mul(float4(psInput.posW, 1.0), gWVP);
        psInput.directionW = input[0].directionW;
        psInput.fSize = input[0].fSize;
		psInput.uiTexIndex = input[0].uiTexIndex;
		psInput.fBalStatus = input[0].fBalStatus;
        //add this triangle to the triangle stream
		triangleStream.Append(psInput);
    }

	triangleStream.RestartStrip();

	for (int j = 0; j < 4; j++)
	{
        //move the quad verts based on the pos
		psInput.posW.x = input[0].posW.x + (quadBalMetrePos[j].x) * input[0].fSize;
        psInput.posW.y = input[0].posW.y + (quadBalMetrePos[j].y) * input[0].fSize + 10.0f; //Above player
        psInput.posW.z = -1.01f;
		psInput.textureUV = texturesUV[j];
		psInput.iFacingDirection = input[0].iFacingDirection;

        //multiply the new verts by the projection matrix
		psInput.posH = mul(float4(psInput.posW, 1.0), gWVP);
        psInput.directionW = input[0].directionW;
        psInput.fSize = input[0].fSize;
		psInput.uiTexIndex = 100;
		psInput.fBalStatus = input[0].fBalStatus;
        //add this triangle to the triangle stream
		triangleStream.Append(psInput);
    }

	triangleStream.RestartStrip();

	for (int j = 0; j < 4; j++)
	{
        //move the quad verts based on the pos
		psInput.posW.x = input[0].posW.x + (quadBalArrowPos[j].x) * input[0].fSize + (8.0f * input[0].fBalStatus);
        psInput.posW.y = input[0].posW.y + (quadBalArrowPos[j].y) * input[0].fSize + 10.0f; //Above player
        psInput.posW.z = -1.02f;
		psInput.textureUV = texturesUV[j];
		psInput.iFacingDirection = input[0].iFacingDirection;

        //multiply the new verts by the projection matrix
		psInput.posH = mul(float4(psInput.posW, 1.0), gWVP);
        psInput.directionW = input[0].directionW;
        psInput.fSize = input[0].fSize;
		psInput.uiTexIndex = 101;
		psInput.fBalStatus = input[0].fBalStatus;
        //add this triangle to the triangle stream
		triangleStream.Append(psInput);
    }

	triangleStream.RestartStrip();
}



//Pixel shader
float4 PS(VS_OUT pIn) : SV_Target
{
	//float4 tempColor = {0.8f,0.2f,0.4f, 1.0f};

	float4 diffuse;
	if (pIn.uiTexIndex < 50)
	{
		switch (pIn.uiTexIndex)
		{
			case 0:
			{
				diffuse = gJake_Attack1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 1:
			{
				diffuse = gJake_Attack2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 2:
			{
				diffuse = gJake_Attack3_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 3:
			{
				diffuse = gJake_Block1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 4:
			{
				diffuse = gJake_Fall1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
			case 5:
			{
				diffuse = gJake_Fall2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 6:
			{
				diffuse = gJake_Fall3_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 7:
			{
				diffuse = gJake_Fall4_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 8:
			{
				diffuse = gJake_Hurt1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 9:
			{
				diffuse = gJake_Jump1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
			case 10:
			{
				diffuse = gJake_Jump2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 11:
			{
				diffuse = gJake_Stand1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 12:
			{
				diffuse = gJake_Walk1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 13:
			{
				diffuse = gJake_Walk2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 14:
			{
				diffuse = gJake_Walk3_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
			case 15:
			{
				diffuse = gJake_Walk4_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			break;

			default:
			{

			}
			break;
		};
	}
	else if (pIn.uiTexIndex < 100)
	{
		switch (pIn.uiTexIndex)
		{
			case 50:
			{
				diffuse = gAlex_Attack1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 51:
			{
				diffuse = gAlex_Attack2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 52:
			{
				diffuse = gAlex_Attack3_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 53:
			{
				diffuse = gAlex_Block1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 54:
			{
				diffuse = gAlex_Fall1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
			case 55:
			{
				diffuse = gAlex_Fall2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 56:
			{
				diffuse = gAlex_Fall3_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 57:
			{
				diffuse = gAlex_Fall4_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 58:
			{
				diffuse = gAlex_Hurt1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 59:
			{
				diffuse = gAlex_Jump1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
			case 60:
			{
				diffuse = gAlex_Jump2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 61:
			{
				diffuse = gAlex_Stand1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			case 62:
			{
				diffuse = gAlex_Walk1_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 63:
			{
				diffuse = gAlex_Walk2_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 64:
			{
				diffuse = gAlex_Walk3_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
			case 65:
			{
				diffuse = gAlex_Walk4_DifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;
		
			break;

			default:
			{

			}
			break;
		};
	}
	else
	{
			switch (pIn.uiTexIndex)
		{
			case 100:
			{
				diffuse = gBalanceMetreDifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			case 101:
			{
				diffuse = gBalanceArrowDifMap.Sample( gTriLinearSam, pIn.textureUV );
			}
			break;

			default:
			{

			}
			break;
		};
	}

	return (diffuse);
}

technique10 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader(gs_4_0, GS()) );
        SetBlendState (blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}