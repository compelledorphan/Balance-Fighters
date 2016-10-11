#pragma once

#include "Resources/Library/Box2D/Box2D.h"

#include "Level.h"
#include "PlayerManager.h"
#include "CServices.h"


class CGame
{
public:
	
	~CGame(void);

	static CGame* GetInstance();

	void Initialise();

	void Draw();

	void Process(float _dt);

	void SetupPhysics();

	void Restart();

	bool Return();

private:
	CGame(void);

private:
	static CGame* pGame;

	CCamera* mCam;

	CPlayerManager* mPlayerManager;
	CLevel* mLevel;

};

