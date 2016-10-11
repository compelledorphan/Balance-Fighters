//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2014 Media Design School
//
// File Name: BalanceFighterApp.cpp
// Description: Base code file for entire game
// Author: Team SnakePit
// Email: liam.simmons@mediadesign.school.nz, jacob.dodunski@mediadesign.school.nz, 
//			rupert.taavao@mediadesign.school.nz, alex.burnie@mediadesign.school.nz
//

// Library Includes
//#include <vld.h>

// Local Includes
#include "CServices.h"

// This Include
#include "BalanceFighterApp.h"

// Implementation

//****************************************************************
//
// @name: WinMain
// @description: Start location for game
// @author: Team SnakePit
// @parameter: HINSTANCE, HISNTANCE, LPSTR, int
// @return: int
// 
//****************************************************************
int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _PrevInstance,
				   LPSTR _lpCmdLine, int _iShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	LPWSTR* pszArgList;
	int iArgCount;
	bool bDebugMode = false;
	// Following code is parsing the commandline arguments for the tag -debug
	pszArgList = CommandLineToArgvW(GetCommandLine(), &iArgCount);
	if (pszArgList == NULL)
	{
		for (int i = 0; i < iArgCount; i++)
		{
			if (pszArgList[i] == L"-debug")
			{
				bDebugMode = true;
			}
			else
			{
				bDebugMode = false;
			}
		}
	}

	LocalFree(pszArgList);

	CLogManager& rLogManager = CLogManager::GetInstance();
	CLogManager::GetInstance().Initialise(bDebugMode);

	CSoundManager& rSoundManager = CSoundManager::GetInstance();

	BalanceFighterApp TheApp(_hInstance);
	
	TheApp.InitApp();

	return TheApp.run();
}

//****************************************************************
//
// @name: BalanceFighterApp::BalanceFighterApp
// @description: Constructer
// @author: Team SnakePit
// @parameter: HINSTANCE
// @return: /
// 
//****************************************************************
BalanceFighterApp::BalanceFighterApp(HINSTANCE _hInstance) :
	D3DApp(_hInstance),
	m_vecEyePos(0.0f, 10.0f, 0.0f), 
	m_fRadius(5.0f), 
	m_fTheta(0.0f), 
	m_fPhi(PI*0.4f), 
	m_eCurrentGameState(EGAME_STATE_MAINMENU)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matWVP); 

	m_pCamera = new CCamera();

	//TODO: Fix Temp break if not controller inserted
	assert(CXInput::GetInstance().IsConnected());

	m_iMenu = 0;
	m_iOptions = 0;
	m_bPause = false;

}

//****************************************************************
//
// @name: BalanceFighterApp::~BalanceFighterApp
// @description: Deconstructor
// @author: Team SnakePit
// @parameter: HINSTANCE
// @return: /
// 
//****************************************************************
BalanceFighterApp::~BalanceFighterApp()
{
	if (md3dDevice)
	{
		md3dDevice->ClearState();
	}
	else
	{
		// Do nothing
	}

	delete m_pCamera;
	m_pCamera = 0;

	delete m_pLobby;
	m_pLobby = 0;

	delete m_pRope;
	m_pRope = 0;

	delete m_pTitle;
	m_pTitle = 0;

	delete m_pOptionMenu;
	m_pOptionMenu = 0;

	delete m_pTick;
	m_pTick = 0;

	delete m_pPause;
	m_pPause = 0;
}

