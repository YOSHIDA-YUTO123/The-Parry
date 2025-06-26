//===================================================
//
// �I�u�W�F�N�gX�t�@�C�� [objectX.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _OBJECTX_H_
#define _OBJECTX_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//***************************************************
// �}�N����`
//***************************************************
#define MAX_MAT (256) // �}�e���A���̐�

//***************************************************
// �I�u�W�F�N�gX�t�@�C���̃N���X�̒�`
//***************************************************
class CObjectX : public CObject
{
public:
	CObjectX(int nPriority = 2);
	~CObjectX();

	static CObjectX* Create(const D3DXVECTOR3 pos, const char* pModelName);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(const float Diffuse);
	void DrawShadow(void);

	HRESULT LoadModel(const char* pXFileName);
	//void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	//void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	
	CPosition* GetPosition(void) const { return m_pPos; }
	CRotation* GetRotaition(void) const { return m_pRot; }

private:
	CPosition* m_pPos; // �ʒu�N���X�ւ̃|�C���^
	CRotation* m_pRot; // �����N���X�ւ̃|�C���^
	//D3DXVECTOR3 m_pos;		// �ʒu(�I�t�Z�b�g)
	//D3DXVECTOR3 m_rot;		// ����
	D3DXMATRIX m_mtxWorld;	// ���[���h�}�g���b�N�X
	int m_nModelIdx;		// ���f���̃C���f�b�N�X
	int *m_pTextureIdx;		// �e�N�X�`���̃C���f�b�N�X
};
#endif
