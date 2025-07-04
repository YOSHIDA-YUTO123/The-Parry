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
#include"effect.h"

using namespace math; // ���O��Ԃ��g�p

//================================================
// �R���X�g���N�^
//================================================
CCollision::CCollision()
{
	m_pos = VEC3_NULL;
	m_type = TYPE::TYPE_AABB;
}

//================================================
// �f�X�g���N�^
//================================================
CCollision::~CCollision()
{
}

//================================================
// �����蔻��̐�������
//================================================
CCollision* CCollision::Create(const D3DXVECTOR3 pos, const TYPE type)
{
	// �����蔻��̃|�C���^
	CCollision* pCollision = nullptr;

	// ��ނ̑J��
	switch (type)
	{
	case TYPE::TYPE_AABB:
		pCollision = new CCollisionAABB;
		break;
	case TYPE::TYPE_SPHERE:
		pCollision = new CCollisionSphere;
		break;
	case TYPE::TYPE_FOV:
		pCollision = new CCollisionFOV;
		break;
	default:
		break;
	}

	// �ʒu�̐ݒ�
	pCollision->m_pos = pos;

	// ��ނ���
	pCollision->m_type = type;

	// �����Ԃ�
	return pCollision;
}

//================================================
// �R���X�g���N�^
//================================================
CCollisionAABB::CCollisionAABB()
{
	m_Size = VEC3_NULL;
	m_posOld = VEC3_NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CCollisionAABB::~CCollisionAABB()
{
}

//================================================
// AABB�̍쐬����
//================================================
CCollisionAABB* CCollisionAABB::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size)
{
	// AABB�̍쐬����
	CCollisionAABB* pAABB = new CCollisionAABB;

	// �ݒ菈��
	pAABB->SetPos(pos);
	pAABB->m_Size = Size;

	return pAABB;
}

