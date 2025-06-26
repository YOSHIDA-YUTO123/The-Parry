//================================================
//
// �����𑀍삷�鏈�� [word.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// ���d�C���N���[�h�h�~
//************************************************
#ifndef _WORD_H_
#define _WORD_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"main.h"

//************************************************
// ���[�h�N���X�̒�`
//************************************************
class CWord
{
public:

	// �����̎��
	typedef enum
	{
		WORD_A = 0,
		WORD_B,
		WORD_C,
		WORD_D,
		WORD_E,
		WORD_F,
		WORD_G,
		WORD_H,
		WORD_I,
		WORD_J,
		WORD_K,
		WORD_L,
		WORD_M,
		WORD_N,
		WORD_O,
		WORD_P,
		WORD_Q,
		WORD_R,
		WORD_S,
		WORD_T,
		WORD_U,
		WORD_V,
		WORD_W,
		WORD_X,
		WORD_Y,
		WORD_Z,
		WORD_MAX
	}WORD;

	CWord();
	~CWord();

	HRESULT Init(const int nPosX, const int nPosY, const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetSize(const float fWIdth, const float fHeight);
	void SetPos(const D3DXVECTOR3 pos);
	void SetUV(const int nNum);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pFileName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// ���_�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	float m_fWidth, m_fHeight;				// ���A����
	int m_nTextureIdx;						// �e�N�X�`���̃C���f�b�N�X
};
#endif