//****************************************************************
//
// @name: BalanceFighterApp::InitApp
// @description: Initialises the app
// @author: Team SnakePit
// @parameter: /
// @return: /
// 
//****************************************************************
void BalanceFighterApp::InitApp()
{
	// TODO: Fix warning here from unsafe cast
	srand(static_cast<int>(time(0)));
	D3DApp::InitApp();

	// Clears as black
	mClearColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	CServices::ProvideService(SERVICE_D3DDEVICE, md3dDevice);
	CServices::ProvideService(SERVICE_CAMERA, m_pCamera);
	
	InitMenu();

	m_pLobby = new CLobby();
	float fScale = 60.f;
	m_pLobby->Init(fScale);
	CLogManager::GetInstance().LogEvent("Initialised Lobby");

	CSoundManager::GetInstance().Initialise();
	CLogManager::GetInstance().LogEvent("Sound Manager initialised");
	CSoundManager::GetInstance().LoadInitialSounds();
	CLogManager::GetInstance().LogEvent("Initial sounds loaded");

	//CGame::GetInstance()->Initialise();
	CLogManager::GetInstance().LogEvent("App initialised");

	CSoundManager::GetInstance().PlayLoadedSound(Announcer, Title);

	// TODO: Place this elsewhere when lobby screen is working
	CSoundManager::GetInstance().LoadPlayerSounds("Alex", "Jake");
	CSoundManager::GetInstance().PlayLoadedSound(PlayerOne, Selection);
}

//****************************************************************
//
// @name: BalanceFighterApp::OnResize
// @description: What happens when screen is resized
// @author: Team SnakePit
// @parameter: /
// @return: /
// 
//****************************************************************
void BalanceFighterApp::OnResize()
{
	D3DApp::OnResize();
	
	// TODO: Fix c style cast ugh
	float fAspect = (float)mClientWidth/mClientHeight;
	m_pCamera->SetPerspective(0.25f*PI, fAspect, 1.0f, 1000.0f);
}

//****************************************************************
//
// @name: BalanceFighterApp::UpdateScene
// @description: Updates the scene
// @author: Team SnakePit
// @parameter: float
// @return: /
// 
//****************************************************************
void BalanceFighterApp::UpdateScene(float _DeltaTime)
{
	D3DApp::updateScene(_DeltaTime);
	m_pCamera->Update(_DeltaTime);
	CSoundManager::GetInstance().m_pSystem->update();

	switch (m_eCurrentGameState)
	{
	case EGAME_STATE_PLAYING:
		{
			if(GetAsyncKeyState(VK_ESCAPE))
			{
				m_eCurrentGameState = EGAME_STATE_MAINMENU;
			}
			else if (GetAsyncKeyState(80)) // p
			{
				m_eCurrentGameState = EGAME_STATE_PAUSED;
			
			}
			else
			{
				// Do nothing
			}

			CGame::GetInstance()->Process(_DeltaTime);	

			if (CGame::GetInstance()->Return() == true)
			{
				m_eCurrentGameState = EGAME_STATE_MAINMENU;
			}
			break;
		}
	case EGAME_STATE_MAINMENU:
		{
			ProcessMenu();
			break;
		}
	case EGAME_STATE_OPTIONS:
		{
			ProcessOptions();
			break;
		}
	case EGAME_STATE_PAUSED:
		{
			if(GetAsyncKeyState(VK_F2))
			{
				if(m_bPause == false)
				{
					m_eCurrentGameState = EGAME_STATE_PLAYING;
				}
				else if(m_bPause == true)
				{
					m_eCurrentGameState = EGAME_STATE_MAINMENU;
				}
				else
				{
					// Do nothing
				}
			}
			else if(GetAsyncKeyState(VK_LEFT))
			{
				m_bPause = false;
			}
			else if(GetAsyncKeyState(VK_RIGHT))
			{
				m_bPause = true;
			}
			else
			{
				// Do nothing
			}
			break;
		}
	case EGAME_STATE_LOBBY:
		{
			Sleep(300);
			CXInput::GetInstance().CheckInput(0);
			eAction ePressAction = CXInput::GetInstance().GetAction();
			if(ePressAction == BUTTON_A)
			{
				m_eCurrentGameState = EGAME_STATE_PLAYING;

				// DELETE AND RECREATE GAME
				CGame::GetInstance()->Restart();
			}
			break;
		}
	case EGAME_STATE_EXITING:
		{
		break;
		}
	default:
		{
		CLogManager::GetInstance().LogEvent("WARNING:Hit default state in BalanceFighterApp::UpdateScene switch case");
		break;
		}
	}
}

