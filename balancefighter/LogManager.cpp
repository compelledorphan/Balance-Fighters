#include "LogManager.h"

CLogManager* CLogManager::s_pLogManager = 0;

CLogManager::CLogManager()
{

}

CLogManager::~CLogManager()
{
	LogEvent("Closing log manager");
	m_filestream.close();

	delete s_pLogManager;
	s_pLogManager = 0;
}

void CLogManager::Initialise(bool _ConsoleOn)
{
	std::string sFilename;
	char cBuffer[80];
	time_t rawtime;
	struct tm * timeinfo;

	// sFileName = "/FileStructure/basefolder/";	// Update for adjusting where log files go.
	m_bConsoleOn = _ConsoleOn;

	if(m_bConsoleOn == true)
	{
		InitialiseConsoleWindow();
	}
	else
	{
		// Do nothing
	}
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	
	// day-month-year_hour-minute-second
	std::strftime(cBuffer, 80, "%d-%m-%y_%H-%M-%S", timeinfo);
	sFilename.append(cBuffer);
	sFilename.append(" - Debug Log.txt");

	m_sFilename = sFilename;
	m_filestream.open(sFilename);
	if(m_filestream.fail())
	{
		std::cout << ("Log Manager failed to start correctly");
	}
	else
	{
		LogEvent("Log Manager initialised");
	}
}

void CLogManager::InitialiseConsoleWindow()
{
	// from http://www.cplusplus.com/forum/windows/58206/ for reference

	int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;

// allocate a console for this app
    AllocConsole();

// set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize);

// redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen( hConHandle, "w" );

    *stdout = *fp;

    setvbuf( stdout, NULL, _IONBF, 0 );

// redirect unbuffered STDIN to the console

    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

// redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen( hConHandle, "w" );

    *stderr = *fp;

    setvbuf( stderr, NULL, _IONBF, 0 );

// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
	std::ios::sync_with_stdio();
}

void CLogManager::LogEvent(std::string _sEventText)
{
	std::string sTextToLog;
	char cBuffer[80];
	time_t rawtime;
	struct tm * timeinfo;
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	
	// hour-minute-second
	std::strftime(cBuffer, 80, "%H:%M:%S", timeinfo);
	sTextToLog = cBuffer;
	sTextToLog.append(" - ");
	sTextToLog.append(_sEventText);
	sTextToLog.append("\n");

	if(m_bConsoleOn == true)
	{
		std::cout << sTextToLog;
	}
	else
	{
		// Don't log to console
	}
	m_filestream << sTextToLog;
}

CLogManager& CLogManager::GetInstance()
{
	if(s_pLogManager == 0)
	{
		s_pLogManager = new CLogManager();
	}
	else
	{
		// Do nothing
	}

	return(*s_pLogManager);
}

bool CLogManager::TestLogManager()
{
	bool bResult = false;
	std::string sFirstLineOfFile;
	std::ifstream TestReadStream;

	if(m_bConsoleOn == false)
	{
		InitialiseConsoleWindow();
	}
	else
	{
		// Do not initialise a second one, funky things may happen
	}
	
	m_bConsoleOn = true;					// Switch on console window, forced for testing
	LogEvent("Testing Log Manager");
	LogEvent("Checking for new lines");
	m_bConsoleOn = false;					// Turn it off, not much point to though, I think though?

	return(bResult);
}