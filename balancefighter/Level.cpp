#include "Level.h"
#include "LevelVertex.h"
#include "CServices.h"

CLevel::CLevel(void)
	: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
	D3DXMatrixIdentity(&matTex); 
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP);
}

CLevel::~CLevel(void)
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void CLevel::initShader()
{
	buildFX();
	buildVertexLayouts();
}

void CLevel::initVerts(float scale)
{
	void* vpDevice;
	CServices::GetService(SERVICE_D3DDEVICE, vpDevice);
	md3dDevice = static_cast<ID3D10Device*>(vpDevice);

	//HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	//L"Resources/Textures/grid.dds", 0, 0, &mDiffuseMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/BackDrop.dds", 0, 0, &mDiffuseMapRV, 0 ));

	initShader();

	mNumVertices = 4;

	// Create vertex buffer
    LevelVertex v[4];

	// Fill in the front face vertex data.
 	v[0] = LevelVertex(-1.0f,-1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = LevelVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = LevelVertex(1.0f, 1.0f,0.0f, 1.0f, 0.0f);
	v[3] = LevelVertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);

	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		v[i].posL *= Level_SIZE;

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

	CSoundManager::GetInstance().StartSoundtrack();
}

void CLevel::draw()
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


void CLevel::buildFX()
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
}

void CLevel::buildVertexLayouts()
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

void CLevel::SetShader()
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


void CLevel::initWorld(float _x, float _y)
{
	m_Gravity.Set(_x, _y);
	m_World = new b2World(m_Gravity);

	AddBarrier(0.0f, -30.0f, 500.0f, 10.0f);
	AddBarrier(-95.0f, 0.0f, 20.0f, 500.0f);
	AddBarrier(95.0f, 0.0f, 20.0f, 500.0f);
	AddBarrier(0.0f, 80.0f, 500.0f, 20.0f);

	/* ADD ROPE */
	
	/* LINK ROPE */

	/* DISTANCE JOINTS */

	CServices::ProvideService(SERVICE_WORLD, m_World);
}

void CLevel::AddBarrier(float posx, float posy, float w, float h)
{
	b2Vec2 Mid = b2Vec2(posx, posy);

	/*Ground Box temporary*/
	b2BodyDef TempBodyDef;
	TempBodyDef.type = b2_staticBody;
	TempBodyDef.position.Set(Mid.x, Mid.y);

	b2PolygonShape TempBox;
	TempBox.SetAsBox(w, h);
	b2Body* TempBody = m_World->CreateBody(&TempBodyDef);

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &TempBox;
	boxFixtureDef.density = 10;
	TempBody->CreateFixture(&boxFixtureDef);
}


void CLevel::Process(float _dt)
{
	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	float32 timeStep = 1.0f / 1200.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	// Instruct the world to perform a single step of simulation.
	// It is generally best to keep the time step and iterations fixed.
	m_World->Step(timeStep, velocityIterations, positionIterations);

	CServices::ProvideService(SERVICE_WORLD, m_World);
}