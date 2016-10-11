#ifndef  __CXINPUT_H_
#define  __CXINPUT_H_

#include "d3dUtil.h"
#include <XInput.h>
#include "EMovments.h"
#include "EActions.h"

class CXInput
{
	// Member Functions
public:

	~CXInput();

	XINPUT_STATE GetState();
	bool IsConnected();

	void Vibrate(int _iLeft, int _iRight);

	void CheckInput(int iController);

	void CheckMovement();
	void CheckFire();

	float GetLX();
	float GetLY();

	float GetRX();
	float GetRY();

	bool GetFire();

	bool GetJump();

	e_Movement GetInput();

	float GetBalance();
	e_Action GetAction();

	static CXInput& GetInstance();
	static void DestroyInstance();

private:
	CXInput();

private:
	int m_iControllerNumber;
	XINPUT_STATE m_ControllerState;

	float m_DeadzoneX;
	float m_DeadzoneY;

	float m_LeftX;
	float m_LeftY;

	float m_RightX;
	float m_RightY;

	bool m_Fire;

	static CXInput* s_Input;

protected:

};

#endif //__CXINPUT_H_

