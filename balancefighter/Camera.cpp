//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2014 Media Design School
//
// File Name : Camera.cpp
// File Description : Camera Implementation
// Author : Jacob Dodunski
// Mail : Jacob.Dodunski@mediadesign.school.nz
//

// Local Includes
#include "CCamera.h"

//****************************************************************
//
// @name: Camera::Camera
// @description: The Camera Constructor
// @author: Jacob Dodunski 
// @parameter: /
// @return: /
// 
//****************************************************************
CCamera::CCamera()
{

	D3DXMatrixIdentity(&m_matViewM);
    D3DXMatrixIdentity(&m_matProjM);
    D3DXMatrixIdentity(&m_matRotM);

	CamReset();

    m_bMoveToggle[0] = false;
	m_bMoveToggle[1] = false;
	m_bMoveToggle[2] = false;
	m_bMoveToggle[3] = false;
	m_bMoveToggle[4] = false;
	m_bMoveToggle[5] = false;
}

//****************************************************************
//
// @name: Camera::~Camera
// @description: The Camera Deconstructor
// @author: Jacob Dodunski 
// @parameter: /
// @return: /
// 
//****************************************************************
CCamera::~CCamera()
{

}

//****************************************************************
//
// @name: Camera::Update
// @description: The Camera Update function, that updates the camera to the user 
//				input based on the movement toggles.
// @author: Jacob Dodunski 
// @parameter: float _fDeltaTime
// @return: /
// 
//****************************************************************
void CCamera::Update(float _fDeltaTime)
{
	D3DXVECTOR3 vecDirection;
	D3DXMATRIX matRotM;

	D3DXMatrixIdentity(&matRotM);

    D3DXMatrixLookAtLH(&m_matViewM, &m_vecPos, &m_vecTarget, &m_vecUp);
}

//****************************************************************
//
// @name: Camera::GetViewMatrix
// @description: Accessor to return the view matrix
// @author: Jacob Dodunski 
// @parameter: /
// @return: D3DXMATRIX
// 
//****************************************************************
D3DXMATRIX& CCamera::GetViewMatrix()
{
	return (m_matViewM);
}

//****************************************************************
//
// @name: Camera::GetProjectionMatrix
// @description: Accessor to return the projection matrix
// @author: Jacob Dodunski 
// @parameter: /
// @return: D3DXMATRIX
// 
//****************************************************************
D3DXMATRIX& CCamera::GetProjectionMatrix()
{
	return (m_matProjM);
}

//****************************************************************
//
// @name: Camera::SetPerspective
// @description: A function to set the perspective of the projection matrix based
//				on inputed values.
// @author: Jacob Dodunski 
// @parameter: float , float , float , float 
// @return: /
// 
//****************************************************************
void CCamera::SetPerspective(float _fFOV, float _fAspectRatio, 
	float _fZNear, float _fZFar)
{
	_fFOV = _fFOV * PI;
	D3DXMatrixPerspectiveFovLH(&m_matProjM, _fFOV, _fAspectRatio, _fZNear, _fZFar);
}

//****************************************************************
//
// @name: Camera::SetMovementToggle
// @description: A function that sets the movement toggles based on user input.
// @author: Jacob Dodunski 
// @parameter: int, bool
// @return: /
// 
//****************************************************************
void CCamera::SetMovementToggle(int _iIndex, bool _bValue)
{
	m_bMoveToggle[_iIndex] = _bValue;
}

//****************************************************************
//
// @name: Camera::GetEye
// @description: A function that returns the Eye Vector of the camera.
// @author: Jacob Dodunski 
// @parameter: int, bool
// @return: /
// 
//****************************************************************
D3DXVECTOR3& CCamera::GetEye()
{
	return (m_vecPos);
}

//****************************************************************
//
// @name: Camera::GetTarget
// @description: A function that returns the Target Vector of the camera.
// @author: Jacob Dodunski 
// @parameter: int, bool
// @return: /
// 
//****************************************************************
D3DXVECTOR3& CCamera::GetTarget()
{
	return (m_vecDirection);
}

//****************************************************************
//
// @name: Camera::CamReset
// @description: Resets camera
// @author: Jacob Dodunski
// @parameter: /
// @return: /
// 
//****************************************************************
void CCamera::CamReset()
{
	m_vecPos = D3DXVECTOR3(0, 0, -20);
	m_vecTarget = D3DXVECTOR3(0, 0, 0);
	m_vecUp = D3DXVECTOR3(0, 1, 0);
	m_vecDirection = D3DXVECTOR3(0, 0, 0);
}

//****************************************************************
//
// @name: Camera::SetBattleCam
// @description: 
// @author: Jacob Dodunski
// @parameter: Vector3, Vector3
// @return: /
// 
//****************************************************************
void CCamera::SetBattleCam(D3DXVECTOR3 _pos, D3DXVECTOR3 _target)
{
	m_vecPos = _pos;
	m_vecTarget = _target;
}