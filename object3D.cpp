//===================================================
//
// 3D�I�u�W�F�N�g�̕`�� [object3D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "object3D.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority)
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));
	m_pPos = nullptr;
	m_pRot = nullptr;
	m_pSize = nullptr;
	m_pVtxBuffer = NULL;
	m_nTextureIdx = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CObject3D::~CObject3D()
{
}

//===================================================
// ����������
//===================================================
HRESULT CObject3D::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �ʒu�A�����A�傫���̐ݒ�
	m_pPos = new CPosition;
	m_pRot = new CRotation;
	m_pSize = new CSize3D;

	//���_�o�b�t�@�̐����E���_���̐ݒ�
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CObject3D::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
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

	// �傫���̔j��
	if (m_pSize != nullptr)
	{
		delete m_pSize;
		m_pSize = nullptr;
	}

	// �������g�̔j��
	Release();
}

//===================================================
// �X�V����
//===================================================
void CObject3D::Update(void)
{
}

//===================================================
// �`�揈��
//===================================================
void CObject3D::Draw(void)
{
	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	//	���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// ����
	D3DXVECTOR3 rot = m_pRot->Get();

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu
	D3DXVECTOR3 pos = m_pPos->Get();

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

#if 0
	LPDIRECT3DTEXTURE9 pTextureMT = pRenderer->GetTextureMT();

	// �e�N�X�`���ݒ�
	pDevice->SetTexture(0, pTextureMT);
#else
	// �e�N�X�`���ݒ�
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));
#endif

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===================================================
// �`��ɕK�v�ȏ�񂾂��̕`�揈��
//===================================================
void CObject3D::SetDraw(void)
{
	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���ݒ�
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

}

//===================================================
// �ʒu�̎擾
//===================================================
CPosition* CObject3D::GetPosition(void)
{
	if (m_pPos == nullptr) return nullptr;

	return m_pPos;
}

//===================================================
// �����̎擾
//===================================================
CRotation* CObject3D::GetRotaition(void)
{
	if (m_pRot == nullptr) return nullptr;

	return m_pRot;
}

//===================================================
// �傫���̎擾
//===================================================
CSize3D* CObject3D::GetSize(void)
{
	if (m_pSize == nullptr) return nullptr;

	return m_pSize;
}

//===================================================
// ���_�̃I�t�Z�b�g�̐ݒ菈��
//===================================================
void CObject3D::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// �傫���̎擾
	D3DXVECTOR3 Size = m_pSize->Get();

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// �@���x�N�g���̐ݒ�
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / nPosX, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / nPosY);
	pVtx[3].tex = D3DXVECTOR2(1.0f / nPosX, 1.0f / nPosY);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �e�N�X�`����ID�̐ݒ�
//===================================================
void CObject3D::SetTextureID(const char* pTextureName)
{
	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �e�N�X�`���̖��O�̐ݒ�
	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// �ʒu�̍X�V����
//===================================================
void CObject3D::UpdatePosition(const D3DXVECTOR3 pos,const D3DXVECTOR3 Size)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	m_pPos->Set(pos);
	m_pSize->Set(Size);

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �F�̍X�V����
//===================================================
void CObject3D::UpdateCol(const D3DXCOLOR col)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;
	
	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// ��������
//===================================================
CObject3D* CObject3D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 size, const char* pTextureName)
{
	CObject3D* pObject3D = nullptr;

	// 3D�I�u�W�F�N�g�̐���
	pObject3D = new CObject3D;

	// �D�揇�ʂ̎擾
	int nPriority = pObject3D->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pObject3D != nullptr)
	{
		// �������g�̔j��
		pObject3D->Release();

		// null�ɂ��Ă���
		pObject3D = nullptr;

		return nullptr;
	}

	if (pObject3D == nullptr) return nullptr;

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	pObject3D->Init();
	pObject3D->m_pPos->Set(pos);
	pObject3D->m_pRot->Set(rot);
	pObject3D->m_pSize->Set(size);
	pObject3D->SetOffsetVtx();
	pObject3D->m_nTextureIdx = pTexture->Register(pTextureName);

	return pObject3D;
}
