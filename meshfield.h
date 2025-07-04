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
#include<memory>

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

	// �g�̏��(�k���n�A�������a�A�O�����a�A�����A�����A�W���A��������)
	struct Config
	{
		D3DXVECTOR3 epicenter;			// �g�̔����n�_
		float fInRadius;				// �g�̓����a
		float fOutRadius;				// �g�̊O���a
		float fHeight;					// �g�̍���
		float fSpeed;					// ����
		float fcoef;					// �W��
		int nTime;						// �g�̔�������
	};

	CMeshFieldWave();
	~CMeshFieldWave();

	static CMeshFieldWave* Create(Config config);
	void Init(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
private:

	// �g�̏��(�A���C�����g�΍�)
	struct ParamInfo
	{
		float fStartHeight;		// �ŏ��̍���
		float fTime;			// ����
		int nCounter;			// �g�̃J�E���^�[
	};

	Config m_Confing;		// �g�̃p�����[�^
	ParamInfo m_Info;			// �g�̏��
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

	// �Ռ��g�̍\��(�ʒu�A���ˈʒu�A�����A�T�[�N���̐F�A�������I�u�W�F�N�g�A�����蔻��̔��a�A�����A�����A��������)
	struct Config
	{
		D3DXVECTOR3 pos;	         // �ʒu
		D3DXVECTOR3 FirstPos;		 // �ŏ��̈ʒu
		D3DXVECTOR3 dir;			 // ����
		D3DXCOLOR Circlecol;		 // �F
		OBJ ObjType;				 // �N���������Ռ��g��
		float fRadius;	         // ���a
		float fHeight;	         // ����
		float fSpeed;				 // ����
		int nTime;		         // ����
	};
	CMeshFieldImpact();
	~CMeshFieldImpact();

	static CMeshFieldImpact* Create(Config config);
	void Uninit(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
	bool Collision(const D3DXVECTOR3 pos,const float fRadius, const OBJ myObj);
	D3DXVECTOR3 GetPosition(void) const { return m_Config.pos; }
	D3DXVECTOR3 GetFirstPos(void) const { return m_Config.FirstPos; }
	void Reset(D3DXVECTOR3 dir, const OBJ obj, const D3DXVECTOR3 FirstPos,const D3DXCOLOR Circlecol);
private:

	// �Ռ��g�̏��()
	struct Info
	{
		int nCounter;		         // �J�E���^�[
	};
	Config m_Config;			 // �\��
	Info m_Info;				 // ���

	std::unique_ptr<CCollisionSphere> m_pSphere; // �~�̓����蔻��
	CVelocity* m_pMove;	         // �ړ��ʃN���X�ւ̃|�C���^
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

	// �Q�b�^�[
	D3DXVECTOR3 GetNor(void) const { return m_Nor; }
	CMeshFieldImpact* GetImpact(void) { return m_pImpact; }

	// �Z�b�^�[
	void SetWave(CMeshFieldWave::Config config);
	void SetImpact(CMeshFieldImpact::Config config);
private:
	void Load(void);

	std::vector<CMeshFieldWave*> m_pWave;	// �t�B�[���h�̔g�N���X�ւ̃|�C���^
	CMeshFieldImpact* m_pImpact;			// �t�B�[���h�̃C���p�N�g�N���X�ւ̃|�C���^
	D3DXVECTOR3 m_Nor;						// �@��
	float m_fWidth, m_fHeight;				// ����,����
};

#endif