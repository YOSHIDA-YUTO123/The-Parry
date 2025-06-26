//================================================
//
// ���b�V���V�����_�[ [cylinder.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "cylinder.h"
#include "player.h"

//================================================
// �R���X�g���N�^
//================================================
CMeshCylinder::CMeshCylinder()
{
}

//================================================
// �f�X�g���N�^
//================================================
CMeshCylinder::~CMeshCylinder()
{
}

//================================================
// ��������
//================================================
CMeshCylinder* CMeshCylinder::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ,const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// ���b�V���t�B�[���h�𐶐�
	CMeshCylinder* pMesh = new CMeshCylinder;

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
	pMesh->SetCylinder(nSegX, nSegZ, fRadius, fHeight);

	return pMesh;
}

//================================================
// ����������
//================================================
HRESULT CMeshCylinder::Init(void)
{
	// ����������
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// �e�N�X�`����ID�̐ݒ�
	CMesh::SetTextureID("data/TEXTURE/field.jpg");

	return S_OK;
}

//================================================
// �I������
//================================================
void CMeshCylinder::Uninit(void)
{
	// �I������
	CMesh::Uninit();
}

//================================================
// �X�V����
//================================================
void CMeshCylinder::Update(void)
{
}

//================================================
// �`�揈��
//================================================
void CMeshCylinder::Draw(void)
{
	// �`�揈��
	CMesh::Draw();
}

//================================================
// �V�����_�[�̐ݒ菈��
//================================================
void CMeshCylinder::SetCylinder(const int nSegX, const int nSegZ, const float fRadius,const float fHeight)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f / nSegX;
	float fTexPosY = 1.0f / nSegZ;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			D3DXVECTOR3 posWk = VEC3_NULL;

			// ���̕�����
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			posWk.x = sinf(fAngle) * fRadius;
			posWk.y = fHeight - (fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * fRadius;

			// ���_�o�b�t�@�̐ݒ�
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)));

			// �@���̐��K��
			NormalizeNormal(nCntVtx);

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
