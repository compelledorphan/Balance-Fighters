#include "Player.h"
#include "CServices.h"


CPlayer::CPlayer(EPLAYER_ANIMATION_STATE _ePlayerType)
	: mNumVertices(0), md3dDevice(0), mVB(0), m_fAnimationTimer (0.0f), m_uiTextIndex(0), m_uiNumLives(3)
{
	ePlayerType = _ePlayerType;

	m_MovementForce = 0.10f;
	m_MJumpForce = 1000.0f;
	m_fAttackAllowed = true;
	m_bAttacking = false;
	m_fAttackDamage = 0.25f;
}

CPlayer::~CPlayer(void)
{
	ReleaseCOM(mVB);
}

void CPlayer::init(ID3D10Device* device, float scale)
{
	md3dDevice = device;

	BalanceMechanicInit();

	mNumVertices = 1;

	// Fill in the front face vertex data.
	// DEFAULTS
	v[0] = Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 5.0f);
	v[0].uiTexIndex = m_uiTextIndex;
	int iTemp = Level_SIZE;

	SetPos(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	SetSize();

	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
}

void CPlayer::draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->Draw(1, 0);
}

EPLAYER_ANIMATION_STATE CPlayer::getPlayerType()
{
	return ePlayerType;
}

void CPlayer::SetPlayerType(EPLAYER_ANIMATION_STATE _type)
{
	ePlayerType = _type;
}

D3DXVECTOR3 CPlayer::GetPos()
{
	return mPos;
}

D3DXVECTOR3 CPlayer::GetDirection()
{
	return mDirection;
}

float CPlayer::GetSize()
{
	return mfSize;
}

void CPlayer::SetPos(D3DXVECTOR3 _pos)
{
	mPos = _pos;
}

void CPlayer::SetDirection(D3DXVECTOR3 _dir)
{
	mDirection = _dir;
}

void CPlayer::SetSize()
{
	mfSize = 7.0f;
}

void CPlayer::UpdateAll()
{
	SetSize();

	Vertex* vTemp = nullptr;

	mVB->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vTemp);

	vTemp[0] = Vertex(mPos.x, mPos.y, mPos.z, mDirection.x, mDirection.y, mDirection.z, 0.0f, 0.0f, mfSize);

	if ((m_eWhoAmI == EPLAYER_CURRENT_CHARACTER_Alex) && (m_uiTextIndex < 50))
	{
		vTemp[0].uiTexIndex = (m_uiTextIndex + 50);
	}
	else
	{
		vTemp[0].uiTexIndex = m_uiTextIndex;
	}

	vTemp[0].fBalanceStatus = m_fBalanceStatus;

	if (FacingRightState())
	{
		vTemp[0].iFacingDirection = 1;
	}
	else
	{
		vTemp[0].iFacingDirection = -1;
	}

	mVB->Unmap();
}

void CPlayer::SetBody()
{
	/* Get World*/
	void* vpWorld;
	CServices::GetService(SERVICE_WORLD, vpWorld);
	m_pWorld = static_cast<b2World*>(vpWorld);

	/* create Dynamic Box*/
	TempBodyDef.type = b2_dynamicBody;
	TempBodyDef.position.Set(mPos.x, mPos.y);

	TempBox.SetAsBox(2.0f, 5.0f);

	m_pBody = m_pWorld->CreateBody(&TempBodyDef);

	BoxFixtureDef.shape = &TempBox;
	BoxFixtureDef.density = 1;
	BoxFix = m_pBody->CreateFixture(&BoxFixtureDef);

	m_pBody->SetFixedRotation(true);
}

void CPlayer::DeleteBody()
{
	b2Filter filter;
	filter.maskBits = 0x0000;
	//filter.maskBits = 0xffff;//Sets to collide with everything
	//BoxFixtureDef.filter.maskBits = 0x0000;
	m_pBody->GetFixtureList()->SetFilterData(filter);
}

void CPlayer::Process(float _dt)
{
	//Needs attack

	Animate(_dt);

	BalanceMechanicProcess(_dt);

	//Physics
	b2Vec2 position = m_pBody->GetPosition();
	mPos.x = position.x;
	mPos.y = position.y;
}

void CPlayer::Balance(float _f)
{
	// TODO: Sneaky add in here
	m_ControllerDirection = _f/5;

}

