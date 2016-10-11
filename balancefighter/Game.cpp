#include "Game.h"

CGame* CGame::pGame = NULL;

CGame::CGame(void)
{
	mPlayerManager = new CPlayerManager();
	mLevel = new CLevel();
}

CGame::~CGame(void)
{

}

CGame* CGame::GetInstance()
{
	if (pGame == NULL)
	{
		pGame = new CGame();
	}
	return pGame;
}

void CGame::Restart()
{
	delete(mPlayerManager);
	delete(mLevel);
	mPlayerManager = new CPlayerManager();
	mLevel = new CLevel();
	mCam = NULL;

	Initialise();
}

void CGame::Initialise()
{
		/* Get World*/
	void* vpCam;
	CServices::GetService(SERVICE_CAMERA, vpCam);
	mCam = static_cast<CCamera*>(vpCam);

	mCam->SetBattleCam(D3DXVECTOR3(0, 0, -22.0f), D3DXVECTOR3(0, 0, 0));

	mPlayerManager->InitPlayerManager();
	mLevel->initVerts(64.0f);
	SetupPhysics();
}

void CGame::Draw()
{
	mLevel->draw();
	mPlayerManager->Draw();
}

void CGame::Process(float _dt)
{
	mPlayerManager->Process(_dt);	/* Updates Players */
	mLevel->Process(_dt);			/* Ticks Level */

	//BATTLE CAM
	//float factor = (mPlayerManager->GetDistance() / 100);
	//mCam->SetBattleCam(D3DXVECTOR3(mPlayerManager->GetCentrePoint().x, 0, ((-10 * factor) - 12.0f)), mPlayerManager->GetCentrePoint());

}

void CGame::SetupPhysics()
{
	mLevel->initWorld(0.0f, -10.0f);
	mPlayerManager->BuildPhysicalBodies();
}

bool CGame::Return()
{
	if (mPlayerManager->GameOver())
	{
		CSoundManager::GetInstance().m_pchannelMusic->stop();
		mCam->CamReset();
		return true;
	}
	else
	{
		return false;
	}

}


