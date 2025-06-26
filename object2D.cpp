//===================================================
//
// �I�u�W�F�N�g2D�̕`�揈�� [object2D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "object2D.h"
#include"renderer.h"
#include "manager.h"

//***************************************************
// �ÓI�����o�ϐ��̐錾
//***************************************************

//===================================================
// �R���X�g���N�^
//===================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	m_pPos = nullptr;
	m_pRot = nullptr;
	m_pSize = nullptr;

	m_pVtxBuffer = NULL;
	m_fAngle = 0.0f;
	m_Length = 0.0f;
}
//===================================================
// �f�X�g���N�^
//===================================================
CObject2D::~CObject2D()
{
}

//===================================================
// ����������
//===================================================
HRESULT CObject2D::Init(void)
{
	m_fAngle = 0.0f;
	m_Length = 0.0f;

	m_pPos = new CPosition;
	m_pRot = new CRotation;
	m_pSize = new CSize2D;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//���_�o�b�t�@�̐����E���_���̐ݒ�
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
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
void CObject2D::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuffer != NULL)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = NULL;
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
void CObject2D::Update(void)
{	
	CObject2D::UpdateVertex();
}
//===================================================
// �`�揈��
//===================================================
void CObject2D::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	if (m_nTextureIdx != -1)
	{
		// NULL�ɖ߂�
		pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));
	}
	else
	{
		// �e�N�X�`�����Ȃ�
		pDevice->SetTexture(0, NULL);
	}
	// �v���C���[�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // �v���~�e�B�u�̎��	
}

//===================================================
// ���_�̃I�t�Z�b�g�̐ݒ菈��
//===================================================
void CObject2D::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pPos->Get();
	D3DXVECTOR3 rot = m_pRot->Get();

	D3DXVECTOR2 Size = m_pSize->Get();

	m_Length = sqrtf((Size.x * Size.x) + (Size.y * Size.y));
	m_fAngle = atan2f(Size.x, Size.y);

	// ���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.y = pos.y + cosf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.y = pos.y + cosf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.z = 0.0f;

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

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
// �I�u�W�F�N�g2D�̐�������
//===================================================
CObject2D* CObject2D::Create(const float fWidth, const float fHeight, const D3DXVECTOR3 pos)
{
	CObject2D* pObject2D = nullptr;

	// 2D�I�u�W�F�N�g�̐���
	pObject2D = new CObject2D;

	// �D�揇�ʂ̎擾
	int nPriority = pObject2D->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pObject2D != nullptr)
	{
		// �������g�̔j��
		pObject2D->Release();

		// null�ɂ��Ă���
		pObject2D = nullptr;

		return nullptr;
	}

	if (pObject2D == nullptr) return nullptr;

	pObject2D->Init();
	pObject2D->m_pPos->Set(pos);
	pObject2D->m_pSize->Set(fWidth,fHeight);
	pObject2D->SetSize(fWidth, fHeight);
	pObject2D->SetOffsetVtx();
	pObject2D->SetTextureID();

	return pObject2D;
}
//===================================================
// �I�u�W�F�N�g�̑傫���̐ݒ菈��
//===================================================
void CObject2D::SetSize(const float fWidth, const float fHeight)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pPos->Get();
	D3DXVECTOR3 rot = m_pRot->Get();

	// �傫���̐ݒ菈��
	m_pSize->Set(fWidth, fHeight);

	m_Length = sqrtf((fWidth * fWidth) + (fHeight * fHeight));
	m_fAngle = atan2f(fWidth, fHeight);

	// ���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.y = pos.y + cosf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.y = pos.y + cosf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.z = 0.0f;

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �傫���̐ݒ菈��
//===================================================
void CObject2D::SetSize(const float leftWidth, const float rightWdth, const float topHeight, const float buttom)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// �ʒu
	D3DXVECTOR3 pos = m_pPos->Get();

	// �傫���̐ݒ菈��
	m_pSize->Set(rightWdth, topHeight);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(pos.x - leftWidth, pos.y - topHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + rightWdth, pos.y - topHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - leftWidth, pos.y + buttom, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + rightWdth, pos.y + buttom, 0.0f);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}
//===================================================
// �ʒu�̍X�V
//===================================================
void CObject2D::UpdateVertex(void)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pPos->Get();
	D3DXVECTOR3 rot = m_pRot->Get();

	// ���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.y = pos.y + cosf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.y = pos.y + cosf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.z = 0.0f;

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}
//===================================================
// �e�N�X�`�����W�̐ݒ�
//===================================================
void CObject2D::SetUvPos(const D3DXVECTOR2 OffPosTex,const D3DXVECTOR2 PosTex)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(OffPosTex.x, OffPosTex.y);
	pVtx[1].tex = D3DXVECTOR2(OffPosTex.x + PosTex.x, OffPosTex.y);
	pVtx[2].tex = D3DXVECTOR2(OffPosTex.x, OffPosTex.y + PosTex.y);
	pVtx[3].tex = D3DXVECTOR2(OffPosTex.x + PosTex.x, OffPosTex.y + PosTex.y);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �F�̐ݒ菈��
//===================================================
void CObject2D::SetColor(const D3DXCOLOR col)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

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
// �e�N�X�`���̐ݒ菈��
//===================================================
void CObject2D::SetTextureID(const char* pTextureName)
{
	// �e�N�X�`��������������
	if (pTextureName == NULL)
	{
		m_nTextureIdx = -1;
		return;
	}

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	m_nTextureIdx = pTexture->Register(pTextureName);
}
