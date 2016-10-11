//
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2005 - 2014 Media Design School
//
//	File Name	:	UserInterface.h
//	Description	:	UserInterface implementation file.
//	Author		:	Alex Burnie
//	Mail		:	alex.burnien@mediadesign.school.nz
//

#ifndef UserInterface_H
#define UserInterface_H

#include "d3dUtil.h"
#include "d3dApp.h"

class UserInterface
{
public:

	UserInterface();
	~UserInterface();

	void init(float scale, D3DXVECTOR3 Movement);
	void draw();

	void initShaders();

private:

	void buildFX();
	void buildVertexLayouts();
	void SetShader();

private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10ShaderResourceView* mDiffuseMapRV;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectMatrixVariable* mfxTexMatVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;

	D3DXMATRIX mWorld;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	D3DXMATRIX matTex;
};

#endif // UserInterface_H
