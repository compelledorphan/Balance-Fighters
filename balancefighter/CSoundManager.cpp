#include "CSoundManager.h"

CSoundManager* CSoundManager::s_pSoundManager = 0;

CSoundManager::CSoundManager()
{
	m_bPlayerOneFalling = false;
	m_bPlayerTwoFalling = false;
	m_iFadeLength = 3000;
}

CSoundManager::~CSoundManager()
{
	FMODErrorCheck(m_pAnnouncerCount->release());
	FMODErrorCheck(m_pAnnouncerChoose->release());
	FMODErrorCheck(m_pAnnouncerGameOver->release());
	FMODErrorCheck(m_pAnnouncerTitle->release());

	FMODErrorCheck(m_pPOneCatchPhrase->release());
	FMODErrorCheck(m_pPOneFall->release());
	FMODErrorCheck(m_pPOneJump->release());
	FMODErrorCheck(m_pPOneLobby->release());
	FMODErrorCheck(m_pPOneMoveToFight->release());
	FMODErrorCheck(m_pPOneSelection->release());
	FMODErrorCheck(m_pPOneHitOne->release());
	FMODErrorCheck(m_pPOneHitTwo->release());
	FMODErrorCheck(m_pPOneVictory->release());
	FMODErrorCheck(m_pPOneWoopsie->release());

	FMODErrorCheck(m_pPTwoCatchPhrase->release());
	FMODErrorCheck(m_pPTwoFall->release());
	FMODErrorCheck(m_pPTwoJump->release());
	FMODErrorCheck(m_pPTwoLobby->release());
	FMODErrorCheck(m_pPTwoMoveToFight->release());
	FMODErrorCheck(m_pPTwoSelection->release());
	FMODErrorCheck(m_pPTwoHitOne->release());
	FMODErrorCheck(m_pPTwoHitTwo->release());
	FMODErrorCheck(m_pPTwoVictory->release());
	FMODErrorCheck(m_pPTwoWoopsie->release());

	m_pSystem->release();

	delete s_pSoundManager;
	s_pSoundManager = 0;
}

void CSoundManager::FMODErrorCheck(FMOD_RESULT _result)
{
	std::string error;

	if (_result != FMOD_OK)
	{
		error = "FMOD error! (";
		error += _result;
		error += FMOD_ErrorString(_result);
		
		CLogManager::GetInstance().LogEvent(error);
		exit(-1);
	}
	CLogManager::GetInstance().LogEvent("SoundManager Error Check passed");
}

void CSoundManager::Initialise()
{
	unsigned int version;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	char name[256];

	// Create FMOD interface object
	m_Result = FMOD::System_Create(&m_pSystem);
	FMODErrorCheck(m_Result);

	// Check version
	m_Result = m_pSystem->getVersion(&version);
	FMODErrorCheck(m_Result);

	if (version < FMOD_VERSION)
	{
		std::cout << "Error! You are using an old version of FMOD " << version << ". This program requires " << FMOD_VERSION << std::endl;
		return;
	}

	// Get number of sound cards
	m_Result = m_pSystem->getNumDrivers(&numDrivers);
	FMODErrorCheck(m_Result);

	// No sound cards (disable sound)
	if (numDrivers == 0)
	{
		m_Result = m_pSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		FMODErrorCheck(m_Result);
	}

	// At least one sound card
	else
	{
		// Get the capabilities of the default (0) sound card
		m_Result = m_pSystem->getDriverCaps(0, &caps, 0, &speakerMode);
		FMODErrorCheck(m_Result);

		// Set the speaker mode to match that in Control Panel
		m_Result = m_pSystem->setSpeakerMode(speakerMode);
		FMODErrorCheck(m_Result);

		// Increase buffer size if user has Acceleration slider set to off
		if (caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			m_Result = m_pSystem->setDSPBufferSize(1024, 10);
			FMODErrorCheck(m_Result);
		}

		// Get name of driver
		m_Result = m_pSystem->getDriverInfo(0, name, 256, 0);
		FMODErrorCheck(m_Result);

		// SigmaTel sound devices crackle for some reason if the format is PCM 16-bit.
		// PCM floating point output seems to solve it.
		if (strstr(name, "SigmaTel"))
		{
			m_Result = m_pSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			FMODErrorCheck(m_Result);
		}
	}

	// Initialise FMOD
	m_Result = m_pSystem->init(1000, FMOD_INIT_NORMAL, 0);

	// If the selected speaker mode isn't supported by this sound card, switch it back to stereo
	if (m_Result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		m_Result = m_pSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		FMODErrorCheck(m_Result);

		m_Result = m_pSystem->init(100, FMOD_INIT_NORMAL, 0);
	}
	FMODErrorCheck(m_Result);

	//m_Result = m_pchannelEffects->setDelay(FMOD_DELAYTYPE_END_MS, 10, 10);
	//FMODErrorCheck(m_Result);

	m_Result = m_pSystem->createChannelGroup(NULL, &m_pchannelgroupSounds);
	FMODErrorCheck(m_Result);

	LoadInitialSounds();
}

