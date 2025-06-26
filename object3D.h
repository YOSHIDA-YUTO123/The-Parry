//===================================================
//
// 3D�I�u�W�F�N�g�̕`�� [object3D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//***************************************************
// �I�u�W�F�N�g3D�N���X�̒�`
//***************************************************
class CObject3D : public CObject
{
public:
	CObject3D();
	~CObject3D();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDraw(void);

	CPosition* GetPosition(void);
	CRotation* GetRotaition(void);
	CSize3D* GetSize(void);

	//void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	//void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	//void SetSize(const D3DXVECTOR3 size) { GetVector = size; }
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void SetTextureID(const char* pTextureName);
	void UpdatePosition(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size);
	void UpdateCol(const D3DXCOLOR col);

	static CObject3D* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot,const D3DXVECTOR3 size,const char *pTextureName);
private:
	CPosition *m_pPos;	// �ʒu
	CRotation *m_pRot;	// ����
	CSize3D *m_pSize;		// �傫��
	//D3DXVECTOR3 m_pos;						// �ʒu
	//D3DXVECTOR3 m_rot;						// ����
	//D3DXVECTOR3 GetVector;						// �傫��
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// ���_�ւ̃|�C���^
	int m_nTextureIdx;						// �e�N�X�`���̃C���f�b�N�X
};
#endif