//================================================
//
// ���b�V���t�B�[���h [meshfield.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "meshfield.h"
#include"manager.h"
#include "math.h"
#include "debugproc.h"
#include "explosion.h"
#include "dust.h"
#include "LoadManager.h"
#include "effect.h"
#include"impact.h"

// �����̖��O��Ԃ��g�p
namespace
{
	constexpr int NUM_POLYGON = 2; // ���肷��|���S���̐�
	constexpr int NUM_SIRCLE = 5;  // ���b�V���T�[�N�����o����
}

//================================================
// �R���X�g���N�^
//================================================
CMeshField::CMeshField(int nPriority) : CMesh(nPriority)
{
	m_fWidth = NULL;
	m_fHeight = NULL;
	m_Nor = VEC3_NULL;
	m_pImpact = nullptr;
	//m_pWave = nullptr;
}

//================================================
// �f�X�g���N�^
//================================================
CMeshField::~CMeshField()
{
}

//================================================
// ��������
//================================================
CMeshField* CMeshField::Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ, const D3DXVECTOR2 Size, const D3DXVECTOR3 rot)
{
	// ���b�V���t�B�[���h�𐶐�
	CMeshField* pMeshField = new CMeshField;

	// �D�揇�ʂ̎擾
	int nPriority = pMeshField->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pMeshField != nullptr)
	{
		// �����̃|�C���^�̉��
		pMeshField->Release();

		// null�ɂ���
		pMeshField = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pMeshField == nullptr) return nullptr;
	
	// ���_���̐ݒ�
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// �|���S�����̐ݒ�
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// �C���f�b�N�X���̐ݒ�
	int nNumIndex = nNumPolygon + 2;

	// ���_�̐ݒ�
	pMeshField->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMeshField->SetSegment(nSegX, nSegZ);

	// ����������
	pMeshField->Init();

	// �ݒ菈��
	pMeshField->SetPosition(pos);
	pMeshField->SetRotation(rot);
	pMeshField->SetMeshField(nSegX, nSegZ, pos, Size);
	pMeshField->m_fWidth = Size.x;
	pMeshField->m_fHeight = Size.y;

	return pMeshField;
}

//================================================
// ����������
//================================================
HRESULT CMeshField::Init(void)
{
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// ���b�V���t�B�[���h�̃��[�h
	Load();

	return S_OK;
}

//================================================
// �I������
//================================================
void CMeshField::Uninit(void)
{
	// �g�̔j��
	for (int nCnt = 0; nCnt < (int)m_pWave.size(); nCnt++)
	{
		if (m_pWave[nCnt] != nullptr)
		{
			delete m_pWave[nCnt];
			m_pWave[nCnt] = nullptr;
		}
	}
	// �v�f�̃N���A
	m_pWave.clear();

	// �C���p�N�g�̔j��
	if (m_pImpact != nullptr)
	{
		m_pImpact->Uninit();
		delete m_pImpact;
		m_pImpact = nullptr;
	}

	// �I������
	CMesh::Uninit();
}

