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
#include"transform.h"
#include"Collision.h"

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
// ���b�V���t�B�[���h�̃C���p�N�g�N���X�̒�`
//************************************************
class CMeshFieldImpact
{
public:

	// �A�^�b�J�[�̎��
	typedef enum
	{
		OBJ_NONE = 0,
		OBJ_PLAYER,
		OBJ_ENEMY,
		OBJ_MAX
	}OBJ;

	CMeshFieldImpact();
	~CMeshFieldImpact();

	static CMeshFieldImpact* Create(const D3DXVECTOR3 pos,D3DXVECTOR3 dir, const float fHeight, const float fRadius, const int nTime,const float fSpeed,const OBJ attacker);
	void Uninit(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
	bool Collision(const D3DXVECTOR3 pos,const float fRadius, const OBJ myObj);
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetFirstPos(void) const { return m_FirstPos; }
	void Reset(D3DXVECTOR3 dir, const OBJ obj, const D3DXVECTOR3 FirstPos);
private:
	CCollisionSphere* m_pSphere; // �~�̓����蔻��
	D3DXVECTOR3 m_FirstPos;		 // �g�̍ŏ��̔����ʒu
	D3DXVECTOR3 m_pos;	         // �ʒu
	CVelocity* m_pMove;	         // �ړ��ʃN���X�ւ̃|�C���^
	OBJ m_ObjType;				 // �N���������Ռ��g��
	float m_fRadius;	         // ���a
	float m_fHeight;	         // ����
	float m_fSpeed;				 // ����
	int m_nTime;		         // ����
	int m_nCounter;		         // �J�E���^�[
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
	void SetImpact(const D3DXVECTOR3 pos, D3DXVECTOR3 dir, const float fHeight, const float fRadius, const int nTime, const float fSpeed,CMeshFieldImpact::OBJ attacker);
	CMeshFieldImpact* GetImpact(void) { return m_pImpact; }
private:
	void Load(void);

	CMeshFieldImpact* m_pImpact;			// �t�B�[���h�̃C���p�N�g�N���X�ւ̃|�C���^
	std::vector<CMeshFieldWave*> m_pWave;	// �t�B�[���h�̔g�N���X�ւ̃|�C���^
	D3DXVECTOR3 m_Nor;					// �@��
	float m_fWidth, m_fHeight;			// ����,����
};

#endif