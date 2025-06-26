//===========================================
//
// �e�N�X�`���}�l�[�W���[ [textureManager.h]
// Author:YUTO YOSHIDA
//
//===========================================

//*******************************************
// �C���N���[�h�t�@�C��
//*******************************************
#include "textureManager.h"
#include "manager.h"
#include "renderer.h"
#include <cassert>
#include<stdio.h>

//*******************************************
// �ÓI�����o�ϐ��錾
//*******************************************
int CTextureManager::m_nNumAll = 0;	// �e�N�X�`���̑���

//===========================================
// �R���X�g���N�^
//===========================================
CTextureManager::CTextureManager()
{
	// �l�̏�����
	memset(m_apTextureInfo, NULL, sizeof(m_apTextureInfo));
}

//===========================================
// �f�X�g���N�^
//===========================================
CTextureManager::~CTextureManager()
{
}

//===========================================
// �e�N�X�`���̔ԍ��̎擾
//===========================================
int CTextureManager::Register(const char* pFilename)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �C���f�b�N�X
	int nIdx = 0;

	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// �ǂ��炩��null��������
		if (m_apTextureInfo[nCnt].filepath == NULL || pFilename == NULL)
		{
			continue;
		}

		// �����񂪈�v���Ă�����
		if (strcmp(m_apTextureInfo[nCnt].filepath, pFilename) == 0)
		{
			nIdx = nCnt;
			break;
		}

		// �w�肳�ꂽ�e�N�X�`��������������
		if (m_nNumAll < nCnt)
		{
			// �e�N�X�`���̓ǂݍ���
			D3DXCreateTextureFromFile(pDevice,
				pFilename,
				&m_apTextureInfo[nCnt].pTexture);

			m_nNumAll++;

			strncpy(m_apTextureInfo[nCnt].filepath, pFilename, sizeof(m_apTextureInfo[nCnt].filepath));

			nIdx = nCnt;
			break;
		}
	}

	return nIdx;
}

//===========================================
// �C���f�b�N�X�̓o�^
//===========================================
int CTextureManager::RegisterX(const char* pFilename)
{
	int nIdx = -1;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (pFilename == NULL)
	{
		return -1;
	}

	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// �ǂ��炩��null��������
		if (m_apTextureInfo[nCnt].filepath == NULL || pFilename == NULL)
		{
			continue;
		}

		// �����񂪈�v���Ă�����
		if (strcmp(m_apTextureInfo[nCnt].filepath, pFilename) == 0)
		{
			nIdx = nCnt;

			break;
		}

		// �w�肳�ꂽ�e�N�X�`��������������
		if (m_nNumAll < nCnt)
		{
			//���̃t�@�C�������g�p���ăe�N�X�`����ǂݍ���
				//�e�N�X�`���̓ǂݍ���
			D3DXCreateTextureFromFile(pDevice,
				pFilename,
				&m_apTextureInfo[nCnt].pTexture);

			strncpy(m_apTextureInfo[nCnt].filepath, pFilename, sizeof(m_apTextureInfo[nCnt].filepath));

			m_nNumAll++;

			nIdx = nCnt;

			break;
		}
	}

	return nIdx;
}

//===========================================
// �e�N�X�`���̎擾����
//===========================================
LPDIRECT3DTEXTURE9 CTextureManager::GetAdress(int nIdx)
{
	// �͈͊O��������
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{
		return nullptr;
	}

	return m_apTextureInfo[nIdx].pTexture;
}

//===========================================
// ���ׂẴe�N�X�`���̓ǂݍ��ݏ���
//===========================================
HRESULT CTextureManager::Load(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �t�@�C�����J��
	FILE* pFile = fopen("data/texture.txt", "r");

	// null����Ȃ�������
	if (pFile != NULL)
	{
		// ������i�[�p
		char aString[MAX_WORD] = {};
		char aSkip[MAX_WORD] = {};

		while (1)
		{
			// 1�������ǂݎ��
			int nData = fscanf(pFile, "%s", &aString);

			// �����񂪈�v���Ă�����
			if (strcmp(aString, "TEXTURE_NAME") == 0)
			{
				// [=]���΂�
				nData = fscanf(pFile, "%s", &aSkip);
				nData = fscanf(pFile, "%s", &aString);

				// �t�@�C���̖��O����
				const char* FILE_NAME = aString;

				// ������̃R�s�[
				strncpy(m_apTextureInfo[m_nNumAll].filepath, FILE_NAME, sizeof(m_apTextureInfo[m_nNumAll].filepath));

				// �e�N�X�`���̓ǂݍ���
				if (FAILED(D3DXCreateTextureFromFile(pDevice,
					FILE_NAME,
					&m_apTextureInfo[m_nNumAll].pTexture)))
				{
					MessageBox(NULL, FILE_NAME, "�e�N�X�`�����ǂݍ��߂܂���", MB_OK | MB_ICONWARNING);
				}

				// �����𑝂₷
				m_nNumAll++;
			}

			// �t�@�C���̍Ō�܂ōs������
			if (nData == EOF)
			{
				break;
			}
		}
	}
	else
	{
		MessageBox(NULL, "�t�@�C�����J���܂���", "textureManager.cpp", MB_OK);
		return S_OK;
	}

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}

//===========================================
// ���ׂẴe�N�X�`���̉������
//===========================================
void CTextureManager::UnLoad(void)
{
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// �e�N�X�`���̔j��
		if (m_apTextureInfo[nCnt].pTexture != nullptr)
		{
			m_apTextureInfo[nCnt].pTexture->Release();
			m_apTextureInfo[nCnt].pTexture = nullptr;
		}
	}
}
