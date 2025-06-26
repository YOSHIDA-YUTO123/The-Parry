//===================================================
//
// ���f�� [model.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _MODEL_H_
#define _MODEL_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"

//***************************************************
// ���f���N���X�̒�`
//***************************************************
class CModel
{
public:
	CModel();
	~CModel();
	static CModel* Create(const char* pModelName);
	static int LoadCharctorSet(const char* pLoadFileName, CModel** pModel,const int nMaxSize);

	HRESULT Init(const char* pModelName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawShadow(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }
	D3DXVECTOR3 GetOffPos(void) const { return m_offpos; }
	D3DXVECTOR3 GetOffRot(void) const { return m_offrot; }

	D3DXMATRIX GetModelMatrix(void) const;
	D3DXVECTOR3 GetSize(void) const { return GetVector; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetOffPos(const D3DXVECTOR3 pos) { m_offpos = pos; }
	void SetOffRot(const D3DXVECTOR3 rot) { m_offrot = rot; }

	void SetParent(CModel* pModel) { m_pParent = pModel; }
	D3DXMATRIX GetMatrixWorld(void) const { return m_mtxWorld; }
private:
	D3DXVECTOR3 GetVector;				// �傫��
	D3DXVECTOR3 m_offpos;			// ��̈ʒu
	D3DXVECTOR3 m_offrot;			// ��̌���
	D3DXVECTOR3 m_pos;				// �ʒu
	D3DXVECTOR3 m_rot;				// ����
	D3DXMATRIX m_mtxWorld;			// ���[���h�}�g���b�N�X
	CModel* m_pParent;				// �e���f���ւ̃|�C���^
	int m_nModelIdx;				// ���f���̃C���f�b�N�X
	int* m_pTextureIdx;				// �e�N�X�`���̃C���f�b�N�X
};
#endif