//================================================
//
// ���b�V���C���p�N�g [impact.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "impact.h"
#include "manager.h"
#include "renderer.h"

//================================================
// �R���X�g���N�^
//================================================
CMeshCircle::CMeshCircle()
{
	ZeroMemory(&m_Config, sizeof(m_Config));
	m_Incol = m_Outcol = WHITE;
	m_fDecAlv = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CMeshCircle::~CMeshCircle()
{
}

//================================================
// ��������
//================================================
CMeshCircle* CMeshCircle::Create(const Confing confing, const D3DXCOLOR col, const D3DXVECTOR3 pos, const int nSegX,const D3DXVECTOR3 rot)
{
	// ���b�V���C���p�N�g�𐶐�
	CMeshCircle* pMesh = new CMeshCircle;

	// �D�揇�ʂ̎擾
	int nPriority = pMesh->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pMesh != nullptr)
	{
		// �����̃|�C���^�̉��
		pMesh->Release();

		// null�ɂ���
		pMesh = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	// z�̕�����1�Œ�
	const int nSegZ = 1;

	if (pMesh == nullptr) return nullptr;

	// ���_���̐ݒ�
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// �|���S�����̐ݒ�
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// �C���f�b�N�X���̐ݒ�
	int nNumIndex = nNumPolygon + 2;

	// ���_�̐ݒ�
	pMesh->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMesh->SetSegment(nSegX, nSegZ);

	// ����������
	pMesh->Init();

	// �ݒ菈��
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);
	pMesh->m_Config = confing;
	pMesh->m_Outcol = col;
	pMesh->m_Incol = D3DXCOLOR(col.r, col.g, col.b, col.a * 0.5f);
	pMesh->m_fDecAlv = col.a / confing.nLife;

	pMesh->SetCircle(nSegX, confing.fInRadius, confing.fOutRadius);

	return pMesh;
}

//================================================
// ����������
//================================================
HRESULT CMeshCircle::Init(void)
{
	// ����������
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// �e�N�X�`����ID�̐ݒ�
	CMesh::SetTextureID();

	return E_FAIL;
}

//================================================
// �I������
//================================================
void CMeshCircle::Uninit(void)
{
	// �I������
	CMesh::Uninit();
}

//================================================
// �X�V����
//================================================
void CMeshCircle::Update(void)
{
	int nCntVtx = 0; // ���_���̃J�E���^�[

	int nSegX = GetSegX(); // ���̕������̎擾

	float fTexPosX = 1.0f / nSegX; // �e�N�X�`�����W�̊��������߂�

	// �X���[���[�V�����̎擾
	CSlow* pSlow = CManager::GetSlow();

	float fSlowLevel = pSlow->GetLevel(false);

	// ���a���g�傷��
	m_Config.fInRadius += m_Config.fSpeed * fSlowLevel;
	m_Config.fOutRadius += m_Config.fSpeed * fSlowLevel;

	// ���b�V���t�B�[���h�̎擾
	CMeshField* pMesh = CManager::GetMeshField();

	// ���_��y���W
	float fHeight = 0.0f;

	// �����̒��_�̍X�V
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		// ����̊��������߂�
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		D3DXVECTOR3 vtxpos = GetVtxPos(nCntVtx);

		// �ό`�ɒ��_������
		posWk.x = sinf(fAngle) * m_Config.fInRadius;
		posWk.y = vtxpos.y;
		posWk.z = cosf(fAngle) * m_Config.fInRadius;

		D3DXVECTOR3 pos = GetPosition();

		// �n�ʂƂ̓����蔻��
		if (pMesh->Collision(posWk + pos, &fHeight) && m_Config.bFiledCollision)
		{
			posWk.y = fHeight;
		}

		// ���_���W�̐ݒ�
		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f),D3DXVECTOR3(0.0f,1.0f,0.0f),m_Incol);

		// ���_�̃J�E���g�����Z
		nCntVtx++;
	}

	// �O���̒��_�̍X�V
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		D3DXVECTOR3 vtxpos = GetVtxPos(nCntVtx);

		posWk.x = sinf(fAngle) * m_Config.fOutRadius;
		posWk.y = vtxpos.y;
		posWk.z = cosf(fAngle) * m_Config.fOutRadius;

		D3DXVECTOR3 pos = GetPosition();

		// �n�ʂƂ̓����蔻��
		if (pMesh->Collision(posWk + pos, &fHeight) && m_Config.bFiledCollision)
		{
			posWk.y = fHeight;
		}

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), m_Outcol);

		nCntVtx++;
	}

	// a�l�����炷
	m_Incol.a -= m_fDecAlv;
	m_Outcol.a -= m_fDecAlv;

	// ���������炷
	m_Config.nLife--;

	// �������s������
	if (m_Config.nLife <= 0)
	{
		// �I������
		Uninit();

		return;
	}
}

//================================================
// �`�揈��
//================================================
void CMeshCircle::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// a�u�����f�B���O
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �`�揈��
	CMesh::Draw();

	// a�u�����f�B���O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// �C���p�N�g�̐ݒ菈��
//================================================
void CMeshCircle::SetCircle(const int nSegX,const float InRadius, const float OutRadius)
{
	int nCntVtx = 0; // ���_���̃J�E���^�[

	float fTexPosX = 1.0f / nSegX; // ���̃e�N�X�`�����W�����߂�

	D3DXVECTOR3 posWk = VEC3_NULL;

	// �����̒��_�̐���
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		// ����̊��������߂�
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		// �~�`�ɓ_������
		posWk.x = sinf(fAngle) * InRadius;
		posWk.y = m_Config.fHeight;
		posWk.z = cosf(fAngle) * InRadius;

		// ���_���W�̐ݒ�
		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		// ���_�̃J�E���^�[�����Z
		nCntVtx++;
	}

	// �O���̒��_�̐���
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		posWk.x = sinf(fAngle) * OutRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * OutRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		nCntVtx++;
	}

	int IndxNum = nSegX + 1; // �C���f�b�N�X�̐��l1

	int IdxCnt = 0; // �z��

	int Num = 0; // �C���f�b�N�X�̐��l2

	// Z�̕�����
	int nSegZ = GetSegZ();

	//�C���f�b�N�X�̐ݒ�
	for (int IndxCount1 = 0; IndxCount1 < nSegZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegX; IndxCount2++, IndxNum++, Num++)
		{
			// �C���f�b�N�X�o�b�t�@�̐ݒ�
			SetIndexBuffer((WORD)IndxNum, IdxCnt);
			SetIndexBuffer((WORD)Num, IdxCnt + 1);
			IdxCnt += 2;
		}

		// NOTE:�Ō�̍s����Ȃ�������
		if (IndxCount1 < nSegZ - 1)
		{
			SetIndexBuffer((WORD)Num - 1, IdxCnt);
			SetIndexBuffer((WORD)IndxNum, IdxCnt + 1);
			IdxCnt += 2;
		}
	}
}

