#ifndef __LOGMANAGER_H__
#define __LOGMANAGER_H__

#pragma once

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <stdio.h>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

static const WORD MAX_CONSOLE_LINES = 500;

// To set up use CLogManager& rLogManager = CLogManager::GetInstance();	CLogManager::GetInstance().Initialise(TRUE FOR CONSOLE/FALSE FOR NONE);
// To log an event use CLogManager::GetInstance().LogEvent("Event Text");
// I've included it in d3dUtil.h, the highest include I think. Change it later if needed.

class CLogManager
{
public:
	~CLogManager();

	static CLogManager& GetInstance();			// Call this, then log
	
	void Initialise(bool _bConsoleOn);			// Call once, set param to true to start console window
	
	void LogEvent(std::string _sEventText);		// Logs event to file(always), and to comsole(if true) with timestamp

	bool TestLogManager();						// Tests the log manager, will test console output regardless of init setting
												// This will also hijack the current console screen, if there is one already
protected:

private:
	// Singleton stuff
	CLogManager();
	CLogManager(const CLogManager& _kr);
	CLogManager& operator= (const CLogManager& _kr);

	void InitialiseConsoleWindow();				// Starts the console window, should only be called once in one run session ever. Otherwise horrors happen
												// There can only be one console window per process, so if running a console app, it will hijack main window
public:

protected:

private:
	static CLogManager* s_pLogManager;
	std::ofstream m_filestream;
	std::string m_sFilename;
	bool m_bConsoleOn;
};

#endif // __LOGMANAGER_H__