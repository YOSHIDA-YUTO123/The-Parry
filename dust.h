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
#include"shadow.h"

//***************************************************
// ���I�N���X�̒�`
//***************************************************
class CRubble : public CObject
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

	CRubble(int nPriority = 3);
	~CRubble();

	static CRubble* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 dir, const int nLife);
	static void Creates(const int nNumDust, const D3DXVECTOR3 move, const D3DXVECTOR3 pos, const int nLife);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	CObjectX* m_pObjectX; // �I�u�W�F�N�gX�ւ̃|�C���^
	CShadow* m_pShadow;	  // �e�N���X�̒�`
	CPosition m_pos;	  // �ʒu�N���X
	CRotation m_rot;	  // �����N���X
	CVelocity m_move;	  // �ړ��N���X

	float m_fDecAlv;	  // �A���t�@�l�̌����X�s�[�h
	int m_nLife;		  // ����
	int m_nMaxLife;		  // �ő��HP
};

#endif

