//================================================
//
// �g�����X�t�H�[���𐧌䂷�鏈�� [transform.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

//*************************************************
// �C���N���[�h�t�@�C��
//*************************************************
#include"main.h"

//*************************************************
// �ʒu�̃N���X�̒�`
//*************************************************
class CPosition
{
public:
	CPosition();
	~CPosition();

	D3DXVECTOR3 Get(void) const { return m_pos; }
	void Set(const D3DXVECTOR3 pos) { m_pos = pos; }

	void UpdatePosition(const D3DXVECTOR3 move);
private:
	D3DXVECTOR3 m_pos; // �ʒu
};

//*************************************************
// ��]�̃N���X�̒�`
//*************************************************
class CRotation
{
public:
	CRotation();
	~CRotation();

	D3DXVECTOR3 Get(void) const { return m_rot; }
	void Set(const D3DXVECTOR3 rot) { m_rot = rot; }
	void Add(const D3DXVECTOR3 rot) { m_rot += rot; }
	void SetDest(const D3DXVECTOR3 dest) { m_Dest = dest; }
	D3DXVECTOR3 GetDest(void) const { return m_Dest; }
	void SetSmoothAngle(const float coef);
private:
	D3DXVECTOR3 m_rot;	   // ����
	D3DXVECTOR3 m_Dest; // �ړI�̌���
};

//*************************************************
// �ړ��̃N���X�̒�`
//*************************************************
class CVelocity
{
public:
	CVelocity();
	~CVelocity();

	D3DXVECTOR3 Get(void) const { return m_move; }
	void Set(const D3DXVECTOR3 move) { m_move = move; }
	void Add(const D3DXVECTOR3 move) { m_move += move; }

	void SetInertia3D(const float fcoef);
	void SetInertia2D(const float fcoef);
	void Gravity(const float gravity);
	void Bound(D3DXVECTOR3 nor, float coef = 0.8f);
	void Jump(const float jumpHeight) { m_move.y = jumpHeight; }
private:
	D3DXVECTOR3 m_move; // �ړ���
};

//*************************************************
// �傫����(�~�`)�N���X�̒�`
//*************************************************
class CSizeCircle
{
public:
	CSizeCircle();
	~CSizeCircle();

	float Get(void) const { return m_fRadius; }
	void Set(const float fRadius) { m_fRadius = fRadius; }
private:
	float m_fRadius; // �傫��
};

//*************************************************
// �傫����2D�N���X�̒�`
//*************************************************
class CSize2D
{
public:
	CSize2D();
	~CSize2D();

	D3DXVECTOR2 Get(void) const { return D3DXVECTOR2(m_fWidth, m_fHeight); }
	void Set(const float fWidth,const float fHeight) { m_fWidth = fWidth , m_fHeight = fHeight; }
private:
	float m_fWidth,m_fHeight; // �傫��
};

//*************************************************
// �傫����3D�N���X�̒�`
//*************************************************
class CSize3D
{
public:
	CSize3D();
	~CSize3D();

	D3DXVECTOR3 Get(void) const { return GetVector; }
	void Set(const D3DXVECTOR3 SIze) { GetVector = SIze; }
private:
	D3DXVECTOR3 GetVector;
};

#endif