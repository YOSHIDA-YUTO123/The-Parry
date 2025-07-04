//================================================
//
// �e [shadow.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "shadow.h"
#include "manager.h"
#include"renderer.h"
#include"math.h"

using namespace math; // ���O���math���g�p
using namespace std;  // ���O���std���g�p

//================================================
// �R���X�g���N�^
//================================================
CShadow::CShadow()
{
	m_col = WHITE;
	m_pos = VEC3_NULL;
	m_pVtxBuffer = nullptr;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nTextureIdx = -1;
}

//================================================
// �f�X�g���N�^
//================================================
CShadow::~CShadow()
{
}

//================================================
// ��������
//================================================
unique_ptr<CShadow> CShadow::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const D3DXCOLOR col)
{
	// �e�̐���
	unique_ptr<CShadow> pShadow = make_unique<CShadow>();

	if (pShadow == nullptr) return nullptr;

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �e�̃e�N�X�`���̐ݒ�
	pShadow->m_nTextureIdx = pTexture->Register("data/TEXTURE/effect000.jpg");

	// �ʒu�A�����A�傫���̐ݒ�
	pShadow->m_pRot = make_unique<CRotation>();
	pShadow->m_pSize = make_unique<CSize3D>();
	pShadow->m_pos = pos;
	pShadow->m_pSize->Set(D3DXVECTOR3(fWidth, 0.0f, fHeight));
	pShadow->m_col = col;
	pShadow->Init();
	
	return pShadow;
}

//================================================
// ����������
//================================================
HRESULT CShadow::Init(void)
{
	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

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
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//================================================
// �I������
//================================================
void CShadow::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

//================================================
// �X�V����
//================================================
void CShadow::Update(const D3DXVECTOR3 player, const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fMaxHeight, const float fAlv)
{
	// �����ƍő�̍����̊������v�Z
	float fWidthWk = player.y / fMaxHeight;
	float fHeightWk = player.y / fMaxHeight;

	// ���������Ƃɑ傫�������߂�
	float SetWidth = fWidth + (fWidthWk * fWidth);
	float SetHeight = fHeight + (fHeightWk * fHeight);

	// �ʒu�̐ݒ�
	m_pos = pos;

	// ���_���̃|�C���^
	VERTEX_3D* pVtx;

	// ���_�o�b�t�@�̃��b�N
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-SetWidth, 0.0f, SetHeight);
	pVtx[1].pos = D3DXVECTOR3(SetWidth, 0.0f, SetHeight);
	pVtx[2].pos = D3DXVECTOR3(-SetWidth, 0.0f, -SetHeight);
	pVtx[3].pos = D3DXVECTOR3(SetWidth, 0.0f, -SetHeight);

	float fAvalue = fabsf(player.y / fMaxHeight);

	m_col.a = fAlv - fAvalue;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// ���_�o�b�t�@�̃A�����b�N
	m_pVtxBuffer->Unlock();
}

//================================================
// �`�揈��
//================================================
void CShadow::Draw(void)
{
	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// Z�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// �����x�I�v�V�����ǉ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

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

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���ݒ�
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Z�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//===================================================
// �n�ʂɍ��킹����]�̎擾����
//===================================================
void CShadow::SetFieldAngle(D3DXVECTOR3 Nor, D3DXVECTOR3 up)
{
	D3DXVECTOR3 axis; // ��]��

	D3DXVec3Normalize(&Nor, &Nor);

	// �������߂�Đ��K������
	D3DXVec3Cross(&axis, &up, &Nor);
	D3DXVec3Normalize(&axis, &axis);

	// �t�B�[���h�̖@���Ə�����x�N�g���̓��ς��Ċp�x�����߂�
	float dot = D3DXVec3Dot(&up, &Nor);
	float fAngle = acosf(dot);

	D3DXMATRIX mtxRot; // �v�Z�p��]�s��

	// �C�ӂ̎������]�s������߂�
	D3DXMatrixRotationAxis(&mtxRot, &axis, fAngle);

	// ��]�s����I�C���[�p�ɕϊ�
	D3DXVECTOR3 rot = MatrixToEulerXYZ(mtxRot);

	// �ݒ�
	m_pRot->Set(rot);
}
