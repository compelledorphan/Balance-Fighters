#include "CServices.h"

CCamera*		CServices::s_pCamera	= nullptr;
ID3D10Device*	CServices::s_pDevice	= nullptr;
b2World*		CServices::s_pB2Word    = nullptr;

void CServices::ProvideService(EServiceType _eService, void* _pService)
{
	switch (_eService)
	{
		case SERVICE_D3DDEVICE:
		{
			s_pCamera = static_cast<CCamera*>(_pService);
		}
		break;
		case SERVICE_CAMERA:
		{
			s_pDevice = static_cast<ID3D10Device*>(_pService);
		}
		break;
		case SERVICE_WORLD:
		{
			s_pB2Word = static_cast<b2World*>(_pService);
		}
		break;

		default: break;
	}
}

void CServices::GetService(EServiceType _eService, void*& _rpService)
{
	switch (_eService)
	{

		case SERVICE_D3DDEVICE:
		{
			_rpService = static_cast<void*>(s_pCamera);
		}
		break;
		case SERVICE_CAMERA:
		{
			_rpService = static_cast<void*>(s_pDevice);
		}
		break;
		case SERVICE_WORLD:
		{
			_rpService = static_cast<void*>(s_pB2Word);
		}
		break;

		default: break;
	}
}