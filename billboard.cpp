//===================================================
//
// �r���{�[�h�̕`�� [billboard.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "billboard.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CObjectBillboard::CObjectBillboard(int nPriority) : CObject(nPriority)
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));
	m_pPos = nullptr;
	m_pSize = nullptr;
	m_nTextureIdx = NULL;
	m_pVtxBuffer = nullptr;
}

//===================================================
// �f�X�g���N�^
//===================================================
CObjectBillboard::~CObjectBillboard()
{
}

//===================================================
// ����������
//===================================================
HRESULT CObjectBillboard::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �ʒu�A�傫���̐���
	m_pPos = new CPosition;
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
void CObjectBillboard::Uninit(void)
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
void CObjectBillboard::Update(void)
{
}

//===================================================
// �`�揈��
//===================================================
void CObjectBillboard::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �v�Z�p�̃}�g���b�N�X��錾
	D3DXMATRIX mtxRot, mtxTrans;

	// ���C�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Z�e�X�g
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �r���[�}�g���b�N�X��錾
	D3DXMATRIX mtxView;

	// �r���[�}�g���b�N�X�̎擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �J�����̋t�s���ݒ�
	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	// �ʒu
	D3DXVECTOR3 pos = m_pPos->Get();

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//// Z�e�X�g
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ���C�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//===================================================
// ���_�̃I�t�Z�b�g�̐ݒ菈��
//===================================================
void CObjectBillboard::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
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
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

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
// ���_�̐F�̐ݒ�
//===================================================
void CObjectBillboard::SetColor(const D3DXCOLOR col)
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
// �e�N�X�`����ID�̐ݒ�
//===================================================
void CObjectBillboard::SetTextureID(const char* pTextureName)
{
	if (pTextureName == NULL)
	{
		m_nTextureIdx = -1;
		return;
	}
	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// ���_���W�̍X�V
//===================================================
void CObjectBillboard::UpdateVertexPos(const D3DXVECTOR3 pos)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// �ʒu�̐ݒ�
	m_pPos->Set(pos);

	// �傫���̎擾
	D3DXVECTOR3 Size = m_pSize->Get();

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// �e�N�X�`�����W�̐ݒ�
//===================================================
void CObjectBillboard::SetTextureVtx(const D3DXVECTOR2 tex,const D3DXVECTOR2 offtex)
{
	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(offtex.x, offtex.y);
	pVtx[1].tex = D3DXVECTOR2(offtex.x + tex.x, offtex.y);
	pVtx[2].tex = D3DXVECTOR2(offtex.x, offtex.y + tex.y);
	pVtx[3].tex = D3DXVECTOR2(offtex.x + tex.x, offtex.y + tex.y);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//===================================================
// ��������
//===================================================
CObjectBillboard* CObjectBillboard::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const char* pTextureName)
{
	CObjectBillboard* pBillboard = nullptr;
	
	// 3D�I�u�W�F�N�g�̐���
	pBillboard = new CObjectBillboard;

	// �D�揇�ʂ̎擾
	int nPriority = pBillboard->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pBillboard != nullptr)
	{
		// �������g�̔j��
		pBillboard->Release();

		// null�ɂ��Ă���
		pBillboard = nullptr;

		return nullptr;
	}

	if (pBillboard == nullptr) return nullptr;

	pBillboard->Init();
	pBillboard->m_pPos->Set(pos);
	pBillboard->m_pSize->Set(size);
	pBillboard->SetOffsetVtx();
	pBillboard->SetTextureID(pTextureName);
	return pBillboard;
}

//===================================================
// �R���X�g���N�^
//===================================================
CBillboardAnimation::CBillboardAnimation(int nPriority) : CObjectBillboard(nPriority)
{
	m_fPosTexH = NULL;
	m_fPosTexU = NULL;
	m_nAnimSpeed = NULL;
	m_nCounterAnim = NULL;
	m_nPatternAnim = NULL;
	m_nU = NULL;
	m_nV = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CBillboardAnimation::~CBillboardAnimation()
{
}

//===================================================
// �A�j���[�V�����̐ݒ菈��
//===================================================
void CBillboardAnimation::SetAnim(const int nAnimSpeed, const int U, const int V)
{
	m_nAnimSpeed = nAnimSpeed;
	m_nU = U;
	m_nV = V;
}

//===================================================
// ����������
//===================================================
HRESULT CBillboardAnimation::Init(void)
{
	// ����������
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CBillboardAnimation::Uninit(void)
{
	// �r���{�[�h�̔j��
	CObjectBillboard::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CBillboardAnimation::Update(void)
{
	// �r���{�[�h�̍X�V
	CObjectBillboard::Update();

	// �A�j���[�V�����̃J�E���^�[��i�߂�
	m_nCounterAnim++;

	m_fPosTexU = 1.0f / m_nU;
	m_fPosTexH = 1.0f / m_nV;

	if (m_nCounterAnim >= m_nAnimSpeed)
	{
		m_nCounterAnim = 0;

		m_nPatternAnim++;

		// Y���W�̊���
		int nRatePosY = m_nPatternAnim / m_nU;

		float UV = m_fPosTexU * m_nPatternAnim;
		float HV = nRatePosY * m_fPosTexH;

		// �e�N�X�`�����W�̍X�V
		CObjectBillboard::SetTextureVtx(D3DXVECTOR2(m_fPosTexU, m_fPosTexH), D3DXVECTOR2(UV, HV));

		// �ő�܂ōs������
		if (m_nPatternAnim >= (m_nU * m_nV))
		{
			Uninit();
		}
	}
}

//===================================================
// �`�揈��
//===================================================
void CBillboardAnimation::Draw(void)
{
	// �r���{�[�h�̕`��
	CObjectBillboard::Draw();
}
