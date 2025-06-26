//================================================
//
// �����e�X�g [testword.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// ���d�C���N���[�h�h�~
//************************************************
#ifndef _TESTWORD_H_
#define _TESTWORD_H_

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include"main.h"
#include"object.h"
#include"word.h"

//************************************************
// �����N���X�̒�`
//************************************************
class CTestWord : public CObject
{
public:
	CTestWord();
	~CTestWord();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CTestWord* Create(const int nWord, const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
private:
	CWord* m_pWord;		// �����̃N���X�ւ̃|�C���^
	D3DXVECTOR3 m_pos;	// �ʒu
	float m_fWidth;		// ����
	float m_fHeight;	// ����
	int m_nWord;		// ����
};
#endif