//================================================
// AABB��AABB�̓����蔻��
//================================================
bool CCollisionAABB::Collision(CCollisionAABB* other)
{
	// �����̈ʒu�Ƒ傫���̎擾
	D3DXVECTOR3 pos = GetPosition();

	// ����̈ʒu�Ƒ傫���̎擾
	D3DXVECTOR3 otherPos = other->GetPosition();
	D3DXVECTOR3 otherSize = other->m_Size;

	bool bHit = false;

	const float HALF_VALUE = 0.5f;

	if (m_posOld.y <= otherPos.y + otherSize.y
		&& m_posOld.y + m_Size.y >= otherPos.y)
	{
		// ���E�̂߂荞�ݔ���
		if (pos.z - m_Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + m_Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
		{
			// x��������E�ɂ߂荞��	
			if (m_posOld.x + m_Size.x * HALF_VALUE < otherPos.x - otherSize.x * HALF_VALUE
				&& pos.x + m_Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x - otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x - Size.x * HALF_VALUE - 0.1f;
			}
			// x���E���獶�ɂ߂荞��	
			else if (m_posOld.x - m_Size.x * HALF_VALUE > otherPos.x + otherSize.x * HALF_VALUE
				&& pos.x - m_Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x + otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x + Size.x * HALF_VALUE + 0.1f;
			}
		}

		// �O�ƌ��̔���
		if (pos.x - m_Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE
			&& pos.x + m_Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
		{
			// z���O������߂荞��
			if (m_posOld.z + m_Size.z * HALF_VALUE < otherPos.z - otherSize.z * HALF_VALUE
				&& pos.z + m_Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z - otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z - Size.z * HALF_VALUE - 0.1f;
			}
			// z���������߂荞��
			else if (m_posOld.z - m_Size.z * HALF_VALUE > otherPos.z + otherSize.z * HALF_VALUE
				&& pos.z - m_Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z + otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z + Size.z * HALF_VALUE + HALF_VALUE;
			}
		}
	}

	if (pos.x - m_Size.x * HALF_VALUE <= otherPos.x + otherSize.x * HALF_VALUE
		&& pos.x + m_Size.x * HALF_VALUE >= otherPos.x - otherSize.x * HALF_VALUE)
	{
		if (pos.z - m_Size.z * HALF_VALUE <= otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + m_Size.z * HALF_VALUE >= otherPos.z - otherSize.z * HALF_VALUE)
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
			else if (m_posOld.y + m_Size.y * HALF_VALUE <= otherPos.y - otherSize.y * HALF_VALUE
				&& pos.y + m_Size.y * HALF_VALUE > otherPos.y - otherSize.y * HALF_VALUE)
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
// �~�̔���̐�������
//================================================
std::unique_ptr<CCollisionSphere> CCollisionSphere::Create(const D3DXVECTOR3 pos, const float fRadius)
{
	// �|�C���^�̐���
	std::unique_ptr<CCollisionSphere> out = std::make_unique<CCollisionSphere>();

	// �ݒ菈��
	out->SetPos(pos);
	out->m_fRadius = fRadius;
	return out;
}

//================================================
// �R���C�_�[�̐�������
//================================================
CCollisionSphere CCollisionSphere::CreateCollider(const D3DXVECTOR3 pos, const float fRadius)
{
	// �A�E�g�v�b�g�p
	CCollisionSphere sphere;

	// ����ɕK�v�ȏ��̐ݒ�
	sphere.SetPos(pos);
	sphere.m_fRadius = fRadius;

	// �R���C�_�[��Ԃ�
	return sphere;
}

//================================================
// �����蔻��(�~vs�~)
//================================================
bool CCollisionSphere::Collision(CCollisionSphere* other)
{
	// �ʒu�̎擾
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 otherPos = other->GetPosition();

	// ���������߂�
	D3DXVECTOR3 diff = otherPos - pos;

	// ���������߂�
	float fDistance = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

	// ���a����
	float fRadius = m_fRadius + other->m_fRadius;

	// ���a��2�悷��
	fRadius = fRadius * fRadius;

	// ���������a�ȉ��������瓖�����Ă���
	if (fDistance <= fRadius)
	{
		return true;
	}

	return false;
}

//================================================
// �R���X�g���N�^
//================================================
CCollisionFOV::CCollisionFOV()
{
	m_fLength = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CCollisionFOV::~CCollisionFOV()
{
}

//================================================
// ���E�̐���
//================================================
std::unique_ptr<CCollisionFOV> CCollisionFOV::Create(const D3DXVECTOR3 pos, const float fLength)
{
	// �|�C���^�̐���
	std::unique_ptr<CCollisionFOV> out = std::make_unique<CCollisionFOV>();

	// �ݒ菈��
	out->SetPos(pos);
	out->m_fLength = fLength;
	return out;
}

////================================================
//// ��������
////================================================
//CCollisionFOV* CCollisionFOV::Create(const D3DXVECTOR3 pos, const float fLength)
//{
//	//// ���E����̐���
//	//CCollisionFOV* pCollision = new CCollisionFOV;
//
//	//pCollision->SetPos(pos);
//	//pCollision->m_fLength = fLength;
//
//	//return pCollision;
//}
//
//================================================
// ���E�̔���
//================================================
bool CCollisionFOV::Collision(const D3DXVECTOR3 pos,const float fAngle, const float fAngleLeft, const float fAngleRight)
{
	// �ʒu�̎擾
	D3DXVECTOR3 objectPos = GetPosition();

	// �O���܂ł̃x�N�g��
	D3DXVECTOR3 vecFront = GetVector(pos, objectPos);

	D3DXVECTOR3 LeftPos; // ���̈ʒu

	// �����̎��E�̒[�̈ʒu�����߂�
	LeftPos.x = objectPos.x + sinf(fAngle + fAngleLeft) * m_fLength;
	LeftPos.y = 0.0f;
	LeftPos.z = objectPos.z + cosf(fAngle + fAngleLeft) * m_fLength;

	// �����̎��E�̃x�N�g���̍쐬
	D3DXVECTOR3 VecLeft = GetVector(LeftPos, objectPos);

	D3DXVECTOR3 RightPos; // �E�̈ʒu

	// �E���̎��E�̒[�̈ʒu�����߂�
	RightPos.x = objectPos.x + sinf(fAngle + fAngleRight) * m_fLength;
	RightPos.y = 0.0f;
	RightPos.z = objectPos.z + cosf(fAngle + fAngleRight) * m_fLength;

	// �E���̎��E�̃x�N�g���̍쐬
	D3DXVECTOR3 VecRight = GetVector(RightPos, objectPos);

	D3DXVECTOR3 Cross0,Cross1; // ����p�O�σx�N�g��

#ifdef _DEBUG

	CEffect3D::Create(RightPos, 50.0f, VEC3_NULL, WHITE, 10);
	CEffect3D::Create(LeftPos, 50.0f, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

	// �O�ς��o��
	D3DXVec3Cross(&Cross0, &VecLeft, &vecFront);
	D3DXVec3Cross(&Cross1, &vecFront, &VecRight);

	// 2�{�Ƃ��������������
	if (Cross0.y >= 0.0f && Cross1.y >= 0.0f)
	{
		return true;
	}

	return false;
}
