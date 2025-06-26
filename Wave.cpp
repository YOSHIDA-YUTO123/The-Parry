//================================================
//
// ���b�V���C���p�N�g [impact.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "Wave.h"
#include"manager.h"
#include"renderer.h"

//================================================
// �R���X�g���N�^
//================================================
CMeshWave::CMeshWave()
{
	m_fRadius = NULL;
	m_col = WHITE;
	m_fDecAlv = NULL;
	m_fSpeed = NULL;
	m_nLife = NULL;
	m_fHeight = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CMeshWave::~CMeshWave()
{
}

//================================================
// ��������
//================================================
CMeshWave* CMeshWave::Create(const D3DXVECTOR3 pos, const float fRadius, const float fHeight, const float speed, const int nLife, const int nSegX, const D3DXCOLOR col, const D3DXVECTOR3 rot)
{
	// ���b�V���E�F�[�u�𐶐�
	CMeshWave* pMesh = new CMeshWave;

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
	pMesh->m_fHeight = fHeight;
	pMesh->m_fRadius = fRadius;
	pMesh->m_nLife = nLife;
	pMesh->m_fSpeed = speed;
	pMesh->m_col = col;
	pMesh->m_fDecAlv = col.a / nLife;

	pMesh->SetWave(nSegX,fRadius, fHeight);

	return pMesh;
}

//================================================
// ����������
//================================================
HRESULT CMeshWave::Init(void)
{
	// ����������
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// �e�N�X�`����ID�̐ݒ�
	CMesh::SetTextureID("data/TEXTURE/dog.png");

	return S_OK;
}

//================================================
// �I������
//================================================
void CMeshWave::Uninit(void)
{
	// �I������
	CMesh::Uninit();
}

//================================================
// �X�V����
//================================================
void CMeshWave::Update(void)
{
	int nCntVtx = 0; // ���_���̃J�E���^�[

	// ���̕������̎擾
	int nSegX = GetSegX();

	float fPosTexV = 1.0f / nSegX; // ���̕�����

	// Z�̕������̎擾
	int nSegZ = GetSegZ();

	// ���a�̍X�V
	m_fRadius += m_fSpeed;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// ����̊��������߂�
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			// �v�Z�p�̈ʒu
			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngle) * m_fRadius;
			posWk.y = m_fHeight - (m_fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * m_fRadius;

			// �@���̐��K��
			D3DXVECTOR3 nor = NormalizeNormal(nCntVtx);

			// ���_���W�̐ݒ�
			SetVtxPos(posWk,nCntVtx);

			// ���_�J���[�̐ݒ�
			SetColor(m_col, nCntVtx);

			// �@���̐ݒ�
			SetNormal(nor,nCntVtx);

			// ���_���̃J�E���^�[�����Z
			nCntVtx++;
		}
	}

	// �����x��������
	m_col.a -= m_fDecAlv;

	// ���������炷
	m_nLife--;

	// �������s����
	if (m_nLife <= 0)
	{
		Uninit();

		return;
	}
}

//================================================
// �`�揈��
//================================================
void CMeshWave::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �|���S���̗��ʉ��I��
	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// a�u�����f�B���O
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// �A���t�@�e�X�g��L��
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// �`�揈��
	CMesh::Draw();

	// a�u�����f�B���O�����Ƃɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �A���t�@�e�X�g�𖳌�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// �|���S���̗��ʉ��I�t
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// �g�̐ݒ菈��
//================================================
void CMeshWave::SetWave(const int nSegX, const float fRadius, const float fHeight)
{
	int nCntVtx = 0; // ���_���̃J�E���^�[

	float fPosTexV = 1.0f / nSegX; // ���̕�����
	
	// Z�̕������̎擾
	int nSegZ = GetSegZ();

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// ����̊��������߂�
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			// �v�Z�p�̈ʒu
			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngle) * fRadius;
			posWk.y = fHeight - (fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * fRadius;

			// ���_�o�b�t�@�̐ݒ�
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fPosTexV * nCntX), 1.0f));

			// �@���̐��K��
			D3DXVECTOR3 nor = NormalizeNormal(nCntVtx);

			SetNormal(nor, nCntVtx);

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1; // �C���f�b�N�X�̐��l1

	int IdxCnt = 0; // �z��

	int Num = 0; // �C���f�b�N�X�̐��l2

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
