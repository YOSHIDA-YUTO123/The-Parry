//===================================================
//
// �I�u�W�F�N�gX�t�@�C�� [objectX.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "objectX.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CObjectX::CObjectX(int nPriority) : CObject(nPriority)
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));

	m_pPos = nullptr;
	m_pRot = nullptr;
	m_pTextureIdx = nullptr;
	m_nModelIdx = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CObjectX::~CObjectX()
{
}

//===================================================
// ��������
//===================================================
CObjectX* CObjectX::Create(const D3DXVECTOR3 pos, const char* pModelName)
{
	CObjectX* pObjectX = nullptr;

	// 3D�I�u�W�F�N�g�̐���
	pObjectX = new CObjectX;

	// �D�揇�ʂ̎擾
	int nPriority = pObjectX->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pObjectX != nullptr)
	{
		// �����̃|�C���^�̉��
		pObjectX->Release();

		// null�ɂ���
		pObjectX = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}


	if (pObjectX == nullptr) return nullptr;

	pObjectX->Init();
	pObjectX->m_pPos->Set(pos);
	pObjectX->LoadModel(pModelName);

	return pObjectX;
}

//===================================================
// ����������
//===================================================
HRESULT CObjectX::Init(void)
{
	// �ʒu�A�����̐���
	m_pPos = new CPosition;
	m_pRot = new CRotation;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CObjectX::Uninit(void)
{
	// �}�e���A���̔j��
	if (m_pTextureIdx != nullptr)
	{
		delete []m_pTextureIdx;
		m_pTextureIdx = nullptr;
	}
	
	// �ʒu�̔j��
	if (m_pPos != nullptr)
	{
		delete m_pPos;
		m_pPos = nullptr;
	}

	// �����̔j��
	if (m_pRot != nullptr)
	{
		delete m_pRot;
		m_pRot = nullptr;
	}

	// �������g�̔j��
	Release();
}

//===================================================
// �X�V����
//===================================================
void CObjectX::Update(void)
{
}

//===================================================
// �`�揈��
//===================================================
void CObjectX::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// ���f���N���X�̎擾
	CModelManager* pModel = CManager::GetModel();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	D3DMATERIAL9 matDef;//���݂̃}�e���A���ۑ��p

	D3DXMATERIAL* pMat;//�}�e���A���f�[�^�ւ̃|�C���^

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// ����
	D3DXVECTOR3 rot = m_pRot->Get();

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu
	D3DXVECTOR3 pos = m_pPos->Get();

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//�ۑ����Ă����}�e���A�������ɖ߂�
		pDevice->SetMaterial(&matDef);

		return;
	}

	//�}�e���A���̃f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// �}�e���A���̑����̎擾
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// ���b�V���̎擾
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		//�}�e���A���̐ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, NULL);
		}
		//���f��(�p�[�c)�̕`��
		pMesh->DrawSubset(nCntMat);
	}

	//�ۑ����Ă����}�e���A�������ɖ߂�
	pDevice->SetMaterial(&matDef);
}

//===================================================
// �`�揈��(�I�[�o�[���[�h)
//===================================================
void CObjectX::Draw(const float Diffuse)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// ���f���N���X�̎擾
	CModelManager* pModel = CManager::GetModel();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxScal, mtxParent;

	D3DMATERIAL9 matDef;//���݂̃}�e���A���ۑ��p

	D3DXMATERIAL* pMat;//�}�e���A���f�[�^�ւ̃|�C���^

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// ����
	D3DXVECTOR3 rot = m_pRot->Get();

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu
	D3DXVECTOR3 pos = m_pPos->Get();

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	if (m_nModelIdx == -1)
	{
		//�ۑ����Ă����}�e���A�������ɖ߂�
		pDevice->SetMaterial(&matDef);

		return;
	}

	//�}�e���A���̃f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)pModel->GetBuffMat(m_nModelIdx)->GetBufferPointer();

	// �}�e���A���̑����̎擾
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// ���b�V���̎擾
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		D3DXMATERIAL Mat = pMat[nCntMat];

		pMat[nCntMat].MatD3D.Diffuse.a = Diffuse;

		//�}�e���A���̐ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (m_pTextureIdx[nCntMat] != -1)
		{
			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, pTexture->GetAdress(m_pTextureIdx[nCntMat]));
		}
		else
		{
			//�e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, NULL);
		}
		//���f��(�p�[�c)�̕`��
		pMesh->DrawSubset(nCntMat);
	}

	//�ۑ����Ă����}�e���A�������ɖ߂�
	pDevice->SetMaterial(&matDef);
}