void CPlayer::Jump(bool _b)
{
	if(_b)
	{
		b2Vec2 Velocity = m_pBody->GetLinearVelocity();

		if(Velocity.y < 0.0001 && Velocity.y > -0.0001)
		{
			m_pBody->ApplyLinearImpulse( b2Vec2(0, m_MJumpForce), m_pBody->GetWorldCenter(), true);
			if (m_eWhoAmI == EPLAYER_CURRENT_CHARACTER_Jake)
			{
				CSoundManager::GetInstance().PlayLoadedSound(PlayerTwo, Jumping);
			}
			else
			{
				CSoundManager::GetInstance().PlayLoadedSound(PlayerOne, Jumping);
			}
			ePlayerType = EPLAYER_ANIMATION_STATE_JUMPING;
		}
	}
}

void CPlayer::Move(e_Movement _movment)
{
	//Controller Apply a force to move players

	switch(_movment)
	{
	case MOVE_LEFT:
		{
			if(m_pBody->GetPosition().y <= -10)
			{
				m_pBody->ApplyLinearImpulse( b2Vec2(-m_MovementForce * 2, 0.0f), m_pBody->GetWorldCenter(), true);

				if ((m_bAttacking == false) && (ePlayerType != EPLAYER_ANIMATION_STATE_HURT))
				{
					ePlayerType = EPLAYER_ANIMATION_STATE_WALKING;
				}
			}
			else
			{
				m_pBody->ApplyLinearImpulse( b2Vec2(-m_MovementForce, 0.0f), m_pBody->GetWorldCenter(), true);
			}
		}
		break;
	case MOVE_RIGHT:
		{
			if(m_pBody->GetPosition().y <= -10)
			{
				m_pBody->ApplyLinearImpulse( b2Vec2(m_MovementForce * 2, 0.0f), m_pBody->GetWorldCenter(), true);

				if ((m_bAttacking == false) && (ePlayerType != EPLAYER_ANIMATION_STATE_HURT))
				{
					ePlayerType = EPLAYER_ANIMATION_STATE_WALKING;
				}
			}
			else
			{
				m_pBody->ApplyLinearImpulse( b2Vec2(m_MovementForce, 0.0f), m_pBody->GetWorldCenter(), true);
			}
		}
		break;
	case NO_MOVE:
		{
			if (m_bAttacking == false)
			{
				if((m_pBody->GetPosition().y <= -10) && (ePlayerType != EPLAYER_ANIMATION_STATE_HURT))
				{	
					ePlayerType = EPLAYER_ANIMATION_STATE_STANDING;
				}
				else
				{
					ePlayerType = EPLAYER_ANIMATION_STATE_JUMPING;
				}
			}
		}
		break;
	default:break;
	}
}


void CPlayer::Animate(float _dt)
{

	m_fAnimationTimer += _dt;

	switch (ePlayerType)
	{
	case EPLAYER_ANIMATION_STATE_STANDING:
		{
			//Standing anim
			m_uiTextIndex = 11;
		}
		break;
	case EPLAYER_ANIMATION_STATE_WALKING:
		{
			//walking anim
			if (m_fAnimationTimer > 0.3f)
			{
				if (m_uiTextIndex == 15)
				{
					m_uiTextIndex = 12;
				}
				else if (m_uiTextIndex >= 12)
				{
					m_uiTextIndex++;
				}
				else
				{
					m_uiTextIndex = 12;
				}
				m_fAnimationTimer = 0.0f;
			}
		}
		break;
	case EPLAYER_ANIMATION_STATE_JUMPING:
		{
			//Jumping anim
			m_uiTextIndex = 10;
		}
		break;
	case EPLAYER_ANIMATION_STATE_ATTACKING:
		{
			//Attacking anim
			if (m_fAnimationTimer > 0.1f)
			{
				if (m_uiTextIndex == 2)
				{
					m_bAttacking = false;
				}
				else if (m_uiTextIndex <= 1)
				{
					m_uiTextIndex++;
				}
				else
				{
					m_uiTextIndex = 0;
					m_bAttacking = true;
				}
				m_fAnimationTimer = 0.0f;
			}
		}
		break;
	case EPLAYER_ANIMATION_STATE_BLOCK:
		{
			//Blocking anim
			m_uiTextIndex = 0;
		}
		break;
	case EPLAYER_ANIMATION_STATE_HURT:
		{
			//Hurt
			m_uiTextIndex = 8;
			if (m_fAnimationTimer > 1.0f)
			{
				m_fAnimationTimer = 0.0f;
				ePlayerType = EPLAYER_ANIMATION_STATE_STANDING;
			}

		}
		break;
	case EPLAYER_ANIMATION_STATE_FALLING:
		{
			//walking anim
			if (m_fAnimationTimer > 0.2f)
			{
				if (m_uiTextIndex == 7)
				{
					m_uiTextIndex = 4;
				}
				else if (m_uiTextIndex >= 7)
				{
					m_uiTextIndex++;
				}
				else
				{
					m_uiTextIndex = 4;
				}
				m_fAnimationTimer = 0.0f;
			}
		}
		break;
	default:
		{
			m_uiTextIndex = 0;
		}
		break;
	}
}