void CSoundManager::LoadPlayerSounds(std::string _sPlayerOneVoiceName, std::string _sPlayerTwoVoiceName)
{
	std::string sPOneCatchPhrase, sPOneFall, sPOneJump, sPOneLobby, sPOneMoveToFight, sPOneSelection, sPOneHitOne, sPOneHitTwo, sPOneVictory, sPOneWoopsie;
	std::string sPTwoCatchPhrase, sPTwoFall, sPTwoJump, sPTwoLobby, sPTwoMoveToFight, sPTwoSelection, sPTwoHitOne, sPTwoHitTwo, sPTwoVictory, sPTwoWoopsie;

	sPOneCatchPhrase	= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Catch Phrase.wav";
	sPOneFall			= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Falling.wav";
	sPOneJump			= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Jump.wav";
	sPOneLobby			= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Lobby.wav";
	sPOneMoveToFight	= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Moving to fight.wav";
	sPOneSelection		= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Selection.wav";
	sPOneHitOne			= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Taking a hit 1.wav";
	sPOneHitTwo			= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Taking a hit 2.wav";
	sPOneVictory		= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Victory.wav";
	sPOneWoopsie		= "Resources\\Sounds\\" + _sPlayerOneVoiceName + " - Woopsie.wav";
									  		
	sPTwoCatchPhrase	= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Catch Phrase.wav";
	sPTwoFall			= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Falling.wav";
	sPTwoJump			= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Jump.wav";
	sPTwoLobby			= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Lobby.wav";
	sPTwoMoveToFight	= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Moving to fight.wav";
	sPTwoSelection		= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Selection.wav";
	sPTwoHitOne			= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Taking a hit 1.wav";
	sPTwoHitTwo			= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Taking a hit 2.wav";
	sPTwoVictory		= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Victory.wav";
	sPTwoWoopsie		= "Resources\\Sounds\\" + _sPlayerTwoVoiceName + " - Woopsie.wav";

	m_Result = m_pSystem->createSound(sPOneCatchPhrase.c_str(), FMOD_UNIQUE, 0, &m_pPOneCatchPhrase);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneFall.c_str(), FMOD_UNIQUE, 0, &m_pPOneFall);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneJump.c_str(), FMOD_UNIQUE, 0, &m_pPOneJump);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneLobby.c_str(), FMOD_UNIQUE, 0, &m_pPOneLobby);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneMoveToFight.c_str(), FMOD_UNIQUE, 0, &m_pPOneMoveToFight);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneSelection.c_str(), FMOD_UNIQUE, 0, &m_pPOneSelection);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneHitOne.c_str(), FMOD_UNIQUE, 0, &m_pPOneHitOne);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneHitTwo.c_str(), FMOD_UNIQUE, 0, &m_pPOneHitTwo);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneVictory.c_str(), FMOD_UNIQUE, 0, &m_pPOneVictory);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPOneWoopsie.c_str(), FMOD_UNIQUE, 0, &m_pPOneWoopsie);
	FMODErrorCheck(m_Result);

	m_Result = m_pSystem->createSound(sPTwoCatchPhrase.c_str(), FMOD_UNIQUE, 0, &m_pPTwoCatchPhrase);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoFall.c_str(), FMOD_UNIQUE, 0, &m_pPTwoFall);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoJump.c_str(), FMOD_UNIQUE, 0, &m_pPTwoJump);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoLobby.c_str(), FMOD_UNIQUE, 0, &m_pPTwoLobby);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoMoveToFight.c_str(), FMOD_UNIQUE, 0, &m_pPTwoMoveToFight);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoSelection.c_str(), FMOD_UNIQUE, 0, &m_pPTwoSelection);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoHitOne.c_str(), FMOD_UNIQUE, 0, &m_pPTwoHitOne);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoHitTwo.c_str(), FMOD_UNIQUE, 0, &m_pPTwoHitTwo);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoVictory.c_str(), FMOD_UNIQUE, 0, &m_pPTwoVictory);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sPTwoWoopsie.c_str(), FMOD_UNIQUE, 0, &m_pPTwoWoopsie);
	FMODErrorCheck(m_Result);
}

