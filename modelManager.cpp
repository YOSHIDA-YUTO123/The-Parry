//==============================================
//
// ���f���}�l�[�W���[ [modelManager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// �C���N���[�h�t�@�C��
//**********************************************
#include "modelManager.h"
#include "manager.h"
#include"renderer.h"
#include<stdio.h>

//**********************************************
// �ÓI�����o�ϐ��錾
//**********************************************
int CModelManager::m_nNumAll = 0;	// ���f���̑���

//==============================================
// �R���X�g���N�^
//==============================================
CModelManager::CModelManager()
{
	memset(m_apModelInfo, NULL, sizeof(m_apModelInfo));
}

//==============================================
// �f�X�g���N�^
//==============================================
CModelManager::~CModelManager()
{
}

//==============================================
// �C���f�b�N�X�̓o�^
//==============================================
int CModelManager::Register(const char* pFilename)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	int nIdx = -1;

	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		if (pFilename == NULL || m_apModelInfo[nCnt].filepath == NULL)
		{
			continue;
		}

		if (strcmp(m_apModelInfo[nCnt].filepath, pFilename) == 0)
		{
			nIdx = nCnt;
			break;
		}

		if (m_nNumAll < nCnt && m_nNumAll <= MAX_MODEL - 1)
		{
			////X�t�@�C���̓ǂݍ���
			if (FAILED(D3DXLoadMeshFromX(pFilename,
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&m_apModelInfo[nCnt].pBuffMat,
				NULL,
				&m_apModelInfo[nCnt].dwNumMat,
				&m_apModelInfo[nCnt].pMesh)))
			{
				// ���b�Z�[�W�{�b�N�X�̕\��
				MessageBox(NULL, pFilename, "���f�����ǂݍ��߂܂���ł���", MB_OK | MB_ICONWARNING);

				return -1;
			}

			// ��������R�s�[����
			strncpy(m_apModelInfo[nCnt].filepath, pFilename, sizeof(m_apModelInfo[nCnt].filepath));

			nIdx = nCnt;

			m_nNumAll++;

			break;
		}
	}
	return nIdx;
}

//==============================================
// ���b�V���̎擾
//==============================================
LPD3DXMESH CModelManager::GetMesh(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return nullptr;
	}

	return m_apModelInfo[nIdx].pMesh;
}

//==============================================
// �o�b�t�@�̎擾
//==============================================
LPD3DXBUFFER CModelManager::GetBuffMat(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return nullptr;
	}

	return m_apModelInfo[nIdx].pBuffMat;
}

//==============================================
// �}�e���A���̂̎擾
//==============================================
DWORD CModelManager::GetNumMat(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return 0;
	}

	return m_apModelInfo[nIdx].dwNumMat;
}

