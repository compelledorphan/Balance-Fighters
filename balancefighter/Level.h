#ifndef CLevel_H
#define CLevel_H

#include "Resources/Library/Box2D/Box2D.h"

#include "d3dUtil.h"
#include "d3dApp.h"

#define Level_SIZE	84;  //Ranges from -64 to 64

class CLevel
{
public:

	CLevel();
	~CLevel();

	void initShader();
	void initVerts(float scale);
	void draw();
	void Process(float _dt);
	void initWorld(float _x, float _y);
	void AddBarrier(float posx, float posy, float w, float h);

private:

	void buildFX();
	void buildVertexLayouts();
	void SetShader();

public:
	bool m_bMusic;

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

	b2World*	m_World;
	b2Vec2		m_Gravity;

	
};

#endif // CLevel_H
