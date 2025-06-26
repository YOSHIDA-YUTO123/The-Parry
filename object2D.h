//===================================================
//
// �I�u�W�F�N�g2D�̕`�揈�� [object2D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _OBJECT2D_H_
#define _OBJECT2D_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//***************************************************
// �}�N����`
//***************************************************

//***************************************************
// �����_���[�N���X�̒�`
//***************************************************
class CObject2D : public CObject
{
public:

	// �I�u�W�F�N�g�̎��
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_BACKGROUND,
		TYPE_PLAYER,
		TYPE_MAX
	}TYPE;

	CObject2D(int nPriority = 3);
	~CObject2D();

	static CObject2D* Create(const float fWidth, const float fHeight, const D3DXVECTOR3 pos);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetSize(const float fWidth, const float fHeight);
	void SetSize(const float leftWidth,const float rightWdth,const float topHeight,const float buttom);
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void UpdateVertex(void);
	void SetUvPos(const D3DXVECTOR2 OffPosTex,const D3DXVECTOR2 PosTex);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName = NULL);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// ���_�ւ̃|�C���^
	CPosition *m_pPos;	// �ʒu
	CRotation *m_pRot;	// ����
	CSize2D *m_pSize;	// �傫��
	float m_fAngle;							// �p�x
	float m_Length;							// ����
	int m_nTextureIdx;						// �e�N�X�`���̃C���f�b�N�X
};

#endif