//================================================
// �X�V����
//================================================
void CMeshField::Update(void)
{
	//CPlayer* pPlayer = CManager::GetPlayer();
	//CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// �@���̍Đݒ�
	UpdateNor();

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// ���_���̐ݒ�
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);
#if 1

	// �v�f�����ׂ�
	for (int nCnt = 0; nCnt < (int)m_pWave.size(); nCnt++)
	{
		// �g�̍X�V����
		if (m_pWave[nCnt] != nullptr)
		{
			// �g�̍X�V����
			bool bResult = m_pWave[nCnt]->Update(this, nNumVtx);

			// �E�F�[�u����������
			if (m_pWave[nCnt] != nullptr && bResult == false)
			{
				// �E�F�[�u�̔j��
				delete m_pWave[nCnt];
				m_pWave[nCnt] = nullptr;
			}
		}
	}

	// null����Ȃ��Ȃ�
	if (m_pImpact != nullptr)
	{
		// �C���p�N�g�̍X�V����
		bool bResult = m_pImpact->Update(this, nNumVtx);

		// �C���p�N�g�̔j��
		if (bResult == false && m_pImpact != nullptr)
		{
			m_pImpact->Uninit();
			delete m_pImpact;
			m_pImpact = nullptr;
		}
	}

	// �v�f�����ׂ�
	for (int nCnt = 0; nCnt < (int)m_pWave.size(); nCnt++)
	{
		// �g���g���Ă���Ȃ�
		if (m_pWave[nCnt] != nullptr)
		{
			return;
		}
	}

	// �C���p�N�g���g���Ă���Ȃ�
	if (m_pImpact != nullptr)
	{
		return;
	}

	// ���_�̍�����0�ɖ߂�
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		D3DXVECTOR3 vtxPos = GetVtxPos(nCnt);

		if (vtxPos.y >= 0.0f)
		{
			vtxPos.y += (0.0f - vtxPos.y) * 0.01f;
			SetVtxPos(vtxPos, nCnt);
		}
	}

#endif // 0

	
#if 0

	m_Wave.fTime += 1.0f / 25.0f;
	
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		D3DXVECTOR3 pos = GetVtxPos(nCnt);

		D3DXVECTOR3 diff = pPlayer->GetPosition() - pos;

		float dis = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

		float offset = sinf((dis * 0.005f) - m_Wave.fTime);

		pos.y = offset * 150.0f;
		
		SetVtxPos(pos, nCnt);
	}
#endif // 0
}

//================================================
// �`�揈��
//================================================
void CMeshField::Draw(void)
{
	// �`�揈��
	CMesh::Draw();
}

//================================================
// ���b�V���t�B�[���h�̐ݒ菈��
//================================================
void CMeshField::SetMeshField(const int nSegX, const int nSegZ, const D3DXVECTOR3 pos,const D3DXVECTOR2 Size)
{
	int nCntVtx = 0;

	float fTexPosX = 1.0f + 1.0f / nSegX;
	float fTexPosY = 1.0f + 1.0f / nSegZ;

	D3DXVECTOR3 posWk;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// �ʒu�̐ݒ�
			posWk.x = ((Size.x / nSegX) * nCntX) - (Size.x * 0.5f);
			posWk.y = pos.y;
			posWk.z = Size.y - ((Size.y / nSegZ) * nCntZ) - (Size.y * 0.5f);

			// ���_�o�b�t�@�̐ݒ�
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fTexPosX * nCntX), (fTexPosY * nCntZ)));

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1;//X

	int IdxCnt = 0;//�z��

	int Num = 0;//

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

