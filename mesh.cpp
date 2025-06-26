//================================================
//
// ���b�V�� [mesh.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "mesh.h"
#include "manager.h"
#include "renderer.h"

//================================================
// �R���X�g���N�^
//================================================
CMesh::CMesh()
{
	m_pVtxBuffMesh = nullptr;
	m_pIdxBuffMesh = nullptr;
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nSegmentX = NULL;	
	m_nSegmentZ = NULL;	
	m_nIdexTexture = -1;
	m_nNumVtx = NULL;	
	m_nNumPolygon = NULL;
	m_nNumIndex = NULL;	

}

//================================================
// �f�X�g���N�^
//================================================
CMesh::~CMesh()
{
}

//================================================
// ����������
//================================================
HRESULT CMesh::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//���_�o�b�t�@�̐���
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffMesh,
		NULL)))
	{
		return E_FAIL;
	}

	//�C���f�b�N�X�o�b�t�@�̐���
	if (FAILED(pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuffMesh,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//================================================
// �I������
//================================================
void CMesh::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuffMesh != nullptr)
	{
		m_pVtxBuffMesh->Release();
		m_pVtxBuffMesh = nullptr;
	}

	// �C���f�b�N�X�o�b�t�@�̔j��
	if (m_pIdxBuffMesh != nullptr)
	{
		m_pIdxBuffMesh->Release();
		m_pIdxBuffMesh = nullptr;
	}

	// �������g�̔j��
	Release();
}

//================================================
// �`�揈��
//================================================
void CMesh::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffMesh, 0, sizeof(VERTEX_3D));

	//�C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetIndices(m_pIdxBuffMesh);

	//�e�N�X�`���t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`��������������
	if (m_nIdexTexture == -1)
	{
		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);
	}
	else
	{
		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, pTexture->GetAdress(m_nIdexTexture));

	}

	//�|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0, m_nNumPolygon);
}

//================================================
// �`�揈��
//================================================
void CMesh::SetUpDraw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuffMesh, 0, sizeof(VERTEX_3D));

	//�C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetIndices(m_pIdxBuffMesh);

	//�e�N�X�`���t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`��������������
	if (m_nIdexTexture == -1)
	{
		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);
	}
	else
	{
		//�e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, pTexture->GetAdress(m_nIdexTexture));

	}
}

//================================================
// ���_���W�̐ݒ�
//================================================
void CMesh::SetVtxBuffer(const D3DXVECTOR3 pos, const int nIdx, const D3DXVECTOR2 tex, const D3DXVECTOR3 nor, const D3DXCOLOR col)
{
	VERTEX_3D* pVtx = NULL;

	// ���_�o�b�t�@�����b�N
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].pos = pos;

	//�@���x�N�g���̐ݒ�
	pVtx[nIdx].nor = nor;

	//���_�J���[�̐ݒ�
	pVtx[nIdx].col = col;

	//�e�N�X�`�����W�̐ݒ�
	pVtx[nIdx].tex = tex;

	// ���_�o�b�t�@���A�����b�N
	m_pVtxBuffMesh->Unlock();
}

//================================================
// �C���f�b�N�X�o�b�t�@�̐ݒ�
//================================================
void CMesh::SetIndexBuffer(const WORD Idx,const int nCnt)
{
	WORD* pIdx;

	//�C���f�b�N�X�o�b�t�@�̃��b�N
	m_pIdxBuffMesh->Lock(0, 0, (void**)&pIdx, 0);

	pIdx[nCnt] = Idx;

	//�C���f�b�N�X�o�b�t�@�̃A�����b�N
	m_pIdxBuffMesh->Unlock();
}

//================================================
// �e�N�X�`���̐ݒ菈��
//================================================
void CMesh::SetTextureID(const char* pTextureName)
{
	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	if (pTextureName == NULL)
	{
		m_nIdexTexture = -1;
		return;
	}

	// �e�N�X�`����ID�̐ݒ�
	m_nIdexTexture = pTexture->Register(pTextureName);
}

//================================================
// ���_�̗v�f�̐ݒ菈��
//================================================
void CMesh::SetVtxElement(const int vertex, const int polygon, const int index)
{
	// ���_���̐ݒ�
	m_nNumVtx = vertex;

	// �|���S�����̐ݒ�
	m_nNumPolygon = polygon;

	// �C���f�b�N�X���̐ݒ�
	m_nNumIndex = index;
}

//================================================
// �������̐ݒ菈��
//================================================
void CMesh::SetSegment(const int nSegX, const int nSegZ)
{
	m_nSegmentX = nSegX;
	m_nSegmentZ = nSegZ;
}

//================================================
// ���_���W�̎擾
//================================================
D3DXVECTOR3 CMesh::GetVtxPos(const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	D3DXVECTOR3 out;

	// ���_�o�b�t�@�����b�N
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	out = pVtx[nIdx].pos;

	// ���_�o�b�t�@���A�����b�N
	m_pVtxBuffMesh->Unlock();

	return out;
}

//================================================
// �C���f�b�N�X�ԍ��̎擾
//================================================
int CMesh::GetIndex(const int nIdx)
{
	WORD* pIdx;

	int out = 0;
	//�C���f�b�N�X�o�b�t�@�̃��b�N
	m_pIdxBuffMesh->Lock(0, 0, (void**)&pIdx, 0);

	out = pIdx[nIdx];

	//�C���f�b�N�X�o�b�t�@�̃A�����b�N
	m_pIdxBuffMesh->Unlock();

	return out;
}

//================================================
// ���_�̈ʒu�̐ݒ�
//================================================
void CMesh::SetVtxPos(const D3DXVECTOR3 pos,const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// ���_�o�b�t�@�����b�N
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].pos = pos;

	// ���_�o�b�t�@���A�����b�N
	m_pVtxBuffMesh->Unlock();
}

//================================================
// �@���̐ݒ�
//================================================
void CMesh::SetNormal(const D3DXVECTOR3 nor, const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// ���_�o�b�t�@�����b�N
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].nor = nor;

	// ���_�o�b�t�@���A�����b�N
	m_pVtxBuffMesh->Unlock();
}

//================================================
// �F�̐ݒ�
//================================================
void CMesh::SetColor(const D3DXCOLOR col,const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// ���_�o�b�t�@�����b�N
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].col = col;

	// ���_�o�b�t�@���A�����b�N
	m_pVtxBuffMesh->Unlock();
}

//================================================
// �@���̐��K������
//================================================
D3DXVECTOR3 CMesh::NormalizeNormal(const int nIdx)
{
	VERTEX_3D* pVtx = NULL;

	// ���_�o�b�t�@�����b�N
	m_pVtxBuffMesh->Lock(0, 0, (void**)&pVtx, 0);

	// �@��
	D3DXVECTOR3 nor = pVtx[nIdx].pos - m_pos;

	D3DXVec3Normalize(&nor, &nor);

	// ���_�o�b�t�@���A�����b�N
	m_pVtxBuffMesh->Unlock();

	return nor;
}
