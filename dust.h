//===================================================
//
// ���I [dust.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _DUST_H_
#define _DUST_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "main.h"
#include "object.h"
#include"transform.h"
#include"objectX.h"

//===================================================
// ���I�N���X�̒�`
//===================================================
class CDust : public CObject
{
public:

	// ���I�̎��
	typedef enum
	{
		TYPE_ONE = 0,
		TYPE_TWO,
		TYPE_THREE,
		TYPE_MAX
	}TYPE;

	CDust(int nPriority = 1);
	~CDust();

	static CDust* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 dir, const int nLife);
	static void Creates(const int nNumDust, const D3DXVECTOR3 move, const D3DXVECTOR3 pos, const int nLife);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	CObjectX* m_pObjectX; // �I�u�W�F�N�gX�ւ̃|�C���^
	CPosition m_pos;	  // �ʒu�N���X
	CRotation m_rot;	  // �����N���X
	CVelocity m_move;	  // �ړ��N���X

	// D3DXVECTOR3 m_pos // �ʒu
	//D3DXVECTOR3 m_rot;	  // ����
	//D3DXVECTOR3 m_move;	  // �ړ���
	float m_fDecAlv;	  // �A���t�@�l�̌����X�s�[�h
	int m_nLife;		  // ����
	int m_nMaxLife;		  // �ő��HP
};

#endif

