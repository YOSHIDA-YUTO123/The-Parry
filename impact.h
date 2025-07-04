//================================================
//
// ���b�V���C���p�N�g [impact.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _IMPACT_H_
#define _IMPACT_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"mesh.h"

//************************************************
// ���b�V���T�[�N��(����)�N���X�̒�`
//************************************************
class CMeshCircle : public CMesh
{
public:

	// �T�[�N���̍\��(�����A�����A���a�����A���a�O���A�����A�n�ʔ���)
	struct Confing
	{
		float fHeight;			 // ����
		float fSpeed;			 // ����
		float fInRadius;		 // �����̔��a
		float fOutRadius;		 // �O���̔��a
		int nLife;				 // ����
		bool bFiledCollision;	 // �n�ʂ̍����ɍ��킹�邩�ǂ���
	};

	CMeshCircle();
	~CMeshCircle();

	static CMeshCircle* Create(const Confing confing,const D3DXCOLOR col, const D3DXVECTOR3 pos,const int nSegX = 32, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	void SetCircle(const int nSegX, const float InRadius, const float OutRadius);  // ���_�̐ݒ�
	Confing m_Config;	// �\��
	D3DXCOLOR m_Incol, m_Outcol; // �F
	float m_fDecAlv;	// ���l�̌����X�s�[�h
};
#endif