//================================================
// ���b�V���t�B�[���h�̓����蔻��
//================================================
bool CMeshField::Collision(const D3DXVECTOR3 pos,float *pOutHeight)
{
	// ���n����
	bool bLanding = false;

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// 1�}�X�̃T�C�Y
	float GridSizeX = m_fWidth / (float)nSegX;
	float GridSizeZ = m_fHeight / (float)nSegZ;

	float X = pos.x + (m_fWidth * 0.5f);
	float Z = (m_fHeight * 0.5f) - pos.z;

	// ���Ԗڂ̃|���S����
	int polyX = (int)(X / GridSizeX);
	int polyZ = (int)(Z / GridSizeZ);

	// ���݂̃|���S���̃C���f�b�N�X�ԍ�
	int polyIndex = ((polyZ * (nSegX - 1) + polyX) * 2) + (polyZ * 6);

	// �|���S�����̐ݒ�
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// �C���f�b�N�X���̐ݒ�
	int nNumIndex = nNumPolygon + 2;

	for (int nCnt = 0; nCnt < NUM_POLYGON; nCnt++)
	{
		// ���_�̃C���f�b�N�X
		int nCntVertex = (polyIndex + nCnt);

		// �}�C�i�X��������
		if (nCntVertex < 0 || nCntVertex > nNumIndex) break;

		// �C���f�b�N�X���擾
		int nIdx0 = GetIndex(nCntVertex);
		int nIdx1 = GetIndex(nCntVertex + 1);
		int nIdx2 = GetIndex(nCntVertex + 2);

		// ���_���擾
		D3DXVECTOR3 vtx0 = GetVtxPos(nIdx0);
		D3DXVECTOR3 vtx1 = GetVtxPos(nIdx1);
		D3DXVECTOR3 vtx2 = GetVtxPos(nIdx2);

		D3DXVECTOR3 edge0 = vtx1 - vtx0; // �Ӄx�N�g��0
		D3DXVECTOR3 edge1 = vtx2 - vtx1; // �Ӄx�N�g��1
		D3DXVECTOR3 edge2 = vtx0 - vtx2; // �Ӄx�N�g��2

		D3DXVECTOR3 Normal = {};

		if (nCnt % 2 == 0)
		{
			// �����Ԗڂ̎O�p�`
			D3DXVec3Cross(&Normal, &edge0, &edge1);
		}
		else
		{
			// ��Ԗڂ̎O�p�`�i�������t�ɂȂ��Ă���j
			D3DXVec3Cross(&Normal, &edge1, &edge0);
		}

		D3DXVec3Normalize(&Normal, &Normal);
		m_Nor = Normal;

		D3DXVECTOR3 PlayerVec0 = pos - vtx0;
		D3DXVECTOR3 PlayerVec1 = pos - vtx1;
		D3DXVECTOR3 PlayerVec2 = pos - vtx2;

		D3DXVECTOR3 Cross0 = {};
		D3DXVECTOR3 Cross1 = {};
		D3DXVECTOR3 Cross2 = {};

		if (nCnt % 2 == 0)
		{
			// �O�p�`�̒��_����O��
			D3DXVec3Cross(&Cross0, &edge0, &PlayerVec0);
			D3DXVec3Normalize(&Cross0, &Cross0);

			D3DXVec3Cross(&Cross1, &edge1, &PlayerVec1);
			D3DXVec3Normalize(&Cross1, &Cross1);

			D3DXVec3Cross(&Cross2, &edge2, &PlayerVec2);
			D3DXVec3Normalize(&Cross2, &Cross2);
		}
		else
		{
			// �O�p�`�̒��_����O��
			D3DXVec3Cross(&Cross0, &PlayerVec0, &edge0);
			D3DXVec3Normalize(&Cross0, &Cross0);

			D3DXVec3Cross(&Cross1, &PlayerVec1, &edge1);
			D3DXVec3Normalize(&Cross1, &Cross1);

			D3DXVec3Cross(&Cross2, &PlayerVec2, &edge2);
			D3DXVec3Normalize(&Cross2, &Cross2);
		}

		if (Cross0.y >= 0.0f && Cross1.y >= 0.0f && Cross2.y >= 0.0f)
		{
			//float xz = ((pPos->x - vtx0.x) * Normal.x + (pPos->z - pPos->z) * Normal.z);

			//float fHeight = vtx0.y - (xz / Normal.y);

			// ���ʂ̕�������D���v�Z
			float D = -(Normal.x * vtx0.x + Normal.y * vtx0.y + Normal.z * vtx0.z);

			// �v���C���[�̈ʒu�Ɋ�Â��āA�v���C���[��Y�ʒu���v�Z
			float PosY = (Normal.x * pos.x + Normal.z * pos.z + D) / -Normal.y;

			D3DXVECTOR3 field = GetPosition();

			D3DXVECTOR3 vec = vtx0 - pos;
			D3DXVec3Normalize(&vec, &vec);

			// �v���C���[���|���S���̗������ǂ����̔���
			float dot = D3DXVec3Dot(&Normal, &vec); // �@���ƃv���C���[�����Ƃ̓���

			*pOutHeight = field.y + PosY;

			if (dot >= 0.0f)
			{
				bLanding = true;
				break;
			}
		}
	}

	return bLanding;//�����Ԃ�
}

