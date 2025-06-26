//===================================================
//
// �G�t�F�N�g [effect.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _EFFECT_H_
#define _EFFECT_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"billboard.h"
#include"transform.h"

//***************************************************
// �I�u�W�F�N�g3D�N���X�̒�`
//***************************************************
class CEffect3D : public CObjectBillboard
{
public:
	CEffect3D(int nPriority = 4);
	~CEffect3D();

	static CEffect3D* Create(const D3DXVECTOR3 pos, const float fRadius,const D3DXVECTOR3 move,const D3DXCOLOR col,const int nLife);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	CVelocity* GetVelocity(void) { return m_pMove; }
private:
	CVelocity *m_pMove;		// �ړ���
	D3DXCOLOR m_col;		// �F
	float m_fRadius;		// ���a
	float m_decRadius;		// �傫���̌����X�s�[�h
	float m_decAlv;			// ���l�̌����X�s�[�h
	int m_nLife;			// ����
};
#endif