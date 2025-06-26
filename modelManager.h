//==============================================
//
// ���f���}�l�[�W���[ [modelManager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// ���d�C���N���[�h�h�~
//**********************************************
#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

//**********************************************
// �C���N���[�h�t�@�C��
//**********************************************
#include"main.h"
#include"object.h"

//**********************************************
// �}�N����`
//**********************************************
#define MAX_MODEL (256) // ���f���̍ő吔

//**********************************************
// ���f���̃}�l�[�W���[�N���X�̒�`
//**********************************************
class CModelManager
{
public:
	CModelManager();
	~CModelManager();

	int Register(const char* pFilename);
	LPD3DXMESH GetMesh(int nIdx);
	LPD3DXBUFFER GetBuffMat(int nIdx);
	DWORD GetNumMat(int nIdx);
	D3DXVECTOR3 GetSize(int nIdx);
	HRESULT Load(void);
	void UnLoad(void);
private:

	// ���f���̏��̒�`
	struct ModelInfo
	{
		D3DXVECTOR3 vtxMin, vtxMax;		 // �ő�A�ŏ��̒��_
		LPD3DXMESH pMesh;				 // ���b�V��(���_���)�ւ̃|�C���^
		LPD3DXBUFFER pBuffMat;			 // �}�e���A���ւ̃|�C���^
		DWORD dwNumMat;					 // �}�e���A���̐�
		char filepath[MAX_WORD];		 // �t�@�C���p�X
	};

	ModelInfo m_apModelInfo[MAX_MODEL];	 // ���f���̏��
	static int m_nNumAll;				 // �e�N�X�`���̔ԍ�
};

#endif
