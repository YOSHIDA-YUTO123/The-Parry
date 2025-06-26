//================================================
//
// �����𑀍삷�鏈�� [number.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// ���d�C���N���[�h�h�~
//*************************************************
#ifndef _NUMBER_H_
#define _NUMBER_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//************************************************
// �i���o�[�N���X�̒�`
//************************************************
class CNumber
{
public:

	CNumber();
	~CNumber();

	HRESULT Init(const int nPosX, const int nPosY, const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetSize(const float fWIdth, const float fHeight) { m_pSize->Set(fWIdth, fHeight); }
	void SetPos(const D3DXVECTOR3 pos);
	void SetUV(const int nNum);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pFileName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// ���_�ւ̃|�C���^
	CPosition* m_pPos;						// �ʒu�̃N���X�ւ̃|�C���^
	CSize2D *m_pSize;						// �傫��
	int m_nTextureIdx;						// �e�N�X�`���̃C���f�b�N�X
};
#endif