void CPlayer::BalanceMechanicInit()
{
	m_fBALANCEDIFICULTY = 1.0f;
	m_fBalanceStatus = -0.001f;

	//Constants
	m_fBM_MAXLENGTH = 1.0f;
	m_fBM_WEIGHT = 1.0f;
	m_fBM_GRAVITY = 500.0f;
	m_fBM_MAXFORCE = (m_fBM_WEIGHT * m_fBM_GRAVITY) * m_fBM_MAXLENGTH;

	//Variables
	m_fBM_LENGTH = 0.0f;
	m_fBM_TORQUE = 0.0f;
	m_fBM_INERTIA = 0.0f;
	m_fBM_ACCELERATION = 0.0f;
	m_fBM_CURRENTFORCE = 0.0f;
}

void CPlayer::BalanceMechanicProcess(float _dt)
{
	m_fBM_LENGTH = (sqrt(m_fBalanceStatus * m_fBalanceStatus));
	m_fBM_CURRENTFORCE = m_fBalanceStatus * m_fBM_GRAVITY;

	m_fBM_ACCELERATION = ( m_fBM_CURRENTFORCE / (m_fBM_WEIGHT * m_fBM_GRAVITY) );

	m_fBM_CURRENTFORCE += (m_fBM_CURRENTFORCE * _dt * m_fBALANCEDIFICULTY);

	m_fBM_CURRENTFORCE += m_ControllerDirection;

	m_fBalanceStatus = m_fBM_CURRENTFORCE / m_fBM_MAXFORCE;

	if (m_fBalanceStatus >= 1.0f)
	{
		m_fBalanceStatus = 1.0f;
		DeleteBody();
		//m_fBM_CURRENTFORCE = m_fBM_MAXFORCE;
	}
	else if (m_fBalanceStatus <= -1.0f)
	{
		m_fBalanceStatus = -1.0f;
		DeleteBody();
		//m_fBM_CURRENTFORCE = (m_fBM_MAXFORCE * -1);
	}
	else
	{
		b2Filter filter;
		filter.maskBits = 0xffff;
		//filter.maskBits = 0xffff;//Sets to collide with everything
		//BoxFixtureDef.filter.maskBits = 0x0000;
		m_pBody->GetFixtureList()->SetFilterData(filter);
	}
}

b2Vec2 CPlayer::Getb2Pos()
{
	return(b2Vec2(mPos.x, mPos.y));
}

b2Body* CPlayer::GetBody()
{
	return(m_pBody);
}

void CPlayer::SetPlayer(EPLAYER_CURRENT_CHARACTER _character)
{
	m_eWhoAmI = _character;
}

void CPlayer::SetAttackAnim()
{
	ePlayerType = EPLAYER_ANIMATION_STATE_ATTACKING;
}

void CPlayer::SetAttackAllow(bool _b)
{
	m_fAttackAllowed = _b;
}

bool CPlayer::GetAttackAllowState()
{
	return m_fAttackAllowed;
}

void CPlayer::SetRightFacing(bool _faceright)
{
	m_bFaceRight = _faceright;
}

bool CPlayer::FacingRightState()
{
	return m_bFaceRight;
}

void CPlayer::SetAnimStateHurt()
{
	ePlayerType = EPLAYER_ANIMATION_STATE_HURT;

	if (m_fBalanceStatus > 0.0f)
	{
		m_fBalanceStatus += m_fAttackDamage;
	}
	else
	{
		m_fBalanceStatus -= m_fAttackDamage;
	}

}

bool CPlayer::IsAlive()
{
	if (mPos.y < -100)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CPlayer::Reset()
{
	m_pBody->DestroyFixture(BoxFix);
	m_pBody->GetWorld()->DestroyBody(m_pBody);
	SetPos(m_vOrigin);
	SetBody();
	//b2Filter filter;
	//filter.maskBits = 0xffff;	//Sets to collide with everything
	//m_pBody->GetFixtureList()->SetFilterData(filter);

	m_fBalanceStatus = 0.01f;

	UpdateAll();

}

void CPlayer::SetOrigin(D3DXVECTOR3 _pos)
{
	m_vOrigin = _pos;
}

void CPlayer::SetPlayerLives(unsigned int _uiNumLives)
{
	m_uiNumLives = _uiNumLives;
}

unsigned int CPlayer::GetNumLives()
{
	return m_uiNumLives;
}