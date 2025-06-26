//================================================
//
// �����𑀍삷�鏈�� [word.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "word.h"
#include "object.h"
#include "manager.h"
#include "renderer.h"

//================================================
// �R���X�g���N�^
//================================================
CWord::CWord()
{
	m_pVtxBuffer = NULL;
	m_fHeight = NULL;
	m_fWidth = NULL;
	m_pos = VEC3_NULL;
	m_nTextureIdx = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CWord::~CWord()
{
}

//================================================
// ����������
//================================================
HRESULT CWord::Init(const int nPosX, const int nPosY, const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
{
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

	m_pos = pos;
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(pos.x - fWidth, pos.y - fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + fWidth, pos.y - fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - fWidth, pos.y + fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + fWidth, pos.y + fHeight, 0.0f);

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = WHITE;
	pVtx[1].col = WHITE;
	pVtx[2].col = WHITE;
	pVtx[3].col = WHITE;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / nPosX, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / nPosY);
	pVtx[3].tex = D3DXVECTOR2(1.0f / nPosX, 1.0f / nPosY);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//================================================
// �I������
//================================================
void CWord::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuffer != NULL)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = NULL;
	}
}

//================================================
// �X�V����
//================================================
void CWord::Update(void)
{
}

//================================================
// �`�揈��
//================================================
void CWord::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ���Ƃɖ߂��Ă���
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// �v���C���[�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // �v���~�e�B�u�̎��
}

//================================================
// �傫���̐ݒ菈��
//================================================
void CWord::SetSize(const float fWIdth, const float fHeight)
{
	m_fWidth = fWIdth;
	m_fHeight = fHeight;
}

//================================================
// �ʒu�̐ݒ菈��
//================================================
void CWord::SetPos(const D3DXVECTOR3 pos)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	m_pos = pos;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(pos.x - m_fWidth, pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + m_fWidth, pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - m_fWidth, pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + m_fWidth, pos.y + m_fHeight, 0.0f);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//================================================
// UV�̐ݒ菈��
//================================================
void CWord::SetUV(const int nNum)
{
	// ���_���̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	float offposX = 1.0f / CWord::WORD_MAX;

	float fPosTexU = offposX * nNum;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(fPosTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(offposX + fPosTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(fPosTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(offposX + fPosTexU, 1.0f);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//================================================
// �F�̐ݒ菈��
//================================================
void CWord::SetColor(const D3DXCOLOR col)
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

//================================================
// �e�N�X�`����ID�ݒ菈��
//================================================
void CWord::SetTextureID(const char* pFileName)
{
	// �e�N�X�`���̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	if (pFileName == NULL)
	{
		m_nTextureIdx = -1;
		return;
	}
	m_nTextureIdx = pTexture->Register(pFileName);
}
