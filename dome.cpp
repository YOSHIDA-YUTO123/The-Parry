//================================================
//
// ���b�V���h�[�� [dome.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "dome.h"
#include"manager.h"
#include"renderer.h"

//================================================
// �R���X�g���N�^
//================================================
CMeshDome::CMeshDome()
{

}

//================================================
// �f�X�g���N�^
//================================================
CMeshDome::~CMeshDome()
{
}

//================================================
// ��������
//================================================
CMeshDome* CMeshDome::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// ���b�V���h�[���𐶐�
	CMeshDome* pMesh = new CMeshDome;

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

	if (pMesh == nullptr) return nullptr;

	// ���_���̐ݒ�
	int nNumFanVtx = (nSegX) + 1;

	// �|���S�����̐ݒ�
	int nNumFanPolygon = (nNumFanVtx) * 3;

	int nNumIdxFan = nNumFanPolygon + 2;
	// ���_���̐ݒ�
	int nNumDomeVtx = (nSegX + 1) * (nSegZ + 1);

	// �|���S�����̐ݒ�
	int nNumDomePolygon = (((nSegX * nSegZ) * 2)) + (4 * (nSegZ - 1));

	// �C���f�b�N�X���̐ݒ�
	int nNumDomeIndex = nNumDomePolygon + 2;

	// ���_���̐ݒ�
	int nNumVtx = nNumFanVtx + nNumDomeVtx;

	int nNumIdx = nNumDomeIndex + nNumIdxFan;

	// ���_�̐ݒ�
	pMesh->SetVtxElement(nNumVtx, nNumDomePolygon, nNumIdx);

	pMesh->SetSegment(nSegX, nSegZ);
	
	// ����������
	pMesh->Init();

	// �ݒ菈��
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);

	pMesh->SetDome(nSegX, nSegZ, fRadius, fHeight);
	return pMesh;
}

//================================================
// ����������
//================================================
HRESULT CMeshDome::Init(void)
{
	// ����������
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// �e�N�X�`����ID�̐ݒ�
	CMesh::SetTextureID("data/TEXTURE/sky.jpg");

	return E_FAIL;
}

//================================================
// �I������
//================================================
void CMeshDome::Uninit(void)
{
	//if (m_pIdxBuffMeshDome != nullptr)
	//{
	//	m_pIdxBuffMeshDome->Release();
	//	m_pIdxBuffMeshDome = nullptr;
	//}

	// �I������
	CMesh::Uninit();
}

//================================================
// �X�V����
//================================================
void CMeshDome::Update(void)
{
	UpdateRotation(D3DXVECTOR3(0.0f,0.001f,0.0f));
}

//================================================
// �`�揈��
//================================================
void CMeshDome::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// ���_���̐ݒ�
	int nNumFanVtx = nSegX + 1;

	// �|���S�����̐ݒ�
	int nNumFanPolygon = nNumFanVtx * 3;

	// ���_���̐ݒ�
	int nNumDomeVtx = (nSegX + 1) * (nSegZ + 1);

	// �|���S�����̐ݒ�
	int nNumDomePolygon = (((nSegX * nSegZ) * 2)) + (4 * (nSegZ - 1));

	// �`�揈��
	CMesh::SetUpDraw();

	//�|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, nNumFanVtx, 0, nNumFanPolygon);

	//�|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, nNumDomeVtx, m_nOffsetIdx, nNumDomePolygon);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// �h�[���̐ݒ菈��
//================================================
void CMeshDome::SetDome(const int nSegX, const int nSegZ, const float fRadius, const float fHeight)
{
	int nCntVtx = 0;
	float fTexX = 0.5f / nSegX;
	float fTexY = 0.5f / nSegZ;

	float fNowRadius = fRadius / (nSegZ + 1);

	// �W�̕����̍쐬
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		D3DXVECTOR3 posWk = VEC3_NULL,posOld = VEC3_NULL;

		posWk.x = sinf(fAngle) * fNowRadius;
		posWk.y = fHeight;
		posWk.z = cosf(fAngle) * fNowRadius;

		float fPosTexX = fTexX + cosf(fAngle) * fTexX;
		float fPosTexY = fTexY - sinf(fAngle) * fTexY;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fPosTexX, fPosTexY));

		// �Ō�܂ōs������
		if (nCntX == nSegX)
		{
			//// �Ō���Ȃ���
			//SetVtxBuffer(posOld, nCntVtx + 1, D3DXVECTOR2(fPosTexX, fPosTexY));
		}

		// �O��̌v�Z��ۑ�
		posOld = posWk;

		nCntVtx++;
	}
	
	int OffsetIdx = nCntVtx;

	// �h�[���̂Ă��؂�̈ʒu�̐ݒ�
	SetVtxBuffer(D3DXVECTOR3(0.0f, fHeight + (fHeight / nSegZ + 1),0.0f), 0, D3DXVECTOR2(fTexX, fTexY));

	int nCntIdx = 0;

	for (int IndxCount2 = 0; IndxCount2 < nSegX; IndxCount2++)
	{
		// �C���f�b�N�X�o�b�t�@�̐ݒ�
		SetIndexBuffer((WORD)0, nCntIdx);
		SetIndexBuffer((WORD)IndxCount2 + 1, nCntIdx + 1);
		SetIndexBuffer((WORD)IndxCount2 + 2, nCntIdx + 2);

		nCntIdx += 3;
	}

	m_nOffsetIdx = nCntIdx;

	// �e�N�X�`���̃I�t�Z�b�g
	fTexX = 1.0f / nSegX;
	fTexY = 1.0f / nSegZ;

	// ���a
	fNowRadius = fRadius / (nSegZ + 1);
	float fRateRadius = fNowRadius;

	//�c
	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		//��
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			float fAngel = (D3DX_PI * 2.0f) / nSegX * nCntX;

			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngel) * fNowRadius;
			posWk.y = fHeight - (fHeight / nSegZ) * nCntZ;
			posWk.z = cosf(fAngel) * fNowRadius;
			
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexX * nCntX, fTexY * nCntZ));

			nCntVtx++;//���Z
		}
		fNowRadius += fRateRadius / (nCntZ + 1);
	}

	int IndxCount3 = nSegX + 1;//X

	int IdxCnt = m_nOffsetIdx;//�z��

	int Num = 0;//

	int Index0 = 0;
	int Index1 = 0;

	//�C���f�b�N�X�̐ݒ�
	for (int IndxCount1 = 0; IndxCount1 < nSegZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegX; IndxCount2++, IndxCount3++, Num++)
		{
			Index0 = IndxCount3 + OffsetIdx;
			Index1 = Num + OffsetIdx;

			// �C���f�b�N�X�o�b�t�@�̐ݒ�
			SetIndexBuffer((WORD)Index0, IdxCnt);
			SetIndexBuffer((WORD)Index1, IdxCnt + 1);
			IdxCnt += 2;
		}

		if (IndxCount1 < nSegZ - 1)
		{
			// �C���f�b�N�X�o�b�t�@�̐ݒ�
			SetIndexBuffer((WORD)(Num - 1 + OffsetIdx), IdxCnt);
			SetIndexBuffer((WORD)(IndxCount3 + OffsetIdx), IdxCnt + 1);
			IdxCnt += 2;
		}
	}

}
