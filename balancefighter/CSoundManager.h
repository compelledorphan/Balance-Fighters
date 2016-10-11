#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#pragma once

#define _USE_MATH_DEFINES

#include "fmod.hpp"
#include "fmod_errors.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <math.h>
#include "LogManager.h"

// Call in every update loop CSoundManager::GetInstance().m_pSystem->update();
// To play sounds use CSoundManager::GetInstance().PlayLoadedSound(USERENUM, SOUNDENUM);

enum m_eSOUNDUSERS
{
	Announcer,
	PlayerOne,
	PlayerTwo
};

enum m_eSOUNDS
{
	Count,
	Choose,
	GameOver,
	Title,
	CatchPhrase,
	Fall,
	Jumping,
	Lobby,
	MoveToFight,
	Selection,
	HitOne,
	HitTwo,
	Victory,
	Woopsie
};

class CSoundManager
{
public:
	~CSoundManager();
		
	void Initialise();			// Load before anything is ever displayed. Starts sound manager and 
	void LoadInitialSounds();	// loads in default announcer / boots up soundtrack.

	static CSoundManager& GetInstance();

//protected:
	void LoadPlayerSounds(std::string _sPlayerOneVoiceName, std::string _sPlayerTwoVoiceName);	// Loads two players sounds
	void LoadAnnouncerSounds(std::string _sAnnouncerName);	// Loads a particular announcers sound files
	void PlayLoadedSound(m_eSOUNDUSERS _m_eSoundUsers, m_eSOUNDS _m_eSounds);	// Plays sounds based on enums
	
	void StartSoundtrack();			// Starts playing the soundtrack on an infinite loop.
	void TogglePauseSoundtrack();	// Toggles whether the soundtrack in pause or not.

	void ChangeMasterVolume(float _fChangeAmount);	// All volumes
	float GetMasterVolume();
	void ChangeMusicVolume(float _fChangeAmount);	// Soundtrack volume (affected by master)
	float GetMusicVolume();
	void ChangeEffectsVolume(float _fChangeAmount);	// Effects volume (affected by master)
	float GetEffectsVolume();

//private:
	void FMODErrorCheck(FMOD_RESULT _result);		// Basic errorchecker for FMOD functions

	void PlayFallingFade(m_eSOUNDUSERS _m_eSoundUsers);		// 
	
	bool m_bPlayerOneFalling, m_bPlayerTwoFalling;
	int m_iFadeLength, m_iFadeStartTickPlayerOne, m_iFadeStartTickPlayerTwo;

	FMOD::System *m_pSystem;
	FMOD_RESULT m_Result;
	FMOD::ChannelGroup *m_pchannelgroupSounds;
	FMOD::Channel *m_pchannelMusic, *m_pchannelEffects, *m_pchannelPlayerOneFalling, 
		*m_pchannelPlayerTwoFalling, *m_pchannelEffectWoopsie;

	FMOD::Sound *m_pAnnouncerCount, *m_pAnnouncerChoose, *m_pAnnouncerGameOver,
		*m_pAnnouncerTitle;
	FMOD::Sound *m_pPOneCatchPhrase, *m_pPOneFall, *m_pPOneJump, *m_pPOneLobby,
		*m_pPOneMoveToFight, *m_pPOneSelection, *m_pPOneHitOne, *m_pPOneHitTwo,
		*m_pPOneVictory, *m_pPOneWoopsie;
	FMOD::Sound *m_pPTwoCatchPhrase, *m_pPTwoFall, *m_pPTwoJump, *m_pPTwoLobby,
		*m_pPTwoMoveToFight, *m_pPTwoSelection, *m_pPTwoHitOne, *m_pPTwoHitTwo,
		*m_pPTwoVictory, *m_pPTwoWoopsie;
	FMOD::Sound *m_pSoundtrack;

private:
	static CSoundManager* s_pSoundManager;
	CSoundManager();
	CSoundManager(const CSoundManager& _kr);
	CSoundManager& operator= (const CSoundManager& _kr);
};

#endif // !__SOUNDMANAGER_H__
// http://katyscode.wordpress.com/2012/10/05/cutting-your-teeth-on-fmod-part-1-build-environment-initialization-and-playing-sounds/