//================================================
// �@���̍Đݒ菈��
//================================================
void CMeshField::UpdateNor(void)
{
	int nCnt = 0;
	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// ���_�������ׂ�
	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// �v�Z�p���_0,1,2,3,4
			D3DXVECTOR3 vtx0, vtx1, vtx2, vtx3, vtx4;

			// �v�Z�p���_��������
			vtx0 = vtx1 = vtx2 = vtx3 = vtx4 = VEC3_NULL;

			// �v�Z�p�x�N�g��0,1,2,3
			D3DXVECTOR3 vec0, vec1, vec2, vec3;

			// �v�Z�p�x�N�g���̏�����
			vec0 = vec1 = vec2 = vec3 = VEC3_NULL;

			// �v�Z�p�C���f�b�N�X0,1,2,3�̐錾
			int nIdx0 = 0;
			int nIdx1 = 0;
			int nIdx2 = 0;
			int nIdx3 = 0;

			// �v�Z�p�@��0,1,2,3
			D3DXVECTOR3 Nor0, Nor1, Nor2, Nor3;

			// �v�Z�p�@���̏�����
			Nor0 = Nor1 = Nor2 = Nor3 = VEC3_NULL;

			// �v�Z��̖@���i�[�p�ϐ�
			D3DXVECTOR3 Normal = VEC3_NULL;

			// ���̏ゾ������
			if (nCntX == 0)
			{
				// ���ゾ������
				if (nCntZ == 0)
				{
					nIdx0 = 0;
					nIdx1 = 1;
					nIdx2 = nSegX + 1;

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// ������������
				else if (nCntZ == nSegZ)
				{
					nIdx0 = (nSegX + 1) * nSegZ;
					nIdx1 = (nSegX + 1) * (nSegZ - 1);
					nIdx2 = ((nSegX + 1) * nSegZ) + 1;

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);

					vec0 = vtx1 - vtx0;
					vec1 = vtx2 - vtx0;

					D3DXVec3Cross(&Normal, &vec0, &vec1);
				}
				// ���̕�(�p�ȊO)��������
				else if(nCnt == (nSegX + 1) * nCntZ)
				{
					nIdx0 = nCnt - (nSegX + 1);
					nIdx1 = nCnt + 1;
					nIdx2 = nCnt + (nSegX + 1);

					vtx0 = GetVtxPos(nIdx0);
					vtx1 = GetVtxPos(nIdx1);
					vtx2 = GetVtxPos(nIdx2);
					vtx3 = GetVtxPos(nCnt);

					vec0 = vtx0 - vtx3;
					vec1 = vtx1 - vtx3;
					vec2 = vtx2 - vtx3;

					D3DXVec3Cross(&Nor0, &vec0, &vec1);
					D3DXVec3Cross(&Nor1, &vec1, &vec2);

					Normal = (Nor0 + Nor1) * 0.5f;
				}
			}
			// ��̕ӂ�������
			else if (nCntZ == 0 && nCnt < nSegX)
			{
				nIdx0 = (nSegX + 1) + nCntX;
				nIdx1 = nCnt - 1;
				nIdx2 = nCnt + 1;

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);
				vtx3 = GetVtxPos(nIdx0);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;
				vec2 = vtx3 - vtx0;

				D3DXVec3Cross(&Nor0, &vec2, &vec0);
				D3DXVec3Cross(&Nor1, &vec1, &vec2);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// �E�ゾ������
			else if (nCntX == nSegX && nCntZ == 0)
			{
				nIdx0 = nSegX;
				nIdx1 = nSegX - 1;
				nIdx2 = nSegX + 1 + nCntX;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// ���̕ӂ�������
			else if (nCntZ == nSegZ && nCnt < ((nSegX + 1) * (nSegZ + 1)) - 1)
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegX + 1);
				nIdx2 = nCnt + 1;

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx0);
				vtx2 = GetVtxPos(nIdx1);
				vtx3 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;
				vec2 = vtx3 - vtx0;

				D3DXVec3Cross(&Nor0, &vec0, &vec1);
				D3DXVec3Cross(&Nor1, &vec1, &vec2);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// �E����������
			else if (nCnt == ((nSegX + 1) * (nSegZ + 1)) - 1)
			{
				nIdx0 = ((nSegX + 1) * (nSegZ + 1)) - 1;
				nIdx1 = nIdx0 - (nSegX + 1);
				nIdx2 = nIdx0 - 1;

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);

				vec0 = vtx1 - vtx0;
				vec1 = vtx2 - vtx0;

				D3DXVec3Cross(&Normal, &vec1, &vec0);
			}
			// �E�̕�(�p�ȊO)��������
			else if (nCntX == nSegX && nCnt == (nCntX * (nCntZ + 1)) + nCntZ)
			{
				nIdx0 = nCnt - (nSegX + 1);
				nIdx1 = nCnt - 1;
				nIdx2 = nCnt + (nSegX + 1);

				vtx0 = GetVtxPos(nIdx0);
				vtx1 = GetVtxPos(nIdx1);
				vtx2 = GetVtxPos(nIdx2);
				vtx3 = GetVtxPos(nCnt);

				vec0 = vtx0 - vtx3;
				vec1 = vtx1 - vtx3;
				vec2 = vtx2 - vtx3;

				D3DXVec3Cross(&Nor0, &vec1, &vec0);
				D3DXVec3Cross(&Nor1, &vec2, &vec1);

				Normal = (Nor0 + Nor1) * 0.5f;
			}
			// ����ȊO(�[�����ł��p�ł��Ȃ�)
			else
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegX + 1);
				nIdx2 = nCnt + 1;
				nIdx3 = nCnt + (nSegX + 1);

				vtx0 = GetVtxPos(nCnt);
				vtx1 = GetVtxPos(nIdx0);
				vtx2 = GetVtxPos(nIdx1);
				vtx3 = GetVtxPos(nIdx2);
				vtx4 = GetVtxPos(nIdx3);

				vec0 = vtx1 - vtx0; // ��
				vec1 = vtx2 - vtx0; // ��

				vec2 = vtx3 - vtx0; // �E
				vec3 = vtx4 - vtx0; // ��

				D3DXVec3Cross(&Nor0, &vec0, &vec1);
				D3DXVec3Cross(&Nor1, &vec3, &vec0);
				D3DXVec3Cross(&Nor2, &vec1, &vec2);
				D3DXVec3Cross(&Nor3, &vec2, &vec3);

				Normal = (Nor0 + Nor1 + Nor2 + Nor3) * 0.25f;
			}

			D3DXVec3Normalize(&Normal, &Normal);

			SetNormal(Normal, nCnt);
			nCnt++;
		}
	}
}

