#include "PlayerManager.h"
#include "BalanceFighterApp.h"
#include "CServices.h"
#include "EMovments.h"
#include "EActions.h"
#include "EMovments.h"

const float m_fAttackTime = 1.0;

CPlayerManager::CPlayerManager(void):
	mFX(0), mTech(0), mfxWVPVar(0), mfxWorldVar(0), mfxEyePosVar(0),
	mVertexLayout(0), iNumTotalPlayers(0), m_uiNumStartLives(3)
{
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	mWorld._42 = 2.0f;

	m_fPlayer1AttackTimer = m_fAttackTime;
	m_fPlayer2AttackTimer = m_fAttackTime;

	m_uiNumStartLives = 3;
}

CPlayerManager::~CPlayerManager(void)
{
	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);

	for (int i = 0; i < static_cast<int>(vecPlayers.size()); ++i)
	{
		delete vecPlayers[i];
	}

	for (int i = 0; i < static_cast<int>(vecLivesPlayer1.size()); ++i)
	{
		delete vecLivesPlayer1[i];
		vecLivesPlayer1[i] = 0;
	}

	for (int i = 0; i < static_cast<int>(vecLivesPlayer1.size()); ++i)
	{
		delete vecLivesPlayer1[i];
		vecLivesPlayer1[i] = 0;
	}
}

void CPlayerManager::InitPlayerManager()
{
	void* vpDevice;
	CServices::GetService(SERVICE_D3DDEVICE, vpDevice);
	md3dDevice = static_cast<ID3D10Device*>(vpDevice);

	buildFX();
	buildVertexLayouts();

	PlayerTexSetup();

	//Create creatrues

	CPlayer* pPlayer1 = new CPlayer(EPLAYER_ANIMATION_STATE_STANDING);
	CPlayer* pPlayer2 = new CPlayer(EPLAYER_ANIMATION_STATE_STANDING);

	pPlayer1->SetPlayer(EPLAYER_CURRENT_CHARACTER_Jake);
	pPlayer2->SetPlayer(EPLAYER_CURRENT_CHARACTER_Alex);

	vecPlayers.push_back(pPlayer1);
	vecPlayers.push_back(pPlayer2);


	for (int i = 0; i < static_cast<int>(vecPlayers.size()); ++i)
	{
		vecPlayers[i]->init(md3dDevice, 1.0f);
		vecPlayers[i]->SetPlayerLives(m_uiNumStartLives);
	}

	vecPlayers[0]->SetOrigin(D3DXVECTOR3(-50.0f, 10.0f, -0.1f));
	vecPlayers[0]->SetPos(D3DXVECTOR3(-50.0f, 10.0f, -0.1f));
	vecPlayers[0]->UpdateAll();
	vecPlayers[1]->SetOrigin(D3DXVECTOR3(50.0f, 10.0f, 0.0f));
	vecPlayers[1]->SetPos(D3DXVECTOR3(50.0f, 10.0f, 0.0f));
	vecPlayers[1]->UpdateAll();

	for (int i = 0; i < m_uiNumStartLives; i++)
	{
		Lives* pLivesPlayer1 = new Lives();
		Lives* pLivesPlayer2 = new Lives();

		vecLivesPlayer1.push_back(pLivesPlayer1);
		vecLivesPlayer2.push_back(pLivesPlayer2);
	}

	float start1 = 10.0f;
	float start2 = -40.0f;

	for (int i = 0; i < static_cast<int>(vecLivesPlayer1.size()); ++i)
	{
		vecLivesPlayer1[i]->init(3.0f,D3DXVECTOR3(start2, 35.0f,-5.0f),1);
		start2 = start2 + 13.0f;
	}

	for (int i = 0; i < static_cast<int>(vecLivesPlayer2.size()); ++i)
	{
		vecLivesPlayer2[i]->init(3.0f,D3DXVECTOR3(start1, 35.0f,-5.0f),2);
		start1 = start1 + 13.0f;
	}

}

void CPlayerManager::Draw()
{
	for (int i = 0; i < static_cast<int>(vecLivesPlayer1.size()); ++i)
	{
		vecLivesPlayer1[i]->draw();
	}

	for (int i = 0; i < static_cast<int>(vecLivesPlayer2.size()); ++i)
	{
		vecLivesPlayer2[i]->draw();
	}

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

	D3D10_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
	rsDesc.CullMode = D3D10_CULL_NONE;
	md3dDevice->CreateRasterizerState(&rsDesc, &mRasState);
	md3dDevice->RSSetState(mRasState);

    md3dDevice->IASetInputLayout(mVertexLayout);
    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	// set constants
	void* vpCam;
	CServices::GetService(SERVICE_CAMERA, vpCam);
	CCamera* pCam = static_cast<CCamera*>(vpCam);
	mView = pCam->GetViewMatrix();
	mProj = pCam->GetProjectionMatrix();

	mWVP = mWorld*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mWorld);

    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
        
		//Draw all creatures
		for (int i = 0; i < static_cast<int>(vecPlayers.size()); ++i)
		{
			vecPlayers[i]->draw();
		}
    }

	pCam = nullptr;
}

