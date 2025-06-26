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

using namespace math;

//================================================
// �R���X�g���N�^
//================================================
CShadow::CShadow()
{
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
CShadow* CShadow::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const D3DXCOLOR col)
{
	CShadow* pShadow = nullptr;

	// �e�̐���
	pShadow = new CShadow;

	// �D�揇�ʂ̎擾
	int nPriority = pShadow->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pShadow != nullptr)
	{
		// �������g�̔j��
		pShadow->Release();

		// null�ɂ��Ă���
		pShadow = nullptr;

		return nullptr;
	}

	if (pShadow == nullptr) return nullptr;

	pShadow->Init();
	pShadow->GetPosition()->Set(pos);
	pShadow->GetSize()->Set(D3DXVECTOR3(fWidth, 0.0f, fHeight));
	pShadow->SetOffsetVtx(col);

	return pShadow;
}

//================================================
// ����������
//================================================
HRESULT CShadow::Init(void)
{
	if (FAILED(CObject3D::Init()))
	{
		return E_FAIL;
	}
	
	// �e�N�X�`����ID�̓o�^
	SetTextureID("data/TEXTURE/effect000.jpg");

	return S_OK;
}

//================================================
// �I������
//================================================
void CShadow::Uninit(void)
{
	CObject3D::Uninit();
}

//================================================
// �X�V����
//================================================
void CShadow::Update(void)
{

}

//================================================
// �`�揈��
//================================================
void CShadow::Draw(void)
{
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

	// �`�揈��
	CObject3D::Draw();

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Z�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//================================================
// �e�̈ʒu�̐ݒ菈��
//================================================
void CShadow::Setting(const D3DXVECTOR3 player, const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fMaxHeight, const float fAlv)
{
	// �����ƍő�̍����̊������v�Z
	float fWidthWk = player.y / fMaxHeight;
	float fHeightWk = player.y / fMaxHeight;

	// ���������Ƃɑ傫�������߂�
	float SetWidth = fWidth + (fWidthWk * fWidth);
	float SetHeight = fHeight + (fHeightWk * fHeight);

	UpdatePosition(pos, D3DXVECTOR3(SetWidth, 0.0f, SetHeight));

	float fAvalue = player.y / fMaxHeight;

	UpdateCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlv - fAvalue));
}

//===================================================
// �n�ʂɍ��킹����]�̎擾����
//===================================================
D3DXVECTOR3 CShadow::GetFieldAngle(D3DXVECTOR3 Nor, D3DXVECTOR3 up)
{
	D3DXVECTOR3 axis; // ��]��

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

	return rot;
}
