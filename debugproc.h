//===================================================
//
// �f�o�b�O�t�H���g [debugproc.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"

//***************************************************
// �}�N����`
//***************************************************
#define MAX_WORD (1024) // �ő啶����

//***************************************************
// �}�l�[�W���[�N���X�̒�`
//***************************************************
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	HRESULT Init(void);
	void Uninit(void);
	static void Print(const char* fmt...);
	static void Draw(void);
	static void Reset(void) { m_nIdx = 0; }
private:
	static int m_nIdx;				// �C���f�b�N�X
	static LPD3DXFONT m_pFont;		// �t�H���g�ւ̃|�C���^
	static char m_aStr[MAX_WORD];	// ��������i�[����o�b�t�@
};
#endif