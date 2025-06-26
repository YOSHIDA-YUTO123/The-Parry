//===================================================
//
// ����,���̕`�� [explosion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"billboard.h"

//***************************************************
// �����A���N���X�̒�`
//***************************************************
class CExplosion : public CBillboardAnimation
{
public:
	CExplosion(int nPriority = 4);
	~CExplosion();

	static CExplosion* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	D3DXCOLOR m_col; // �F
};

//***************************************************
// ���I�̉��N���X�̒�`
//***************************************************
class CDustSmoke : public CObjectBillboard
{
public:
	CDustSmoke(int nPriority = 4);
	~CDustSmoke();

	static CDustSmoke* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	D3DXCOLOR m_col;	// �F
	D3DXVECTOR3 m_move;	// �ړ���
};
#endif