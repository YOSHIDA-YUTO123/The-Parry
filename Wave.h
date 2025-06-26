//================================================
//
// ���b�V���E�F�[�u [wave.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _WAVE_H_
#define _WAVE_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"mesh.h"

//************************************************
// ���b�V���E�F�[�u�N���X�̒�`
//************************************************
class CMeshWave : public CMesh
{
public:
	CMeshWave();
	~CMeshWave();

	static CMeshWave* Create(const D3DXVECTOR3 pos, const float fRadius, const float fHeight, const float speed, const int nLife, const int nSegX = 32, const D3DXCOLOR col = WHITE, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetWave(const int nSegX, const float fRadius, const float fHeight); // ���_�̐ݒ�
private:
	D3DXCOLOR m_col;	// �F
	int m_nLife;		// ����
	float m_fDecAlv;	// ���l�̌����X�s�[�h
	float m_fSpeed;		// ����
	float m_fHeight;	// ����
	float m_fRadius;	// ���a
};

#endif