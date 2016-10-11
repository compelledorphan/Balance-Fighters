#pragma once
#include "d3dUtil.h"
#include <vector> 
#include "Player.h"
#include "CXInput.h"
#include "Lives.h"

const unsigned int MAX_PLAYERS = 2;

class CPlayerManager
{
public:
	CPlayerManager(void);
	~CPlayerManager(void);

	void InitPlayerManager();
	void Draw();
	void Process(float _dt);

	void PlayerTexSetup();
	void LoadJake();
	void LoadAlex();

	void BuildPhysicalBodies();
	void CheckAttackConditions();
	void ResetLevel();
	D3DXVECTOR3 GetCentrePoint();
	float GetDistance();

	bool GameOver();

private:
	void buildFX();
	void buildVertexLayouts();

private:

	ID3D10Device* md3dDevice;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10RasterizerState* mRasState;

	//Setting all animations in shader

	//SW BEGIN
	ID3D10ShaderResourceView* mDiffuse_Jake_Attack_1RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Attack_2RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Attack_3RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Jump_1RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Jump_2RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Block_1RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Fall_1RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Fall_2RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Fall_3RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Fall_4RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_HurtRV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Stand_1RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Walk_1RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Walk_2RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Walk_3RV;
	ID3D10ShaderResourceView* mDiffuse_Jake_Walk_4RV;
	//ID3D10ShaderResourceView* mDiffuse_Jake_SelectRV;
	//ID3D10ShaderResourceView* mDiffuse_Jake_WhoopsieRV;

	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Attack_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Attack_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Attack_3Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Jump_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Jump_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Block_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Fall_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Fall_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Fall_3Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Fall_4Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_HurtVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Stand_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Walk_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Walk_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Walk_3Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Walk_4Var;
	//ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_SelectVar;
	//ID3D10EffectShaderResourceVariable* mfxDiffuse_Jake_Whoopsie_Var;
	//SW End

	//Viper BEGIN
	ID3D10ShaderResourceView* mDiffuse_Alex_Attack_1RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Attack_2RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Attack_3RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Jump_1RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Jump_2RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Block_1RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Fall_1RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Fall_2RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Fall_3RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Fall_4RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_HurtRV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Stand_1RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Walk_1RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Walk_2RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Walk_3RV;
	ID3D10ShaderResourceView* mDiffuse_Alex_Walk_4RV;
	//ID3D10ShaderResourceView* mDiffuse_Alex_SelectRV;
	//ID3D10ShaderResourceView* mDiffuse_Alex_WhoopsieRV;

	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Attack_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Attack_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Attack_3Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Jump_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Jump_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Block_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Fall_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Fall_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Fall_3Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Fall_4Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_HurtVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Stand_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Walk_1Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Walk_2Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Walk_3Var;
	ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Walk_4Var;
	//ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_SelectVar;
	//ID3D10EffectShaderResourceVariable* mfxDiffuse_Alex_Whoopsie_Var;
	//Viper End

	//Balance Metre
	ID3D10ShaderResourceView* mDiffuseBalanceMetreRV;
	ID3D10ShaderResourceView* mDiffuseBalanceArrowRV;
	ID3D10EffectShaderResourceVariable* mfxDiffuseBalanceMetreVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseBalanceArrowVar;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	

	D3DXMATRIX mWorld;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	std::vector<CPlayer*> vecPlayers;

	int		iNumTotalPlayers;

	float m_fPlayer1AttackTimer;
	float m_fPlayer2AttackTimer;

	std::vector<Lives*> vecLivesPlayer1;
	std::vector<Lives*> vecLivesPlayer2;

	unsigned int m_uiNumStartLives;
};