//================================================
// �E�F�[�u�̐ݒ菈��
//================================================
void CMeshField::SetWave(const D3DXVECTOR3 epicenter, const float InR, const float OutR, const float fHeight, const float fSpeed, const float fcoef, const int nTime)
{
	// �E�F�[�u�̐���
	CMeshFieldWave *pWave = CMeshFieldWave::Create(epicenter, InR, OutR, fHeight, fSpeed, fcoef, nTime);

	// �v�f�̒ǉ�
	m_pWave.push_back(pWave);
}

//================================================
// �C���p�N�g�̐ݒ菈��
//================================================
void CMeshField::SetImpact(const D3DXVECTOR3 pos, D3DXVECTOR3 dir, const float fHeight, const float fRadius, const int nTime, const float fSpeed, CMeshFieldImpact::OBJ attacker)
{
	// �C���p�N�g�̐���
	if (m_pImpact == nullptr)
	{
		// �C���p�N�g�̐���
		m_pImpact = CMeshFieldImpact::Create(pos, dir, fHeight, fRadius, nTime, fSpeed, attacker);
	}
}

//================================================
// ���b�V���t�B�[���h�̃��[�h
//================================================
void CMeshField::Load(void)
{
	fstream file("data/system.ini"); // �t�@�C�����J��
	string line; // �t�@�C���̕�����ǂݎ��p
	string input; // �l��������

	// �t�@�C�����J������
	if (file.is_open())
	{
		// ���[�h�̃}�l�[�W���̐���
		CLoadManager* pLoadManager = new CLoadManager;

		// �Ōザ��Ȃ��Ȃ�
		while (getline(file, line))
		{
			// �v���C���[�̃��[�V�����t�@�C����ǂݎ������
			if (line.find("FIELD_TEXTURE") != string::npos)
			{
				size_t equal_pos = line.find("="); // =�̈ʒu

				// [=] ���������߂�
				input = line.substr(equal_pos + 1);

				// �t�@�C���̖��O���擾
				string file_name = pLoadManager->GetString(input);

				// �t�@�C���̖��O����
				const char* FILE_NAME = file_name.c_str();

				// �e�N�X�`����ID�̐ݒ�
				CMesh::SetTextureID(FILE_NAME);
			}
		}

		// ���[�h�̃}�l�[�W���[�̔j��
		if (pLoadManager != nullptr)
		{
			delete pLoadManager;
			pLoadManager = nullptr;
		}
		// �t�@�C�������
		file.close();
	}
	else
	{
		MessageBox(NULL, "system.ini���J���܂���", "�t�@�C�������݂��܂���B", MB_OK | MB_ICONWARNING);
		return;
	}
}

