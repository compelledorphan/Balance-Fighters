#ifndef CPlayer_H
#define CPlayer_H

#include "Resources/Library/Box2D/Box2D.h"

#include "EMovments.h"
#include "d3dUtil.h"
#include "d3dApp.h"
#include "Level.h"
#include "Vertex.h"
#include <vector>
#include <list>

enum EPLAYER_CURRENT_CHARACTER
{
	EPLAYER_CURRENT_CHARACTER_Jake,
	EPLAYER_CURRENT_CHARACTER_Alex,

	EPLAYER_CURRENT_CHARACTER_MAX
};

enum EPLAYER_ANIMATION_STATE
{
	EPLAYER_ANIMATION_STATE_STANDING,
	EPLAYER_ANIMATION_STATE_WALKING,
	EPLAYER_ANIMATION_STATE_JUMPING,
	EPLAYER_ANIMATION_STATE_ATTACKING,
	EPLAYER_ANIMATION_STATE_BLOCK,
	EPLAYER_ANIMATION_STATE_HURT,
	EPLAYER_ANIMATION_STATE_FALLING,

	EPLAYER_ANIMATION_STATE_MAX
};

class CPlayer
{
public:

	CPlayer(EPLAYER_ANIMATION_STATE _ePlayerType);
	~CPlayer(void);

	void init(ID3D10Device* device, float scale);
	void draw();

	EPLAYER_ANIMATION_STATE getPlayerType();
	void SetPlayerType(EPLAYER_ANIMATION_STATE _type);

	float CalculateRotationAngle();

	D3DXVECTOR3 GetPos();
	D3DXVECTOR3 GetDirection();
	D3DXCOLOR GetColour();
	float GetSize();

	void SetOrigin(D3DXVECTOR3 _pos);
	void SetPos(D3DXVECTOR3 _pos);
	void SetDirection(D3DXVECTOR3 _dir);
	void SetSize();

	void UpdateAll();

	void SetBody();
	void DeleteBody();
	void Process(float _dt);
	void Animate(float _dt);

	void BalanceMechanicInit();
	void BalanceMechanicProcess(float _dt);

	void Move(e_Movement _Movement);
	void Jump(bool _b);
	void Balance(float _f);
	void SetAttackAnim();
	void SetAttackAllow(bool _b);
	bool GetAttackAllowState();
	void SetRightFacing(bool _faceright);
	bool FacingRightState();
	void SetAnimStateHurt();
	bool IsAlive();
	void Reset();

	b2Vec2 Getb2Pos();
	b2Body* GetBody();

	void SetPlayer(EPLAYER_CURRENT_CHARACTER _character);

	void SetPlayerLives(unsigned int _uiNumLives);
	unsigned int GetNumLives();

public:

private:
	DWORD mNumVertices;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;

	EPLAYER_ANIMATION_STATE ePlayerType;

	// Create vertex buffer
    Vertex v[1];

	D3DXVECTOR3 mPos;
	D3DXVECTOR3 mDirection;
	float mfSize;
	float m_fAnimationTimer;
	unsigned int m_uiTextIndex; 

	b2Body*		m_pBody;
	b2World*    m_pWorld;
	b2FixtureDef BoxFixtureDef;
	b2Fixture* BoxFix;
	b2BodyDef TempBodyDef;
	b2PolygonShape TempBox;

	float m_fBalanceStatus;
	float m_fBM_LENGTH;
	float m_fBM_MAXLENGTH;
	float m_fBM_WEIGHT;
	float m_fBM_GRAVITY;
	float m_fBM_TORQUE;
	float m_fBM_INERTIA;
	float m_fBM_ACCELERATION;
	float m_fBM_CURRENTFORCE;
	float m_fBM_MAXFORCE;

	float m_fAttackDamage;

	float m_fBALANCEDIFICULTY;

	float m_MovementForce;
	float m_MJumpForce;

	float m_ControllerDirection;

	EPLAYER_CURRENT_CHARACTER m_eWhoAmI;

	bool m_fAttackAllowed;
	bool m_bAttacking;

	bool m_bFaceRight;

	D3DXVECTOR3 m_vOrigin;

	unsigned int m_uiNumLives;
};

#endif // CPlayer_H