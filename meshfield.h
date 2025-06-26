//================================================
//
// ���b�V���t�B�[���h [meshfield.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"mesh.h"
#include<vector>

//************************************************
// ���b�V���t�B�[���h�N���X�̒�`
//************************************************
class CMeshField : public CMesh
{
public:
	CMeshField();
	~CMeshField();

	static CMeshField* Create(const D3DXVECTOR3 pos,const int nSegX,const int nSegZ,const D3DXVECTOR2 Size, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMeshField(const int nSegX, const int nSegZ, const  D3DXVECTOR3 pos, const D3DXVECTOR2 Size);
	bool Collision(const D3DXVECTOR3 pos, float* pOutHeight);
	void SetWave(const D3DXVECTOR3 epicenter, const int nTime, const float fSpeed, const float fInRadius, const float fOutRadius, const float fWaveHeight, const float fcoef);
	void UpdateNor(void);
	D3DXVECTOR3 GetNor(void) const { return m_Nor; }
private:
	void UpdateWave(const int nNumVtx);

	// �g�̏��
	struct Wave
	{
		D3DXVECTOR3 epicenter;
		float fInRadius, fOutRadius;	// �g�̔��a
		float fHeight;					// �g�̍���
		float fStartHeight;				// �ŏ��̍���
		float fTime;					// ����
		float fSpeed;					// ����
		float fcoef;					// �W��
		int nCounter;					// �g�̃J�E���^�[
		int nTime;						// �g�̔�������
		bool bUse;						// �g�p���Ă��邩�ǂ���
	};

	D3DXVECTOR3 m_Nor;					// �@��
	Wave m_Wave;						// �E�F�[�u�̏��
	float m_fWidth, m_fHeight;			// ����,����
};
#endif