//================================================
// �R���X�g���N�^
//================================================
CMeshFieldWave::CMeshFieldWave()
{
	m_epicenter = VEC3_NULL;
	m_fSpeed = NULL;
	m_nTime = NULL;
	m_fInRadius = NULL;
	m_fOutRadius = NULL;
	m_fHeight = NULL;
	m_fStartHeight = NULL;
	m_nCounter = NULL;
	m_fTime = NULL;
	m_fcoef = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CMeshFieldWave::~CMeshFieldWave()
{
}

//================================================
// ��������
//================================================
CMeshFieldWave* CMeshFieldWave::Create(const D3DXVECTOR3 epicenter, const float InR, const float OutR, const float fHeight, const float fSpeed, const float fcoef, const int nTime)
{
	// �g�̐���
	CMeshFieldWave* pWave = new CMeshFieldWave;

	// null��������
	if (pWave == nullptr) return nullptr;

	pWave->Init(); // ����������

	pWave->m_epicenter = epicenter;		// �����n�_
	pWave->m_fInRadius = InR;			// �����̔��a
	pWave->m_fOutRadius = OutR;			// �O���̂����a
	pWave->m_fHeight = fHeight;			// �g�̍���
	pWave->m_fStartHeight = fHeight;	// �ŏ��̔g�̍���
	pWave->m_fSpeed = fSpeed;			// �L���鑬��
	pWave->m_fcoef = fcoef;				// �W��
	pWave->m_nTime = nTime;				// �g�̔�������

	return pWave;
}

//================================================
// ����������
//================================================
void CMeshFieldWave::Init(void)
{
	m_nCounter = 0;
}

//================================================
// �X�V����
//================================================
bool CMeshFieldWave::Update(CMeshField* pMeshField,const int nNumVtx)
{
	// �g�̃J�E���^�[��i�߂�
	m_nCounter++;

	// ���Βl�����߂�
	float fRate = (float)m_nCounter / (float)m_nTime;

	// �����ɉ������g�̕���ݒ�
	m_fTime += m_fSpeed;

	// �g�̍��������񂾂�0�ɋ߂Â���
	m_fHeight = m_fStartHeight + (0.0f - m_fStartHeight) * fRate;

	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// ���_���W�̎擾
		D3DXVECTOR3 pos = pMeshField->GetVtxPos(nCnt);

		// �k���n���璸�_�܂ł̍���
		D3DXVECTOR3 diff = m_epicenter - pos;

		// ���������Ƃ߂�
		float dis = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

		// ���Ԃɉ�����������ݒ�
		float fTimeInRadius = m_fInRadius + m_fTime;
		float fTimeOutRadius = m_fOutRadius + m_fTime;

		// �͈͓���������
		if (dis >= fTimeInRadius && dis <= fTimeOutRadius)
		{
			//// �F�̕ύX
			//pMeshField->SetColor(D3DXCOLOR(fRate, fRate, fRate, 1.0f), nCnt);

			// �����̐ݒ�
			float dest = m_fHeight + sinf(dis * m_fcoef);

			// �ړI�̍����ɋ߂Â���
			pos.y += (dest - pos.y) * 0.1f;
		}
		else
		{
			// �ړI�̍����ɋ߂Â���
			pos.y += (0.0f - pos.y) * 0.05f;
		}

		// ���_���W�̐ݒ�
		pMeshField->SetVtxPos(pos, nCnt);
	}

	// �ő�l��������
	if (m_nCounter >= m_nTime)
	{
		// �J�E���^�[�����Z�b�g
		m_nCounter = 0;

		return false;
	}

	return true;
}

