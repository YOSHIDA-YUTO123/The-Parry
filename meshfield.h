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
// �O���錾
//************************************************
class CMeshField;

//************************************************
// ���b�V���̃t�B�[���h�̃E�F�[�u�N���X�̒�`
//************************************************
class CMeshFieldWave
{
public:
	CMeshFieldWave();
	~CMeshFieldWave();

	static CMeshFieldWave* Create(const D3DXVECTOR3 epicenter, const float InR, const float OutR, const float fHeight, const float fSpeed, const float fcoef, const int nTime);
	void Init(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
private:
	D3DXVECTOR3 m_epicenter;			// �g�̔����n�_
	float m_fInRadius, m_fOutRadius;	// �g�̔��a
	float m_fHeight;					// �g�̍���
	float m_fStartHeight;				// �ŏ��̍���
	float m_fTime;						// ����
	float m_fSpeed;						// ����
	float m_fcoef;						// �W��
	int m_nCounter;						// �g�̃J�E���^�[
	int m_nTime;						// �g�̔�������
};

//************************************************
// ���b�V���t�B�[���h�N���X�̒�`
//************************************************
class CMeshField : public CMesh
{
public:
	CMeshField(int nPriority = 2);
	~CMeshField();

	static CMeshField* Create(const D3DXVECTOR3 pos,const int nSegX,const int nSegZ,const D3DXVECTOR2 Size, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMeshField(const int nSegX, const int nSegZ, const  D3DXVECTOR3 pos, const D3DXVECTOR2 Size);
	bool Collision(const D3DXVECTOR3 pos, float* pOutHeight);
	void UpdateNor(void); // �@���̍Đݒ�
	D3DXVECTOR3 GetNor(void) const { return m_Nor; }
	void SetWave(const D3DXVECTOR3 epicenter, const float InR, const float OutR, const float fHeight, const float fSpeed, const float fcoef, const int nTime);
private:
	void Load(void);

	std::vector<CMeshFieldWave*> m_pWave;	// �t�B�[���h�̔g�N���X�ւ̃|�C���^
	D3DXVECTOR3 m_Nor;					// �@��
	float m_fWidth, m_fHeight;			// ����,����
};

#endif