void CPlayerManager::Process(float _dt)
{
	if (vecPlayers[0]->GetNumLives() < static_cast<unsigned int>(vecLivesPlayer1.size()))
	{
		vecLivesPlayer1.erase(vecLivesPlayer1.end() - 1);
	}

	if (vecPlayers[1]->GetNumLives() < static_cast<unsigned int>(vecLivesPlayer2.size()))
	{
		vecLivesPlayer2.erase(vecLivesPlayer2.end() - 1);
	}


	CSoundManager::GetInstance().m_pSystem->update();

	//Set the facing directions of the players
	if(vecPlayers[0]->GetPos().x > vecPlayers[1]->GetPos().x)
	{
		//Face correct way 
		//player1 face left 
		//player2 face right
		vecPlayers[0]->SetRightFacing(false);
		vecPlayers[1]->SetRightFacing(true);
	}
	else
	{
		//player1 face right 
		//player2 face left
		vecPlayers[0]->SetRightFacing(true);
		vecPlayers[1]->SetRightFacing(false);
	}

	//Attack timer
	m_fPlayer1AttackTimer += _dt;
	m_fPlayer2AttackTimer += _dt;
	CheckAttackConditions();

	bool bResetTrigger = false;

	for (unsigned i = 0; i < vecPlayers.size(); ++i)
	{	
		/* Get Controller Input */
		CXInput::GetInstance().CheckInput(i);

		e_Movement eMove = CXInput::GetInstance().GetInput();
		float fBalance = CXInput::GetInstance().GetBalance();
		e_Action bAttack = CXInput::GetInstance().GetAction();

		/* Update Movement and Balance */
		vecPlayers[i]->Move(eMove);
		vecPlayers[i]->Jump(CXInput::GetInstance().GetJump());
		vecPlayers[i]->Balance(fBalance);

		

		/* Attack */
		if(bAttack == BUTTON_A && vecPlayers[i]->GetAttackAllowState())
		{
			vecPlayers[i]->SetAttackAnim();

			if (i == 0)
			{
				CSoundManager::GetInstance().PlayLoadedSound(PlayerTwo, HitTwo);
			}
			else
			{
				CSoundManager::GetInstance().PlayLoadedSound(PlayerOne, HitTwo);
			}

			if(b2Distance(vecPlayers[0]->Getb2Pos(), vecPlayers[1]->Getb2Pos()) < 10)
			{
				float fAttForce = 400.0f;

				if(i == 0)
				{
					if(vecPlayers[0]->GetPos().x > vecPlayers[1]->GetPos().x)
					{
						fAttForce = -fAttForce;
					}
					vecPlayers[1]->GetBody()->ApplyLinearImpulse( b2Vec2(fAttForce, 0.0f), vecPlayers[0]->GetBody()->GetWorldCenter(), true);
					vecPlayers[1]->SetAnimStateHurt();
					CSoundManager::GetInstance().PlayLoadedSound(PlayerOne, HitOne);
				}
				else
				{
					if(vecPlayers[0]->GetPos().x < vecPlayers[1]->GetPos().x)
					{
						fAttForce = -fAttForce;
					}
					vecPlayers[0]->GetBody()->ApplyLinearImpulse( b2Vec2(fAttForce, 0.0f), vecPlayers[0]->GetBody()->GetWorldCenter(), true);
					vecPlayers[0]->SetAnimStateHurt();
					CSoundManager::GetInstance().PlayLoadedSound(PlayerTwo, HitOne);
				}
			}
			//Reset timer
			switch (i)
			{
			case 0:
				{
					m_fPlayer1AttackTimer = 0.0f;
				}
				break;
			case 1:
				{
					m_fPlayer2AttackTimer = 0.0f;
				}
				break;
			}
			CheckAttackConditions();
		}
		/* Process the output and update renderer*/
		vecPlayers[i]->Process(_dt);
		vecPlayers[i]->UpdateAll();
		if (!vecPlayers[i]->IsAlive())
		{
			vecPlayers[i]->SetPlayerLives(vecPlayers[i]->GetNumLives() - 1);
			bResetTrigger = true;
		}
	}	

	if (bResetTrigger)
	{
		ResetLevel();
	}
}

