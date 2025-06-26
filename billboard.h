//===================================================
//
// �r���{�[�h�̕`�� [billboard.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//***************************************************
// �I�u�W�F�N�g3D�N���X�̒�`
//***************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 4);
	virtual ~CObjectBillboard();

	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	CPosition* GetPosition(void) const { return m_pPos; }
	CSize3D* GetSize(void) const { return m_pSize; }
	
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName);
	void UpdateVertexPos(const D3DXVECTOR3 pos);
	void SetTextureVtx(const D3DXVECTOR2 tex, const D3DXVECTOR2 offtex);

	static CObjectBillboard* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const char* pTextureName);
private:
	CPosition* m_pPos;		// �ʒu�̃N���X�̃|�C���^
	CSize3D* m_pSize;		// �傫���̃N���X�̃|�C���^
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// ���_�ւ̃|�C���^
	int m_nTextureIdx;						// �e�N�X�`���̃C���f�b�N�X
};

//***************************************************
// �r���{�[�h(�A�j���[�V����)�N���X�̒�`
//***************************************************
class CBillboardAnimation : public CObjectBillboard
{
public:
	CBillboardAnimation(int nPriority = 4);
	~CBillboardAnimation();

	void SetAnim(const int nAnimSpeed,const int U,const int V);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	float m_fPosTexU;	// �e�N�X�`�����WUV
	float m_fPosTexH;	// �e�N�X�`�����WHV
	int m_nAnimSpeed;	// �A�j���[�V�����̑���
	int m_nPatternAnim;	// �A�j���[�V�����̃p�^�[��
	int m_nCounterAnim;	// �A�j���[�V�����̃J�E���^�[
	int m_nU,m_nV;		// �e�N�X�`����UV�̕�����
};
#endif