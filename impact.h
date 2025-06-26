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
	CMeshCircle();
	~CMeshCircle();

	static CMeshCircle* Create(const D3DXVECTOR3 pos, const float InRadius, const float OutRadius,const float speed,const int nLife, const int nSegX = 32, const D3DXCOLOR col = WHITE,const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetCircle(const int nSegX, const float InRadius,const float OutRadius); // ���_�̐ݒ�
private:
	D3DXCOLOR m_Incol,m_Outcol;	// �F
	int m_nLife;				// ����
	float m_fDecAlv;			// ���l�̌����X�s�[�h
	float m_fSpeed;				// ����
	float m_fInRadius;			// �����̔��a
	float m_fOutRadius;			// �O���̔��a
};
#endif