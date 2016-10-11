//
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2005 - 2014 Media Design School
//
//	File Name	:	Lives.cpp
//	Description	:	Lives implementation file.
//	Mail		:	alex.burnie@mediadesign.school.nz
//
//Library Includes

//Local Includes
#include "Lives.h"
#include "Vertex.h"
#include "CServices.h"
#include "LevelVertex.h"

Lives::Lives()
	: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
	D3DXMatrixIdentity(&matTex); 
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP);
}

Lives::~Lives()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Lives::initShaders()
{
	buildFX();
	buildVertexLayouts();
}

void Lives::init(float Scale, D3DXVECTOR3 Movement, int iTextureVar )
{
	void* vpDevice;
	CServices::GetService(SERVICE_D3DDEVICE, vpDevice);
	md3dDevice = static_cast<ID3D10Device*>(vpDevice);

	if(iTextureVar == 1)
	{
		//Background Title
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
			L"Resources/Textures/jakelife.dds", 0, 0, &mDiffuseMapRV, 0 ));
	}
	else if(iTextureVar == 2)
	{
		//Highlight of Rope
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
			L"Resources/Textures/LifeAlex.dds", 0, 0, &mDiffuseMapRV, 0 ));
	}

	initShaders();

	mNumVertices = 4;

	// Create vertex buffer
	LevelVertex v[4];

		v[0] = LevelVertex(-1.0f, -1.0f , 0.0f, 0.0f, 1.0f);
		v[1] = LevelVertex(-1.0f, 1.0f , 0.0f , 0.0f, 0.0f);
		v[2] = LevelVertex(1.0f,  1.0f , 0.0f , 1.0f, 0.0f);
		v[3] = LevelVertex(1.0f,  -1.0f, 0.0f , 1.0f, 1.0f);


	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
	{
		v[i].posL *= Scale;
	}


	D3DXVECTOR3 TempVector;

	TempVector = Movement;

	for(unsigned int i = 0; i < mNumVertices; ++i)
	{
		v[i].posL.x += TempVector.x;
		v[i].posL.y += TempVector.y;
		v[i].posL.z += TempVector.z;
	}

	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(LevelVertex) * mNumVertices;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	DWORD i[6];

	i[0] = 0; i[1] =  1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * 6;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = i;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));

	
}

void Lives::draw()
{
	SetShader();

	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);

		//Usual draw calls
		UINT stride = sizeof(LevelVertex);
		UINT offset = 0;
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
		md3dDevice->DrawIndexed(6, 0, 0);
	}

}

void Lives::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"Resources/Shaders/levelFX.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("TexTech");

	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxTexMatVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

	CLogManager::GetInstance().LogEvent("Built Lives shaders");
}

void Lives::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

void Lives::SetShader()
{
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDevice->IASetInputLayout(mVertexLayout);
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set constants
	void* vpCam;
	CServices::GetService(SERVICE_CAMERA, vpCam);
	CCamera* pCam = static_cast<CCamera*>(vpCam);
	mView = pCam->GetViewMatrix();
	mProj = pCam->GetProjectionMatrix();

	mWVP = mWorld*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mWorld);
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);

	mfxTexMatVar->SetMatrix((float*)&matTex);
}

