#include <math.h>

#include "CXInput.h"

CXInput* CXInput::s_Input = 0;

CXInput::CXInput()
{
	m_iControllerNumber = 0 ;
	m_DeadzoneX = 0.08f;
	m_DeadzoneY = 0.08f;

	m_LeftX = 0.0f;
	m_LeftY = 0.0f;

	m_RightX = 0.0f;
	m_RightY = 0.0f;

	m_Fire = false;
}


CXInput::~CXInput()
{
	s_Input = 0;
}

XINPUT_STATE CXInput::GetState()
{
	ZeroMemory(&m_ControllerState, sizeof(XINPUT_STATE));

	XInputGetState(m_iControllerNumber, &m_ControllerState);

	return m_ControllerState;
}

bool CXInput::IsConnected()
{
	ZeroMemory(&m_ControllerState, sizeof(XINPUT_STATE));

	DWORD Result = XInputGetState(0, &m_ControllerState);
	//DWORD Result2 = XInputGetState(1, &m_ControllerState);

	if(Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CXInput::Vibrate(int _iLeft, int _iRight)
{
	/* Create a Vibraton State */
	XINPUT_VIBRATION Vibration;

	/* Zero the Vibration */
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	/* Set the Vibration Values */
	Vibration.wLeftMotorSpeed = _iLeft;
	Vibration.wRightMotorSpeed = _iRight;

	/* Vibrate the controller */
	XInputSetState(m_iControllerNumber, &Vibration);
}

void CXInput::CheckInput(int iController)
{
	if(IsConnected())
	{
		XInputGetState(iController, &m_ControllerState);
	}
}

e_Movement CXInput::GetInput()
{
	e_Movement retVal = NO_MOVE;

	CheckMovement();

	if(m_LeftY != 0)
	{
		if(m_LeftY > 0)
		{
			retVal = MOVE_UP;
		}
		else
		{
			retVal = MOVE_DOWN;
		}
	}

	if(m_LeftX != 0)
	{
		if(m_LeftX > 0.2)
		{
			retVal = MOVE_RIGHT;
		}
		else if(m_LeftX < -0.2)
		{
			retVal = MOVE_LEFT;
		}
	}
	return retVal;
}


float CXInput::GetBalance()
{
	CheckFire();

	return m_RightX;
}

e_Action CXInput::GetAction()
{
	e_Action retVal = NO_ACTION;

	if(m_ControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		retVal = BUTTON_A;
	}
	return retVal;
}

void CXInput::CheckMovement()
{

	float normLX = max(-1, (float) m_ControllerState.Gamepad.sThumbLX / 32767);
	float normLY = max(-1, (float) m_ControllerState.Gamepad.sThumbLY / 32767);

	if(abs(normLX) < m_DeadzoneX)
	{
		m_LeftX = 0;
	}
	else
	{
		m_LeftX = (abs(normLX) - m_DeadzoneX) * (normLX / abs(normLX));
	}

	if(abs(normLY) < m_DeadzoneY)
	{
		m_LeftY = 0;
	}
	else
	{
		m_LeftY = (abs(normLY) - m_DeadzoneY) * (normLY / abs(normLY));
	}

	if (m_DeadzoneX > 0) 
	{
		m_LeftX *= 1 / (1 - m_DeadzoneX);
	}
	if (m_DeadzoneY > 0) 
	{
		m_LeftY *= 1 / (1 - m_DeadzoneY);
	}
}

void CXInput::CheckFire()
{
	float normRX = max(-1, (float) m_ControllerState.Gamepad.sThumbRX / 32767);
	float normRY = max(-1, (float) m_ControllerState.Gamepad.sThumbRY / 32767);

	if(abs(normRX) < m_DeadzoneX)
	{
		m_RightX = 0;
	}
	else
	{
		m_RightX = (abs(normRX) - m_DeadzoneX) * (normRX / abs(normRX));
	}

	if(abs(normRY) < m_DeadzoneY)
	{
		m_RightY = 0;
	}
	else
	{
		m_RightY = (abs(normRY) - m_DeadzoneY) * (normRY / abs(normRY));
	}

	if (m_DeadzoneX > 0) 
	{
		m_RightY *= 1 / (1 - m_DeadzoneX);
	}

	if (m_DeadzoneY > 0) 
	{
		m_RightY *= 1 / (1 - m_DeadzoneY);
	}
}

float CXInput::GetLX()
{
	return -m_LeftX;
}

float CXInput::GetLY()
{
	return m_LeftY;
}

float CXInput::GetRX()
{
	return -m_RightX;
}

float CXInput::GetRY()
{
	return m_RightY;
}

bool CXInput::GetFire()
{
	if(abs(m_RightX) > 0.2 || abs(m_RightY) > 0.2)
	{

		return true;
	}
	else
	{
		return false;
	}
}

bool CXInput::GetJump()
{
	bool retVal = false;
	if(m_LeftY > 0.8)
	{
		retVal = true;
	}

	return retVal;
}

CXInput& CXInput::GetInstance()
{
	if (s_Input == 0)
	{
		s_Input = new CXInput();
	}
	return (*s_Input);
}

void CXInput::DestroyInstance()
{
	delete s_Input;
	s_Input = 0;
}