//****************************************************************
//
// @name: BalanceFighterApp::ProcessMenu
// @description: Processes the menu
// @author: Alex Burnie
// @parameter: /
// @return: /
// 
//****************************************************************
void BalanceFighterApp::ProcessMenu()
{
	CXInput::GetInstance().CheckInput(0);
	eMovement eMenu = CXInput::GetInstance().GetInput();
	eAction ePressAction = CXInput::GetInstance().GetAction();

	if(eMenu != NO_MOVE)
	{
		switch (eMenu)
		{
		case MOVE_UP:
		{
			if (m_iMenu <= 0)
			{
				// Do nothing
			}
			else
			{
				--m_iMenu;
			}
			Sleep(200);
			break;
		}
		case MOVE_DOWN:
		{
			if (m_iMenu >= 2)
			{
			}
			else
			{
				++m_iMenu;
			}
			Sleep(200);
			break;
		}
		case MOVE_RIGHT:
		{
			// Do nothing
			break;
		}
		case MOVE_LEFT:
		{
			// Do nothing
			break;
		}
		default:
		{
			CLogManager::GetInstance().LogEvent("WARNING: Hit default state in BalanceFighterApp::ProcessMenu eMenu switch case");
			break;
		}
		}
	}

	if(ePressAction != NO_ACTION)
	{
		switch(ePressAction)
		{
		case BUTTON_A:
			{
				if(m_iMenu == 0)
				{
					m_eCurrentGameState = EGAME_STATE_LOBBY;
					// DELETE AND RECREATE GAME
					//CGame::GetInstance()->Restart();
				}
				else if(m_iMenu == 1)
				{
					m_eCurrentGameState = EGAME_STATE_OPTIONS;
				}
				else if(m_iMenu == 2)
				{
					PostQuitMessage(0);
				}
				else
				{
					// Do nothing
				}
				break;
			}
		default:
		{
			CLogManager::GetInstance().LogEvent("WARNING:Hit default state in BalanceFighterApp::ProcessMenu ePressAction switch case");
			break;
		}
		}
	}
}

