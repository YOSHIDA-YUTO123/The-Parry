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
	CCollision();
	virtual ~CCollision();

	void SetElement(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size = VEC3_NULL);
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetSize(void) const { return GetVector; }
private:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 GetVector;
};

//************************************************
// �����蔻��AABB�N���X
//************************************************
class CCollisionAABB : public CCollision
{
public:
	CCollisionAABB();
	~CCollisionAABB();
	bool Collision(CCollisionAABB* other);
	void SetOldPos(const D3DXVECTOR3 posOld) { m_posOld = posOld; }
private:
	D3DXVECTOR3 m_posOld;
};

//************************************************
// �����蔻��(�~)�N���X
//************************************************
class CCollisionSphere : public CCollision
{
public:
	CCollisionSphere();
	~CCollisionSphere();
	bool Collision(CCollisionSphere* other);
	void SetRadius(const float fRadius) { m_fRadius = fRadius; }
private:
	float m_fRadius; // ���a
};

#endif
