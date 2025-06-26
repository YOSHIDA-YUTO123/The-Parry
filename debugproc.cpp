//===================================================
//
// �f�o�b�O�t�H���g [debugproc.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "debugproc.h"
#include "manager.h"
#include"renderer.h"
#include <stdio.h>
//***************************************************
// �ÓI�����o�ϐ��̐錾
//***************************************************
LPD3DXFONT CDebugProc::m_pFont = NULL;	// �t�H���g�ւ̃|�C���^
char CDebugProc::m_aStr[MAX_WORD] = {};	// ������i�[�p�o�b�t�@
int CDebugProc::m_nIdx = 0;				// �C���f�b�N�X

//===================================================
// �R���X�g���N�^
//===================================================
CDebugProc::CDebugProc()
{
	m_pFont = NULL;
	memset(&m_aStr[0], NULL, sizeof(m_aStr));
}

//===================================================
// �f�X�g���N�^
//===================================================
CDebugProc::~CDebugProc()
{
}

//===================================================
// ����������
//===================================================
HRESULT CDebugProc::Init(void)
{
	CRenderer* pRenderer = NULL;

	pRenderer = CManager::GetRenderer();

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// �f�o�b�O�\���p�̃t�H���g��ݒ�
	D3DXCreateFont(pDevice, 18, 0, 0, 0,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		"Terminal", &m_pFont);

	memset(&m_aStr[0], NULL, sizeof(m_aStr));

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CDebugProc::Uninit(void)
{
	if (m_pFont != NULL)
	{
		m_pFont->Release();
		m_pFont = NULL;
	}
}

//===================================================
// �f�o�b�O�\������
//===================================================
void CDebugProc::Print(const char* fmt ...)
{
	// �ψ���������
	va_list args;

	// �ψ����̏������n�߂�
	va_start(args, fmt);

	// �t�H�[�}�b�g�ς݂̕������astring�ɕۊ�
	m_nIdx += vsnprintf(&m_aStr[m_nIdx], *fmt, fmt, args);

	// �ψ����̏������I��
	va_end(args);
}

//===================================================
// �`�揈��
//===================================================
void CDebugProc::Draw(void)
{
	// �f�o�b�O�\����ON��������
	if (CManager::GetShowDebug() == true)
	{
		RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

		m_pFont->DrawText(NULL, &m_aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255,255,255,255));
	}

	memset(&m_aStr[0], NULL, sizeof(m_aStr));	
}