//****************************************************************
//
// @name: BalanceFighterApp::ProcessOptions
// @description: Processes the options menu
// @author: Alex Burnie
// @parameter: /
// @return: /
// 
//****************************************************************
void BalanceFighterApp::ProcessOptions()
{
	if(GetAsyncKeyState(VK_LEFT))
	{
		if(m_iOptions <= 0)
		{
			// Do nothing
		}
		else
		{
			--m_iOptions;
		}
		
		Sleep(100);
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{				
		if(m_iOptions >= 5)
		{
			// Do nothing
		}
		else
		{
			++m_iOptions;
		}
		Sleep(100);
	}
	else if(GetAsyncKeyState(VK_ESCAPE))
	{
		m_eCurrentGameState = EGAME_STATE_MAINMENU;
	}
	else
	{
		// Do nothing
	}
}

//****************************************************************
//
// @name: BalanceFighterApp::DrawScene
// @description: Draws the scene
// @author: Team SnakePit
// @parameter: /
// @return: /
// 
//****************************************************************
void BalanceFighterApp::DrawScene()
{
	D3DApp::DrawScene();
	
	switch (m_eCurrentGameState)
	{
	case EGAME_STATE_PLAYING:
		{
			CGame::GetInstance()->Draw();
			break;
		}
	case EGAME_STATE_MAINMENU:
		{
			m_pTitle->Draw();

			if(m_iMenu == 0)
			{
				m_pRope->Init(2.0f,D3DXVECTOR3(2.0f,-13.0f,1.0f), 2 , 2);	
				m_pRope->Draw();
			}
			else if(m_iMenu == 1)
			{
				m_pRope->Init(2.0f,D3DXVECTOR3(2.0f,-33.0f,1.0f), 2 , 2);	
				m_pRope->Draw();
			}
			else if(m_iMenu == 2)
			{
				m_pRope->Init(2.0f,D3DXVECTOR3(2.0f,-53.0f,1.0f), 2 , 2);	
				m_pRope->Draw();
			}
			else
			{
				// Do nothing
			}
			break;
		}
	case EGAME_STATE_OPTIONS:
		{
			m_pOptionMenu->Draw();

			if(m_iOptions == 0)
			{
				m_pTick->InitOptions(5.0f , D3DXVECTOR3(-5.0f,5.0f,0.0f), 2 , 1);
				m_pTick->Draw();
			}
			else if(m_iOptions == 1)
			{
				m_pTick->InitOptions(5.0f , D3DXVECTOR3(66.0f,5.0f,0.0f), 2 , 1);
				m_pTick->Draw();

			}
			else if(m_iOptions == 2)
			{
				m_pTick->InitOptions(5.0f , D3DXVECTOR3(-5.0f,-10.0f,0.0f), 2 , 1);
				m_pTick->Draw();
			}
			else if(m_iOptions == 3)
			{
				m_pTick->InitOptions(5.0f , D3DXVECTOR3(66.0f, -10.0f,0.0f), 2 , 1);
				m_pTick->Draw();
			}
			else if(m_iOptions == 4)
			{
				m_pTick->InitOptions(5.0f , D3DXVECTOR3(-5.0f,-25.0f,0.0f), 2 , 1);
				m_pTick->Draw();
			}
			else if(m_iOptions == 5)
			{
				m_pTick->InitOptions(5.0f , D3DXVECTOR3(66.0f,-25.0f,0.0f), 2 , 1);
				m_pTick->Draw();
			}
			else
			{
				// Do nothing
			}
			break;
		}
	case EGAME_STATE_PAUSED:
		{
			m_pPause->Draw();

			if(m_bPause == true)
			{
				m_pPauseChoice->InitPause(10.0f , D3DXVECTOR3(35.0f,-20.0f,0.0f), 2 , 1);
				m_pPauseChoice->Draw();
			}
			else if(m_bPause == false)
			{
				m_pPauseChoice->InitPause(10.0f , D3DXVECTOR3(-35.0f,-20.0f,0.0f), 2 , 1);
				m_pPauseChoice->Draw();
			}
			else
			{
				// Do nothing
			}
			break;
		}
			case EGAME_STATE_LOBBY:
		{
				m_pLobby->Draw();
				break;
		}
	case EGAME_STATE_EXITING:
		{
		// Do nothing
		break;
		}
	default:
		{
		// Do nothing
		break;
		}
	}

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	//mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, WHITE);

	mSwapChain->Present(0, 0);
}
 
//****************************************************************
//
// @name: BalanceFighterApp::InitMenu
// @description: Initialises the menus
// @author: Alex Burnie
// @parameter: /
// @return: /
// 
//****************************************************************
void BalanceFighterApp::InitMenu()
{
	int x = 10;
	float start1 = 10.0f;
	float start2 = -40.0f;

	m_pRope	= new Menu();
	m_pTitle = new Menu();

	m_pPause = new Menu();
	m_pPauseChoice = new Menu();

	//Option Menu Items
	m_pOptionMenu = new Menu();
	m_pTick		 = new Menu();

	//Main Menu Items
	m_pTitle->Init(90.0f,D3DXVECTOR3(0.0f,0.0f,2.0f), 1 , 1);

	m_pOptionMenu->InitOptions(85.0f , D3DXVECTOR3(0.0f,0.0f,2.0f), 1 , 1);
	m_pPause->InitPause(85.0f , D3DXVECTOR3(0.0f,0.0f,2.0f), 1 , 1);

	CLogManager::GetInstance().LogEvent("Initialised Menus");

	CLogManager::GetInstance().LogEvent("Initialised Lives");

	CLogManager::GetInstance().LogEvent("App initialised menu");
}	