void CSoundManager::LoadAnnouncerSounds(std::string _sAnnouncerName)
{
	std::string sCount, sChoose, sGameOver, sTitle;

	sCount		= "Resources\\Sounds\\" +_sAnnouncerName + " - 3 2 1.wav";
	sChoose		= "Resources\\Sounds\\" +_sAnnouncerName + " - Choose your fighter.wav";
	sGameOver	= "Resources\\Sounds\\" +_sAnnouncerName + " - Game Over.wav";
	sTitle		= "Resources\\Sounds\\" +_sAnnouncerName + " - Title.wav";

	m_Result = m_pSystem->createSound(sCount.c_str(), FMOD_UNIQUE, 0, &m_pAnnouncerCount);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sChoose.c_str(), FMOD_UNIQUE, 0, &m_pAnnouncerChoose);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sGameOver.c_str(), FMOD_UNIQUE, 0, &m_pAnnouncerGameOver);
	FMODErrorCheck(m_Result);
	m_Result = m_pSystem->createSound(sTitle.c_str(), FMOD_UNIQUE, 0, &m_pAnnouncerTitle);
	FMODErrorCheck(m_Result);
}

void CSoundManager::LoadInitialSounds()
{
	LoadAnnouncerSounds("Kelsey");

	m_Result = m_pSystem->createSound("Resources\\Sounds\\Soundtrack.mp3", FMOD_DEFAULT, 0, &m_pSoundtrack);
	FMODErrorCheck(m_Result);
	m_pchannelMusic->setLoopCount(-1); // Forever loop
}

CSoundManager& CSoundManager::GetInstance()
{
	if (s_pSoundManager == 0)
	{
		s_pSoundManager = new CSoundManager();
	}
	else
	{
		// Do nothing
	}

	return(*s_pSoundManager);
}

void CSoundManager::PlayFallingFade(m_eSOUNDUSERS _m_eSoundUsers)
{
	if (m_bPlayerOneFalling == true)
	{
		// Get volume from 0.0f - 1.0f depending on number of milliseconds elapsed since fade started
		float volume = min(static_cast<float>(GetTickCount() - m_iFadeStartTickPlayerOne) / m_iFadeLength, 1.0f);

		volume = static_cast<float>(sin(volume * M_PI / 2));
		volume *= volume;

		m_pchannelPlayerOneFalling->setVolume(1.0f - volume);

		float fGottenVolume;

		if (m_pchannelPlayerOneFalling->getVolume(&fGottenVolume) == 0.0f)
		{
			m_bPlayerOneFalling = false;
		}
	}

	if (m_bPlayerTwoFalling == true)
	{
		// Get volume from 0.0f - 1.0f depending on number of milliseconds elapsed since fade started
		float volume = min(static_cast<float>(GetTickCount() - m_iFadeStartTickPlayerTwo) / m_iFadeLength, 1.0f);

		volume = static_cast<float>(sin(volume * M_PI / 2));
		volume *= volume;

		m_pchannelPlayerTwoFalling->setVolume(1.0f - volume);

		float fGottenVolume;

		if (m_pchannelPlayerTwoFalling->getVolume(&fGottenVolume) == 0.0f)
		{
			m_bPlayerTwoFalling = false;
		}
	}
}

