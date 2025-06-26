//===================================================
//
// ����,�� [explosion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "explosion.h"
#include "manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CExplosion::CExplosion(int nPriority) : CBillboardAnimation(nPriority)
{
	m_col = WHITE;
}

//===================================================
// �f�X�g���N�^
//===================================================
CExplosion::~CExplosion()
{
}

//===================================================
// ��������
//===================================================
CExplosion* CExplosion::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed)
{
	// �����̐���
	CExplosion* pExplotion = new CExplosion;

	// �D�揇�ʂ̎擾
	int nPriority = pExplotion->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pExplotion != nullptr)
	{
		// �����̃|�C���^�̉��
		pExplotion->Release();

		// null�ɂ���
		pExplotion = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pExplotion == nullptr) return nullptr;

	pExplotion->m_col = col;
	pExplotion->SetAnim(nAnimSpeed, U, V);
	pExplotion->Init();
	pExplotion->GetPosition()->Set(pos);
	pExplotion->GetSize()->Set(size);
	pExplotion->SetOffsetVtx(col, U, V);

	return pExplotion;
}

//===================================================
// ����������
//===================================================
HRESULT CExplosion::Init(void)
{
	if (FAILED(CBillboardAnimation::Init()))
	{
		return E_FAIL;
	}

	// �e�N�X�`����ID�̐ݒ�
	SetTextureID("data/TEXTURE/explosion001.png");

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CExplosion::Uninit(void)
{
	// �I������
	CBillboardAnimation::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CExplosion::Update(void)
{
	// �X�V����
	CBillboardAnimation::Update();
}

//===================================================
// �`�揈��
//===================================================
void CExplosion::Draw(void)
{
	// �X�V����
	CBillboardAnimation::Draw();
}

//===================================================
// �R���X�g���N�^
//===================================================
CDustSmoke::CDustSmoke(int nPriority) : CObjectBillboard(nPriority)
{
	m_col = WHITE;
	m_move = VEC3_NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CDustSmoke::~CDustSmoke()
{
}

//===================================================
// ��������
//===================================================
CDustSmoke* CDustSmoke::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 size, const D3DXCOLOR col)
{
	// ���̐���
	CDustSmoke* pExplotion = new CDustSmoke;

	// �D�揇�ʂ̎擾
	int nPriority = pExplotion->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pExplotion != nullptr)
	{
		// �����̃|�C���^�̉��
		pExplotion->Release();

		// null�ɂ���
		pExplotion = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pExplotion == nullptr) return nullptr;

	pExplotion->m_col = col;
	pExplotion->Init();
	pExplotion->GetPosition()->Set(pos);
	pExplotion->GetSize()->Set(size);
	pExplotion->SetOffsetVtx(col, 1, 1);

	return pExplotion;
}

//===================================================
// ����������
//===================================================
HRESULT CDustSmoke::Init(void)
{
	// ����������
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	SetTextureID("data/TEXTURE/smoke.jpg");

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CDustSmoke::Uninit(void)
{
	// �I������
	CObjectBillboard::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CDustSmoke::Update(void)
{
	// �ʒu�̍X�V����
	GetPosition()->UpdatePosition(m_move);
}

//===================================================
// �`�揈��
//===================================================
void CDustSmoke::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �[�b�g�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//// �A���t�@�e�X�g��L��
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// a�u�����f�B���O
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �`�揈��
	CObjectBillboard::Draw();

	//�[�b�g�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//// �A���t�@�e�X�g�𖳌�
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// a�u�����f�B���O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