//===================================================
// �e�̕`�揈��
//===================================================
void CObjectX::DrawShadow(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���C�g�̕���
	D3DXVECTOR4 lightDir = D3DXVECTOR4(0.707f, -0.707f, 0.707f, 0.0f);

	// �e�𗎂Ƃ��n��
	D3DXPLANE plane = D3DXPLANE(0, -1.0f, 0, 0);

	// �e�p�̃}�g���b�N�X���쐬
	D3DXMATRIX mtxShadow, mtxShadowWorld;

	D3DMATERIAL9 matDef;//���݂̃}�e���A���ۑ��p

	// �e�p�̃}�g���b�N�X�̍쐬
	D3DXMatrixShadow(&mtxShadow, &lightDir, &plane);

	// �e�̃}�g���b�N�X�Ǝ����̃}�g���b�N�X���|�����킹��
	D3DXMatrixMultiply(&mtxShadowWorld, &m_mtxWorld, &mtxShadow);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadowWorld);

	//���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	// �������̍��}�e���A�����Z�b�g
	D3DMATERIAL9 matShadow = {};

	// �e�̐F�̐ݒ�
	matShadow.Diffuse.r = 0.0f;
	matShadow.Diffuse.g = 0.0f;
	matShadow.Diffuse.b = 0.0f;
	matShadow.Diffuse.a = 1.0f;

	// ���f���̎擾
	CModelManager* pModel = CManager::GetModel();

	// ���b�V���̎擾
	LPD3DXMESH pMesh = pModel->GetMesh(m_nModelIdx);

	// ���b�V����nullptr��������֐��𔲂���
	if (pMesh == nullptr) return;
	
	// �}�e���A���̐��̎擾
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);

	// �e�̕`��
	for (int nCntMat = 0; nCntMat < (int)dwNumMat; nCntMat++)
	{
		// �e�̃}�e���A���̐ݒ�
		pDevice->SetMaterial(&matShadow);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);

		// �e�̕`��
		pMesh->DrawSubset(nCntMat);
	}
	
	// �}�e���A�������Ƃɖ߂�
	pDevice->SetMaterial(&matDef);
}

//===================================================
// ���f���̃��[�h����
//===================================================
HRESULT CObjectX::LoadModel(const char* pXFileName)
{
	// ���f���N���X�̎擾
	CModelManager* pModel = CManager::GetModel();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �C���f�b�N�X�̓o�^
	m_nModelIdx = pModel->Register(pXFileName);

	if (m_nModelIdx == -1)
	{
		return E_FAIL;
	}

	// �}�e���A���̎擾
	DWORD dwNumMat = pModel->GetNumMat(m_nModelIdx);
	LPD3DXBUFFER pBuffMat = pModel->GetBuffMat(m_nModelIdx);

	D3DXMATERIAL* pMat;//�}�e���A���ւ̃|�C���^

	//�}�e���A���̃f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	// �}�e���A���̐����������̊m��
	m_pTextureIdx = new int[dwNumMat];

	for (int nCnt = 0; nCnt < (int)dwNumMat; nCnt++)
	{
		// �t�@�C�������g�p���ăe�N�X�`����ǂݍ���
		m_pTextureIdx[nCnt] = pTexture->RegisterX(pMat[nCnt].pTextureFilename);
	}

	return S_OK;
}