void CSoundManager::PlayLoadedSound(m_eSOUNDUSERS _m_eSoundUsers, m_eSOUNDS _m_eSounds)
{
	switch (_m_eSoundUsers)
	{
	case Announcer:
	{
		if (_m_eSounds == Count)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pAnnouncerCount, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Count Announce Sound Played");
		}
		else if (_m_eSounds == Choose)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pAnnouncerChoose, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Choose Announce Sound Played");
		}
		else if (_m_eSounds == GameOver)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pAnnouncerGameOver, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("GameOver Announce Sound Played");
		}
		else if (_m_eSounds == Title)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pAnnouncerTitle, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Title Announce Sound Played");
		}
		else
		{
			// No sounds
		}
		break;
	}
	case PlayerOne:
	{
		if (_m_eSounds == CatchPhrase)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneCatchPhrase, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_bPlayerOneFalling = true;
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("CatchPhrase P1 Sound Played");
		}
		else if (_m_eSounds == Fall)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneFall, true, &m_pchannelPlayerOneFalling);
			m_pchannelPlayerOneFalling->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelPlayerOneFalling->setPaused(false);
			m_iFadeStartTickPlayerOne = GetTickCount();
			PlayFallingFade(PlayerOne);
			CLogManager::GetInstance().LogEvent("Fall P1 Sound Played");
		}
		else if (_m_eSounds == Jumping)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneJump, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Jump P1 Sound Played");
		}
		else if (_m_eSounds == Lobby)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneLobby, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Lobby P1 Sound Played");
		}
		else if (_m_eSounds == MoveToFight)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneMoveToFight, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("MoveToFight P1 Sound Played");
		}
		else if (_m_eSounds == Selection)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneSelection, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Selection P1 Sound Played");
		}
		else if (_m_eSounds == HitOne)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneHitOne, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("HitOne P1 Sound Played");
		}
		else if (_m_eSounds == HitTwo)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneHitTwo, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("HitTwo P1 Sound Played");

		}
		else if (_m_eSounds == Victory)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneVictory, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Victory P1 Sound Played");
		}
		else if (_m_eSounds == Woopsie)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPOneWoopsie, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Woopsie P1 Sound Played");
		}
		else
		{
			// No sounds
		}
		break;
	}
	case PlayerTwo:
		if (_m_eSounds == CatchPhrase)
		{
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoCatchPhrase, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("CatchPhrase P2 Sound Played");
		}												
		else if (_m_eSounds == Fall)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoFall, true, &m_pchannelPlayerTwoFalling);
			m_pchannelPlayerTwoFalling->setChannelGroup(m_pchannelgroupSounds);
			m_bPlayerTwoFalling = true;
			m_pchannelPlayerTwoFalling->setPaused(false);
			m_iFadeStartTickPlayerTwo = GetTickCount();
			PlayFallingFade(PlayerTwo);
			CLogManager::GetInstance().LogEvent("Fall P2 Sound Played");
		}												
		else if (_m_eSounds == Jumping)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoJump, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Jump P2 Sound Played");
		}												
		else if (_m_eSounds == Lobby)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoLobby, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Lobby P2 Sound Played");
		}												
		else if (_m_eSounds == MoveToFight)				
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoMoveToFight, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("MoveToFight P2 Sound Played");
		}												
		else if (_m_eSounds == Selection)				
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoSelection, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Selection P2 Sound Played");
		}												
		else if (_m_eSounds == HitOne)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoHitOne, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("HitOne P2 Sound Played");
		}												
		else if (_m_eSounds == HitTwo)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoHitTwo, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("HitTwo P2 Sound Played");
		}												
		else if (_m_eSounds == Victory)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoVictory, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Victory P2 Sound Played");
		}												
		else if (_m_eSounds == Woopsie)					
		{												
			m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pPTwoWoopsie, true, &m_pchannelEffects);
			m_pchannelEffects->setChannelGroup(m_pchannelgroupSounds);
			m_pchannelEffects->setPaused(false);
			CLogManager::GetInstance().LogEvent("Woopsie P2 Sound Played");
		}
		else
		{
			// No sounds
		}
		break;
	}
}

void CSoundManager::ChangeMasterVolume(float _fChangeAmount)
{
	float fVolume = 0.0f;

	fVolume = GetMasterVolume() + _fChangeAmount;
	m_pchannelgroupSounds->setVolume(fVolume);
}

float CSoundManager::GetMasterVolume()
{
	float fVolume = 0.0f;

	m_pchannelgroupSounds->getVolume(&fVolume);

	return(fVolume);
}

void CSoundManager::ChangeMusicVolume(float _fChangeAmount)
{
	float fVolume = 0.0f;

	fVolume = GetMusicVolume() + _fChangeAmount;
	m_pchannelMusic->setVolume(fVolume);
}

float CSoundManager::GetMusicVolume()
{
	float fVolume = 0.0f;

	m_pchannelMusic->getVolume(&fVolume);

	return(fVolume);
}

void CSoundManager::ChangeEffectsVolume(float _fChangeAmount)
{
	float fVolume = 0.0f;

	fVolume = GetEffectsVolume() + _fChangeAmount;
	m_pchannelEffects->setVolume(fVolume);
}

float CSoundManager::GetEffectsVolume()
{
	float fVolume = 0.0f;

	m_pchannelEffects->getVolume(&fVolume);

	return(fVolume);
}

void CSoundManager::StartSoundtrack()
{
	FMODErrorCheck(m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pSoundtrack, true, &m_pchannelMusic));
	m_pchannelMusic->setChannelGroup(m_pchannelgroupSounds);

	m_pchannelMusic->setPaused(false);
}

void CSoundManager::TogglePauseSoundtrack()
{
	bool bPaused;

	m_pchannelMusic->getPaused(&bPaused);

	if (bPaused == false)
	{
		m_pchannelMusic->setPaused(true);
	}
	else
	{
		m_pchannelMusic->setPaused(false);
	}
}