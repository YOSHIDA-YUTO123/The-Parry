//===================================================
//
// �G�t�F�N�g [effect.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "effect.h"
#include "textureManager.h"
#include "manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CEffect3D::CEffect3D(int nPriority) : CObjectBillboard(nPriority)
{
	m_col = WHITE;
	m_decAlv = NULL;
	m_decRadius = NULL;
	m_fRadius = NULL;
	m_pMove = nullptr;
	m_nLife = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CEffect3D::~CEffect3D()
{
}

//===================================================
// ����������
//===================================================
HRESULT CEffect3D::Init(void)
{
	// ����������
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	// ID�̐ݒ�
	CObjectBillboard::SetTextureID("data/TEXTURE/effect000.jpg");

	// �ړ��N���X�̐���
	m_pMove = new CVelocity;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CEffect3D::Uninit(void)
{
	// �ړ��̔j��
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}
	// �I������
	CObjectBillboard::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CEffect3D::Update(void)
{
	// �ʒu�̍X�V
	GetPosition()->UpdatePosition(m_pMove->Get());

	// ���a�����炷
	m_fRadius -= m_decRadius;

	// �����x���グ��
	m_col.a -= m_decAlv;

	// ����
	m_nLife--;

	// �F�̐ݒ�
	SetColor(m_col);

	// �傫���̐ݒ�
	GetSize()->Set(D3DXVECTOR3(m_fRadius, m_fRadius, 0.0f));

	// �ʒu�̐ݒ�
	UpdateVertexPos(GetPosition()->Get());

	if (m_nLife <= 0)
	{
		// �j��
		Uninit();

		return;
	}
}

//===================================================
// �`�揈��
//===================================================
void CEffect3D::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �[�b�g�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// �A���t�@�e�X�g��L��
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// a�u�����f�B���O
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �`�揈��
	CObjectBillboard::Draw();

	//�[�b�g�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// �A���t�@�e�X�g�𖳌�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// a�u�����f�B���O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//===================================================
// ��������
//===================================================
CEffect3D* CEffect3D::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXVECTOR3 move, const D3DXCOLOR col, const int nLife)
{
	CEffect3D* pEffect = nullptr;

	// 3D�G�t�F�N�g�̐���
	pEffect = new CEffect3D;

	// �D�揇�ʂ̎擾
	int nPriority = pEffect->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pEffect != nullptr)
	{
		// �������g�̔j��
		pEffect->Uninit();

		// null�ɂ��Ă���
		pEffect = nullptr;

		return nullptr;
	}

	if (pEffect == nullptr) return nullptr;

	pEffect->Init();
	pEffect->GetPosition()->Set(pos);
	pEffect->GetSize()->Set(D3DXVECTOR3(fRadius, fRadius, 0.0f));
	pEffect->SetOffsetVtx(col);
	pEffect->m_col = col;
	pEffect->m_pMove->Set(move);
	pEffect->m_decAlv = col.a / nLife;
	pEffect->m_decRadius = fRadius / nLife;
	pEffect->m_fRadius = fRadius;
	pEffect->m_nLife = nLife;

	return pEffect;
}
