//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2014 Media Design School
//
// File Name: BalanceFighterApp.h
// Description: Base header for entire game
// Author: Team SnakePit
// Email: liam.simmons@mediadesign.school.nz, jacob.dodunski@mediadesign.school.nz, 
//			rupert.taavao@mediadesign.school.nz, alex.burnie@mediadesign.school.nz
//

#pragma once

#ifndef __BALANCEFIGHTERAPP_H__
#define __BALANCEFIGHTERAPP_H__

// Library Includes
#include <time.h>
#include <random>

// Local Includes
#include "d3dApp.h"
#include "Game.h"
#include "Camera.h"
#include "CMenu.h"
#include "UserInterface.h"
#include "CXInput.h"
#include "Lobby.h"
#include "Player.h"

// Types
typedef enum 
{
	EGAME_INVALID_STATE = 0,
	EGAME_STATE_MAINMENU,
	EGAME_STATE_OPTIONS,
	EGAME_STATE_PLAYING,
	EGAME_STATE_LOBBY,
	EGAME_STATE_EXITING,
	EGAME_STATE_PAUSED,

	EGAME_STATE_MAX
} EGAME_STATE;

class BalanceFighterApp : public D3DApp
{
public:
	BalanceFighterApp(HINSTANCE _hInstance);
	~BalanceFighterApp();

	void InitApp();
	void OnResize();
	void UpdateScene(float _fDeltaTime);
	void DrawScene(); 
	void InitMenu();

	void ProcessMenu();
	void ProcessOptions();

protected:
 
private:
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
	D3DXMATRIX m_matWVP;

	D3DXVECTOR3 m_vecEyePos;

	float m_fRadius;
	float m_fTheta;
	float m_fPhi;

	int m_iMenu;
	int m_iOptions;
	int m_iHearts;

	bool m_bPause;

	EGAME_STATE m_eCurrentGameState;

	CCamera* m_pCamera;

	CLobby* m_pLobby;

	//Main Menu 
	Menu* m_pRope;
	Menu* m_pTitle;

	//Option Menu
	Menu* m_pOptionMenu;
	Menu* m_pTick;
	
	// Pause menu
	Menu* m_pPause;
	Menu* m_pPauseChoice;

	CPlayer* m_pPlayer;
};

#endif // __BALANCEFIGHTERAPP_H__