//================================================
// �R���X�g���N�^
//================================================
CMeshFieldImpact::CMeshFieldImpact()
{
	m_fHeight = NULL;
	m_fRadius = NULL;
	m_nCounter = NULL;
	m_nTime = NULL;
	m_pMove = nullptr;
	m_pos = VEC3_NULL;
	m_pSphere = nullptr;
	m_ObjType = OBJ::OBJ_NONE;
	m_FirstPos = VEC3_NULL;
	m_fSpeed = NULL;
}

//================================================
// �f�X�g���N�^
//================================================
CMeshFieldImpact::~CMeshFieldImpact()
{
}

//================================================
// ��������
//================================================
CMeshFieldImpact* CMeshFieldImpact::Create(const D3DXVECTOR3 pos,D3DXVECTOR3 dir,const float fHeight,const float fRadius,const int nTime, const float fSpeed, const OBJ attacker)
{
	// �Ռ��g�̐���
	CMeshFieldImpact* pImpact = new CMeshFieldImpact;

	// �ړ��ʂ̐���
	pImpact->m_pMove = new CVelocity;

	// �����蔻��̐���
	pImpact->m_pSphere = CCollisionSphere::Create(pos, fRadius);

	// �����x�N�g���ɂ���
	D3DXVec3Normalize(&dir, &dir);

	// y�͍l�����Ȃ�
	dir.y = 0.0f;

	// �ړ��ʂ̐ݒ�
	pImpact->m_pMove->Set(dir * fSpeed);

	// �ݒ菈��
	pImpact->m_pos = pos;
	pImpact->m_FirstPos = pos;
	pImpact->m_fHeight = fHeight;
	pImpact->m_fRadius = fRadius;
	pImpact->m_nCounter = 0;
	pImpact->m_nTime = nTime;
	pImpact->m_ObjType = attacker;
	pImpact->m_fSpeed = fSpeed;

	return pImpact;
}

//================================================
// �I������
//================================================
void CMeshFieldImpact::Uninit(void)
{
	// �ړ��ʂ̔j��
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}

	// �~�̔���̔j��
	if (m_pSphere != nullptr)
	{
		delete m_pSphere;
		m_pSphere = nullptr;
	}
}

