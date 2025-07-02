//================================================
//
// �����蔻�� [collision.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// ���d�C���N���[�h�h�~
//************************************************
#ifndef _COLLISION_H_
#define _COLLISION_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"main.h"
#include"object.h"

//************************************************
// �����蔻��AABB�̃N���X�̒�`
//************************************************
class CCollision
{
public:

	typedef enum
	{
		TYPE_AABB = 0,
		TYPE_SPHERE,
		TYPE_FOV,
		TYPE_MAX
	}TYPE;

	CCollision();
	virtual ~CCollision();

	static CCollision* Create(const D3DXVECTOR3 pos,const TYPE type);
	void SetPos(const D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
private:
	D3DXVECTOR3 m_pos; // �ʒu
	TYPE m_type;	   // �����蔻��̎��
};

//************************************************
// �����蔻��AABB�N���X
//************************************************
class CCollisionAABB : public CCollision
{
public:
	CCollisionAABB();
	~CCollisionAABB();

	static CCollisionAABB* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size);
	bool Collision(CCollisionAABB* other);
	void SetOldPos(const D3DXVECTOR3 posOld) { m_posOld = posOld; }
private:
	D3DXVECTOR3 m_posOld;
	D3DXVECTOR3 m_Size;
};

//************************************************
// �����蔻��(�~)�N���X
//************************************************
class CCollisionSphere : public CCollision
{
public:
	CCollisionSphere();
	~CCollisionSphere();
	static CCollisionSphere* Create(const D3DXVECTOR3 pos, const float fRadius);

	// �R���C�_�[�̍쐬����
	CCollisionSphere CreateCollider(const D3DXVECTOR3 pos, const float fRadius);

	bool Collision(CCollisionSphere* other);
	void SetRadius(const float fRadius) { m_fRadius = fRadius; }
private:
	float m_fRadius; // ���a
};

//************************************************
// �����蔻��(���E)�N���X
//************************************************
class CCollisionFOV : public CCollision
{
public:
	CCollisionFOV();
	~CCollisionFOV();
	static CCollisionFOV* Create(const D3DXVECTOR3 pos,const float fLength);

	bool Collision(const D3DXVECTOR3 pos, const float fAngle,const float fAngleLeft,const float fAngleRight);
private:
	float m_fLength; // ���E�̒���
};

#endif