void CPlayerManager::BuildPhysicalBodies()
{
	for(unsigned i = 0; i < vecPlayers.size(); i++)
	{
		vecPlayers[i]->SetBody();
	}
}

void CPlayerManager::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"Resources/Shaders/playerFX.fx", 0, 0, 
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
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");

	//Player Textures
	//Jake
	mfxDiffuse_Jake_Attack_1Var	 = mFX->GetVariableByName("gJake_Attack1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Attack_2Var	 = mFX->GetVariableByName("gJake_Attack2_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Attack_3Var	 = mFX->GetVariableByName("gJake_Attack3_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Jump_1Var	 = mFX->GetVariableByName("gJake_Jump1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Jump_2Var	 = mFX->GetVariableByName("gJake_Jump2_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Block_1Var	 = mFX->GetVariableByName("gJake_Block1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Fall_1Var	 = mFX->GetVariableByName("gJake_Fall1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Fall_2Var	 = mFX->GetVariableByName("gJake_Fall2_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Fall_3Var	 = mFX->GetVariableByName("gJake_Fall3_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Fall_4Var	 = mFX->GetVariableByName("gJake_Fall4_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_HurtVar		 = mFX->GetVariableByName("gJake_Hurt1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Stand_1Var	 = mFX->GetVariableByName("gJake_Stand1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Walk_1Var	 = mFX->GetVariableByName("gJake_Walk1_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Walk_2Var	 = mFX->GetVariableByName("gJake_Walk2_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Walk_3Var	 = mFX->GetVariableByName("gJake_Walk3_DifMap")->AsShaderResource();
	mfxDiffuse_Jake_Walk_4Var	 = mFX->GetVariableByName("gJake_Walk4_DifMap")->AsShaderResource();
	//mfxDiffuse_Jake_SelectVar	 = mFX->GetVariableByName("gJake_Select1_DifMap")->AsShaderResource();
	//mfxDiffuse_Jake_Whoopsie_Var = mFX->GetVariableByName("gJake_Whoopsie_DifMap")->AsShaderResource();

	//Alex
	mfxDiffuse_Alex_Attack_1Var	 = mFX->GetVariableByName("gAlex_Attack1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Attack_2Var	 = mFX->GetVariableByName("gAlex_Attack2_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Attack_3Var	 = mFX->GetVariableByName("gAlex_Attack3_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Jump_1Var	 = mFX->GetVariableByName("gAlex_Jump1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Jump_2Var	 = mFX->GetVariableByName("gAlex_Jump2_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Block_1Var	 = mFX->GetVariableByName("gAlex_Block1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Fall_1Var	 = mFX->GetVariableByName("gAlex_Fall1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Fall_2Var	 = mFX->GetVariableByName("gAlex_Fall2_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Fall_3Var	 = mFX->GetVariableByName("gAlex_Fall3_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Fall_4Var	 = mFX->GetVariableByName("gAlex_Fall4_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_HurtVar		 = mFX->GetVariableByName("gAlex_Hurt1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Stand_1Var	 = mFX->GetVariableByName("gAlex_Stand1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Walk_1Var	 = mFX->GetVariableByName("gAlex_Walk1_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Walk_2Var	 = mFX->GetVariableByName("gAlex_Walk2_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Walk_3Var	 = mFX->GetVariableByName("gAlex_Walk3_DifMap")->AsShaderResource();
	mfxDiffuse_Alex_Walk_4Var	 = mFX->GetVariableByName("gAlex_Walk4_DifMap")->AsShaderResource();
	//mfxDiffuse_Alex_SelectVar	 = mFX->GetVariableByName("gAlex_Select1_DifMap")->AsShaderResource();
	//mfxDiffuse_Alex_Whoopsie_Var = mFX->GetVariableByName("gAlex_Whoopsie_DifMap")->AsShaderResource();

	//Metre + Arrow Textures
	mfxDiffuseBalanceMetreVar = mFX->GetVariableByName("gBalanceMetreDifMap")->AsShaderResource();
	mfxDiffuseBalanceArrowVar = mFX->GetVariableByName("gBalanceArrowDifMap")->AsShaderResource();
}

void CPlayerManager::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIRECTION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE", 0, DXGI_FORMAT_R32_FLOAT,    0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXINDEX", 0, DXGI_FORMAT_R32_UINT,    0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"BALANCESTATUS", 0, DXGI_FORMAT_R32_FLOAT,    0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"FACINGDIRECTION", 0, DXGI_FORMAT_R32_SINT,    0, 44, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 7, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

void CPlayerManager::PlayerTexSetup()
{
	LoadJake();
	LoadAlex();
	
	//Metre
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/BalanceMetre.png", 0, 0, &mDiffuseBalanceMetreRV, 0 ));
	mfxDiffuseBalanceMetreVar->SetResource(mDiffuseBalanceMetreRV);
	
	//Arrow
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Balance-Arrow.png", 0, 0, &mDiffuseBalanceArrowRV, 0 ));
	mfxDiffuseBalanceArrowVar->SetResource(mDiffuseBalanceArrowRV);
}

void CPlayerManager::LoadJake()
{
	//Attacks
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Attack_1.png", 0, 0, &mDiffuse_Jake_Attack_1RV, 0 ));
	mfxDiffuse_Jake_Attack_1Var->SetResource(mDiffuse_Jake_Attack_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Attack_2.png", 0, 0, &mDiffuse_Jake_Attack_2RV, 0 ));
	mfxDiffuse_Jake_Attack_2Var->SetResource(mDiffuse_Jake_Attack_2RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Attack_3.png", 0, 0, &mDiffuse_Jake_Attack_3RV, 0 ));
	mfxDiffuse_Jake_Attack_3Var->SetResource(mDiffuse_Jake_Attack_3RV);

	//Jumping
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Jump_1.png", 0, 0, &mDiffuse_Jake_Jump_1RV, 0 ));
	mfxDiffuse_Jake_Jump_1Var->SetResource(mDiffuse_Jake_Jump_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Jump_2.png", 0, 0, &mDiffuse_Jake_Jump_2RV, 0 ));
	mfxDiffuse_Jake_Jump_2Var->SetResource(mDiffuse_Jake_Jump_2RV);

	//Block 
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Block_1.png", 0, 0, &mDiffuse_Jake_Block_1RV, 0 ));
	mfxDiffuse_Jake_Block_1Var->SetResource(mDiffuse_Jake_Block_1RV);
	
	//Fall
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Fall_1.png", 0, 0, &mDiffuse_Jake_Fall_1RV, 0 ));
	mfxDiffuse_Jake_Fall_1Var->SetResource(mDiffuse_Jake_Fall_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Fall_2.png", 0, 0, &mDiffuse_Jake_Fall_2RV, 0 ));
	mfxDiffuse_Jake_Fall_2Var->SetResource(mDiffuse_Jake_Fall_2RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Fall_3.png", 0, 0, &mDiffuse_Jake_Fall_3RV, 0 ));
	mfxDiffuse_Jake_Fall_3Var->SetResource(mDiffuse_Jake_Fall_3RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Fall_4.png", 0, 0, &mDiffuse_Jake_Fall_4RV, 0 ));
	mfxDiffuse_Jake_Fall_4Var->SetResource(mDiffuse_Jake_Fall_4RV);

	//Hurt
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Hurt.png", 0, 0, &mDiffuse_Jake_HurtRV, 0 ));
	mfxDiffuse_Jake_HurtVar->SetResource(mDiffuse_Jake_HurtRV);

	//Stand
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Stand_1.png", 0, 0, &mDiffuse_Jake_Stand_1RV, 0 ));
	mfxDiffuse_Jake_Stand_1Var->SetResource(mDiffuse_Jake_Stand_1RV);

	//Walk2
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Walk_1.png", 0, 0, &mDiffuse_Jake_Walk_1RV, 0 ));
	mfxDiffuse_Jake_Walk_1Var->SetResource(mDiffuse_Jake_Walk_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Walk_2.png", 0, 0, &mDiffuse_Jake_Walk_2RV, 0 ));
	mfxDiffuse_Jake_Walk_2Var->SetResource(mDiffuse_Jake_Walk_2RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Walk_3.png", 0, 0, &mDiffuse_Jake_Walk_3RV, 0 ));
	mfxDiffuse_Jake_Walk_3Var->SetResource(mDiffuse_Jake_Walk_3RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/SW/Walk_4.png", 0, 0, &mDiffuse_Jake_Walk_4RV, 0 ));
	mfxDiffuse_Jake_Walk_4Var->SetResource(mDiffuse_Jake_Walk_4RV);

}

void CPlayerManager::LoadAlex()
{
	//Attacks
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Attack_1.png", 0, 0, &mDiffuse_Alex_Attack_1RV, 0 ));
	mfxDiffuse_Alex_Attack_1Var->SetResource(mDiffuse_Alex_Attack_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Attack_2.png", 0, 0, &mDiffuse_Alex_Attack_2RV, 0 ));
	mfxDiffuse_Alex_Attack_2Var->SetResource(mDiffuse_Alex_Attack_2RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Attack_3.png", 0, 0, &mDiffuse_Alex_Attack_3RV, 0 ));
	mfxDiffuse_Alex_Attack_3Var->SetResource(mDiffuse_Alex_Attack_3RV);

	//Jumping
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Jump_1.png", 0, 0, &mDiffuse_Alex_Jump_1RV, 0 ));
	mfxDiffuse_Alex_Jump_1Var->SetResource(mDiffuse_Alex_Jump_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Jump_2.png", 0, 0, &mDiffuse_Alex_Jump_2RV, 0 ));
	mfxDiffuse_Alex_Jump_2Var->SetResource(mDiffuse_Alex_Jump_2RV);

	//Block 
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Block_1.png", 0, 0, &mDiffuse_Alex_Block_1RV, 0 ));
	mfxDiffuse_Alex_Block_1Var->SetResource(mDiffuse_Alex_Block_1RV);
	
	//Fall
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Fall_1.png", 0, 0, &mDiffuse_Alex_Fall_1RV, 0 ));
	mfxDiffuse_Alex_Fall_1Var->SetResource(mDiffuse_Alex_Fall_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Fall_2.png", 0, 0, &mDiffuse_Alex_Fall_2RV, 0 ));
	mfxDiffuse_Alex_Fall_2Var->SetResource(mDiffuse_Alex_Fall_2RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Fall_3.png", 0, 0, &mDiffuse_Alex_Fall_3RV, 0 ));
	mfxDiffuse_Alex_Fall_3Var->SetResource(mDiffuse_Alex_Fall_3RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Fall_4.png", 0, 0, &mDiffuse_Alex_Fall_4RV, 0 ));
	mfxDiffuse_Alex_Fall_4Var->SetResource(mDiffuse_Alex_Fall_4RV);

	//Hurt
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Hurt.png", 0, 0, &mDiffuse_Alex_HurtRV, 0 ));
	mfxDiffuse_Alex_HurtVar->SetResource(mDiffuse_Alex_HurtRV);

	//Stand
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Stand_1.png", 0, 0, &mDiffuse_Alex_Stand_1RV, 0 ));
	mfxDiffuse_Alex_Stand_1Var->SetResource(mDiffuse_Alex_Stand_1RV);

	//Walk2
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Walk_1.png", 0, 0, &mDiffuse_Alex_Walk_1RV, 0 ));
	mfxDiffuse_Alex_Walk_1Var->SetResource(mDiffuse_Alex_Walk_1RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Walk_2.png", 0, 0, &mDiffuse_Alex_Walk_2RV, 0 ));
	mfxDiffuse_Alex_Walk_2Var->SetResource(mDiffuse_Alex_Walk_2RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Walk_3.png", 0, 0, &mDiffuse_Alex_Walk_3RV, 0 ));
	mfxDiffuse_Alex_Walk_3Var->SetResource(mDiffuse_Alex_Walk_3RV);
		HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
	L"Resources/Textures/Viper/Walk_4.png", 0, 0, &mDiffuse_Alex_Walk_4RV, 0 ));
	mfxDiffuse_Alex_Walk_4Var->SetResource(mDiffuse_Alex_Walk_4RV);
}

void CPlayerManager::CheckAttackConditions()
{
	if (m_fPlayer1AttackTimer > m_fAttackTime)
	{
		vecPlayers[0]->SetAttackAllow(true);
	}
	else
	{
		vecPlayers[0]->SetAttackAllow(false);
	}

	if (m_fPlayer2AttackTimer > m_fAttackTime)
	{
		vecPlayers[1]->SetAttackAllow(true);
	}
	else
	{
		vecPlayers[1]->SetAttackAllow(false);
	}
}

void CPlayerManager::ResetLevel()
{
	//this->InitPlayerManager();
	vecPlayers[0]->Reset();
	vecPlayers[1]->Reset();
}

D3DXVECTOR3 CPlayerManager::GetCentrePoint()
{
	float x = ((vecPlayers[0]->Getb2Pos().x + vecPlayers[1]->Getb2Pos().x) / 2);

	return D3DXVECTOR3(x, 0.0f, 0.0f);
}

float CPlayerManager::GetDistance()
{
	return (sqrt((vecPlayers[0]->Getb2Pos().x - vecPlayers[1]->Getb2Pos().x) * (vecPlayers[0]->Getb2Pos().x - vecPlayers[1]->Getb2Pos().x)));
}

bool CPlayerManager::GameOver()
{
	for (int i = 0; i < static_cast<int>(vecPlayers.size()); i++)
	{
		if (vecPlayers[i]->GetNumLives() <= 0)
		{
			return true;
		}
	}
	return false;
}