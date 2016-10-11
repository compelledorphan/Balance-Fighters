//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2014 Media Design School
//
// File Name : Camera.h
// File Description : Camera Header File
// Author : Jacob Dodunski
// Mail : Jacob.Dodunski@mediadesign.school.nz
//

#pragma once

#ifndef __CCAMERA_H__
#define __CCAMERA_H__

// Local Includes
#include "d3dUtil.h"

class CCamera
{
public:
	CCamera();
	~CCamera();
	
	// Updates
	void Update(float _fDeltaTime);

	// Retriving variables 
	D3DXMATRIX& GetViewMatrix();
	D3DXMATRIX& GetProjectionMatrix();
	D3DXVECTOR3& GetEye();

    void SetPerspective(float _fFOV, float _fAspectRatio, float _fZNear, float _fZFar);
 
    // Camera Methods
    void SetMovementToggle(int _iIndex, bool _bValue);

	D3DXVECTOR3& GetTarget();

	void CamReset();
	void SetBattleCam(D3DXVECTOR3 _vecPos, D3DXVECTOR3 _vecTarget);

private:

	// Matricies
	D3DXMATRIX m_matViewM;
    D3DXMATRIX m_matProjM;
    D3DXMATRIX m_matRotM;

	// Vectors for View
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecTarget;
	D3DXVECTOR3 m_vecUp;
				  
	D3DXVECTOR3 m_vecDirection;

	// Movement Toggles
    bool m_bMoveToggle[6];
};

#endif	// __CCAMERA_H__