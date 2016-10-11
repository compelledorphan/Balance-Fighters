#ifndef __CSERVICES_H_
#define __CSERVICES_H_

#pragma once

#include "Resources/Library/Box2D/Box2D.h"

#include "CCamera.h"
#include "d3dApp.h"
#include "d3dUtil.h"

enum EServiceType
{
	SERVICE_D3DDEVICE,
	SERVICE_CAMERA,
	SERVICE_WORLD,
};

class CServices
{

public:
	static void ProvideService(EServiceType _eService, void* _pService);
	static void GetService(EServiceType _eService, void*& _rpService);
private:
protected:

public:
private:
protected:
	static CCamera*			s_pCamera;
	static ID3D10Device*	s_pDevice;
	static b2World*			s_pB2Word;

};

#endif