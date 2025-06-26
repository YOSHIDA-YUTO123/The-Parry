//================================================
//
// �����蔻�� [collision.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "Collision.h"
#include"math.h"

using namespace math; // ���O��Ԃ��g�p

//================================================
// �R���X�g���N�^
//================================================
CCollision::CCollision()
{
	m_pos = VEC3_NULL;
	GetVector = VEC3_NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CCollision::~CCollision()
{
}

//================================================
// �v�f�̐ݒ菈��
//================================================
void CCollision::SetElement(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size)
{
	m_pos = pos;
	GetVector = Size;
}

//================================================
// �R���X�g���N�^
//================================================
CCollisionAABB::CCollisionAABB()
{
	m_posOld = VEC3_NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CCollisionAABB::~CCollisionAABB()
{
}

//================================================
// AABB��AABB�̓����蔻��
//================================================
bool CCollisionAABB::Collision(CCollisionAABB* other)
{
	// �����̈ʒu�Ƒ傫���̎擾
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// ����̈ʒu�Ƒ傫���̎擾
	D3DXVECTOR3 otherPos = other->GetPosition();
	D3DXVECTOR3 otherSize = other->GetSize();

	bool bHit = false;

	const float HALF_VALUE = 0.5f;

	if (m_posOld.y <= otherPos.y + otherSize.y
		&& m_posOld.y + Size.y >= otherPos.y)
	{
		// ���E�̂߂荞�ݔ���
		if (pos.z - Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
		{
			// x��������E�ɂ߂荞��	
			if (m_posOld.x + Size.x * HALF_VALUE < otherPos.x - otherSize.x * HALF_VALUE
				&& pos.x + Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x - otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x - Size.x * HALF_VALUE - 0.1f;
			}
			// x���E���獶�ɂ߂荞��	
			else if (m_posOld.x - Size.x * HALF_VALUE > otherPos.x + otherSize.x * HALF_VALUE
				&& pos.x - Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x + otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x + Size.x * HALF_VALUE + 0.1f;
			}
		}

		// �O�ƌ��̔���
		if (pos.x - Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE
			&& pos.x + Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
		{
			// z���O������߂荞��
			if (m_posOld.z + Size.z * HALF_VALUE < otherPos.z - otherSize.z * HALF_VALUE
				&& pos.z + Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z - otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z - Size.z * HALF_VALUE - 0.1f;
			}
			// z���������߂荞��
			else if (m_posOld.z - Size.z * HALF_VALUE > otherPos.z + otherSize.z * HALF_VALUE
				&& pos.z - Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z + otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z + Size.z * HALF_VALUE + HALF_VALUE;
			}
		}
	}

	if (pos.x - Size.x * HALF_VALUE <= otherPos.x + otherSize.x * HALF_VALUE
		&& pos.x + Size.x * HALF_VALUE >= otherPos.x - otherSize.x * HALF_VALUE)
	{
		if (pos.z - Size.z * HALF_VALUE <= otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + Size.z * HALF_VALUE >= otherPos.z - otherSize.z * HALF_VALUE)
		{
			// �ォ�牺
			if (m_posOld.y >= otherPos.y + otherSize.y
				&& pos.y < otherPos.y + otherSize.y)
			{
				bHit = true;

				//bLanding = true;
				//pos.y = otherPos.y + otherSize.y * g_Block[nCntBlock].Scal.y - Size.y;
				//pMove->y = 0.0f;
			}
			// �������
			else if (m_posOld.y + Size.y * HALF_VALUE <= otherPos.y - otherSize.y * HALF_VALUE
				&& pos.y + Size.y * HALF_VALUE > otherPos.y - otherSize.y * HALF_VALUE)
			{
				bHit = true;

				//pos.y = m_posOld.y;
				//pMove->y = 0.0f;
			}

		}
	}
	
	return bHit;
}

//================================================
// �R���X�g���N�^
//================================================
CCollisionSphere::CCollisionSphere()
{
	m_fRadius = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CCollisionSphere::~CCollisionSphere()
{
}

//================================================
// �����蔻��(�~vs�~)
//================================================
bool CCollisionSphere::Collision(CCollisionSphere* other)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 otherPos = other->GetPosition();

	D3DXVECTOR3 diff = otherPos - pos;

	float fDistance = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

	float fRadius = m_fRadius + other->m_fRadius;

	fRadius = fRadius * fRadius;

	if (fDistance <= fRadius)
	{
		return true;
	}

	return false;
}