//================================================
// �X�V����
//================================================
bool CMeshFieldImpact::Update(CMeshField* pMeshField, const int nNumVtx)
{
	// null����Ȃ��Ȃ�
	if (m_pMove != nullptr)
	{
		// �ړ��ʂ̍X�V����
		m_pos += m_pMove->Get();
	}

	// ������їʂ�I�o
	float dir = rand() % 15 + 5.0f;
	float Jump = rand() % 15 + 25.0f;

	float fAngle = (float)(rand() % 629 - 314);

	// �����ɉ�����������їʂ��v�Z
	float fMoveX = sinf(fAngle) * dir;
	float fMoveZ = cosf(fAngle) * dir;

	// ������I�o
	int nLife = rand() % 120 + 60;

	// ���I�𐶐�
	CRubble::Create(m_pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, CRubble::TYPE_THREE);

	// null����Ȃ��Ȃ�
	if (m_pSphere != nullptr)
	{
		// �ʒu�̍X�V����
		m_pSphere->SetPos(m_pos);
	}

#ifdef _DEBUG

	CEffect3D::Create(m_pos, m_fRadius, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

	// �g�̃J�E���^�[��i�߂�
	m_nCounter++;

	// �C���p�N�g���o���^�C�~���O�����߂�
	int SetImpact = m_nTime / NUM_SIRCLE;

	// �����΂�ŏ��ɏo���A
	if (SetImpact != 0 && (m_nCounter % SetImpact == 0 || m_nCounter == 0))
	{
		// �p�x�����߂�
		float rotY = atan2f(m_pMove->Get().x, m_pMove->Get().z);

		// �T�[�N���̐���
		CMeshCircle::Create(m_pos, 0.0f, 50.0f, 10.0f, 60, 50.0f, 32, WHITE, D3DXVECTOR3(D3DX_PI * 0.5f, rotY, 0.0f), false);
	}

	// ���_�������ׂ�
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// ���_�̈ʒu�̎擾
		D3DXVECTOR3 vtxPos = pMeshField->GetVtxPos(nCnt);

		// null����Ȃ��Ȃ�
		if (m_pSphere != nullptr)
		{
			// �R���C�_�[�̍쐬
			CCollisionSphere spere = m_pSphere->CreateCollider(vtxPos, 50.0f);

			// �~�Ɖ~�̔���
			if (m_pSphere->Collision(&spere))
			{
				vtxPos.y += (m_fHeight - vtxPos.y) * 0.3f;
			}
			else
			{
				// �ړI�̍����ɋ߂Â���
				vtxPos.y += (0.0f - vtxPos.y) * 0.05f;
			}

			pMeshField->SetVtxPos(vtxPos, nCnt);
		}
	}

	// �ő�l��������
	if (m_nCounter >= m_nTime)
	{
		// �J�E���^�[�����Z�b�g
		m_nCounter = 0;

		return false;
	}

	return true;
}

//================================================
// �����蔻��
//================================================
bool CMeshFieldImpact::Collision(const D3DXVECTOR3 pos, const float fRadius,const OBJ myObj)
{
	// �R���C�_�[�̍쐬
	CCollisionSphere sphere = m_pSphere->CreateCollider(pos, fRadius);

	// �����蔻��
	if (m_pSphere->Collision(&sphere) && myObj != m_ObjType)
	{
		return true;
	}

	return false;
}

//================================================
// �ݒ菈��
//================================================
void CMeshFieldImpact::Reset(D3DXVECTOR3 dir,const OBJ obj,const D3DXVECTOR3 FirstPos)
{
	m_nCounter = 0; 				// �J�E���^�[�����Z�b�g
	m_FirstPos = FirstPos; 			// ���˒n�_��ݒ�
	m_ObjType = obj;				// ���˂����I�u�W�F�N�g��ݒ�
	D3DXVec3Normalize(&dir, &dir);	// �����̐��K��
	m_pMove->Set(dir * m_fSpeed);	// �ړ��ʂ�ݒ�
}