//==============================================
// �傫���̎擾
//==============================================
D3DXVECTOR3 CModelManager::GetSize(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{
		return VEC3_NULL;
	}

	// ���_���W�̎擾
	int nNumVtx;	// ���_��
	DWORD sizeFVF;  // ���_�t�H�[�}�b�g�̃T�C�Y
	BYTE* pVtxBuff; // ���_�o�b�t�@�ւ̃|�C���^

	// ���_���̎擾
	nNumVtx = m_apModelInfo[nIdx].pMesh->GetNumVertices();

	// ���_�t�H�[�}�b�g�̃T�C�Y�擾
	sizeFVF = D3DXGetFVFVertexSize(m_apModelInfo[nIdx].pMesh->GetFVF());

	// ���_�o�b�t�@�̃��b�N
	m_apModelInfo[nIdx].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		// ���_���W�̑��
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

		// ���_���W���r���ău���b�N�̍ŏ��l,�ő�l���擾
		if (vtx.x < m_apModelInfo[nIdx].vtxMin.x)
		{
			m_apModelInfo[nIdx].vtxMin.x = vtx.x;
		}
		else if (vtx.y < m_apModelInfo[nIdx].vtxMin.y)
		{
			m_apModelInfo[nIdx].vtxMin.y = vtx.y;
		}
		else if (vtx.z < m_apModelInfo[nIdx].vtxMin.z)
		{
			m_apModelInfo[nIdx].vtxMin.z = vtx.z;
		}
		else if (vtx.x > m_apModelInfo[nIdx].vtxMax.x)
		{
			m_apModelInfo[nIdx].vtxMax.x = vtx.x;
		}
		else if (vtx.y > m_apModelInfo[nIdx].vtxMax.y)
		{
			m_apModelInfo[nIdx].vtxMax.y = vtx.y;
		}
		else if (vtx.z > m_apModelInfo[nIdx].vtxMax.z)
		{
			m_apModelInfo[nIdx].vtxMax.z = vtx.z;
		}

		// ���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
		pVtxBuff += sizeFVF;
	}

	// �傫��
	D3DXVECTOR3 Size = VEC3_NULL;

	// �T�C�Y�ɑ��
	Size.x = m_apModelInfo[nIdx].vtxMax.x - m_apModelInfo[nIdx].vtxMin.x;
	Size.y = m_apModelInfo[nIdx].vtxMax.y - m_apModelInfo[nIdx].vtxMin.y;
	Size.z = m_apModelInfo[nIdx].vtxMax.z - m_apModelInfo[nIdx].vtxMin.z;

	// ���_�o�b�t�@�̃A�����b�N
	m_apModelInfo[nIdx].pMesh->UnlockVertexBuffer();

	return Size;
}

//==============================================
// ���ׂĂ�xFile�̃��[�h����
//==============================================
HRESULT CModelManager::Load(void)
{
	// �t�@�C�����J��
	FILE* pFile = fopen("data/Modellist.txt", "r");

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �t�@�C�����J������
	if (pFile != NULL)
	{
		// ������i�[�p
		char aString[MAX_WORD] = {};
		char aSkip[MAX_WORD] = {};

		while (1)
		{
			// �t�@�C���̕�����ǂݎ��
			int nData = fscanf(pFile, "%s", &aString);

			// �����񂪈�v������
			if (strcmp(aString, "FILENAME") == 0)
			{
				// [=]��ǂݎ��
				nData = fscanf(pFile, "%s", &aSkip);
				nData = fscanf(pFile, "%s", &aString);

				const char* MODEL_NAME = aString;

				// ��������R�s�[����
				strncpy(m_apModelInfo[m_nNumAll].filepath, MODEL_NAME,sizeof(m_apModelInfo[m_nNumAll].filepath));

				////X�t�@�C���̓ǂݍ���
				if (FAILED(D3DXLoadMeshFromX(MODEL_NAME,
					D3DXMESH_SYSTEMMEM,
					pDevice,
					NULL,
					&m_apModelInfo[m_nNumAll].pBuffMat,
					NULL,
					&m_apModelInfo[m_nNumAll].dwNumMat,
					&m_apModelInfo[m_nNumAll].pMesh)))
				{
					// ���b�Z�[�W�{�b�N�X�̕\��
					MessageBox(NULL, MODEL_NAME, "���f�����ǂݍ��߂܂���ł���", MB_OK | MB_ICONWARNING);

					return E_FAIL;
				}

				m_nNumAll++;
			}

			if (nData == EOF)
			{
				break;
			}
		}
	}
	else
	{
		// ���b�Z�[�W�{�b�N�X
		MessageBox(NULL, "�t�@�C�����J���܂���", "modelManager.txt", MB_OK | MB_ICONWARNING);
		return E_FAIL;
	}

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}

//==============================================
// ���ׂĂ�xFile�̔j������
//==============================================
void CModelManager::UnLoad(void)
{
	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		// ���b�V���̔j��
		if (m_apModelInfo[nCnt].pMesh != nullptr)
		{
			m_apModelInfo[nCnt].pMesh->Release();
			m_apModelInfo[nCnt].pMesh = nullptr;
		}

		// �}�e���A���̔j��
		if (m_apModelInfo[nCnt].pBuffMat != nullptr)
		{
			m_apModelInfo[nCnt].pBuffMat->Release();
			m_apModelInfo[nCnt].pBuffMat = nullptr;
		}
	}
}
