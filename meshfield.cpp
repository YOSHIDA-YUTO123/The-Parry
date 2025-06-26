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

//************************************************
// �}�N����`
//************************************************
#define NUM_POLYGON (2)		 // ���肷��|���S���̐�
#define NULL_VECTOR (999.0f) // vector�̋�z��

//================================================
// �R���X�g���N�^
//================================================
CMeshField::CMeshField()
{
	m_fWidth = NULL;
	m_fHeight = NULL;
	memset(&m_Wave, NULL, sizeof(m_Wave));
	m_Nor = VEC3_NULL;
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

	// �e�N�X�`����ID�̐ݒ�
	CMesh::SetTextureID("data/TEXTURE/field.png");

	return S_OK;
}

//================================================
// �I������
//================================================
void CMeshField::Uninit(void)
{
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

	int nSegX = GetSegX();
	int nSegZ = GetSegZ();

	// ���_���̐ݒ�
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);
#if 1

	// �g�̍X�V����
	UpdateWave(nNumVtx);

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

	//for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	//{
	//	D3DXVECTOR3 pos = GetVtxPos(nCnt);

	//	D3DXVECTOR3 diff = pPlayer->GetPosition() - pos;

	//	float sqdiffX = diff.x * diff.x;
	//	float sqdiffY = diff.y * diff.y;
	//	float sqdiffZ = diff.z * diff.z;

	//	float fDistance = (sqdiffX + sqdiffY + sqdiffZ);

	//	float fRadius = 50.0f + 50.0f;

	//	fRadius = fRadius * fRadius;

	//	if (fDistance <= fRadius)
	//	{
	//		if (pKeyboard->GetPress(DIK_UP))
	//		{
	//			pos.y += 5.0f;
	//		}
	//		else if (pKeyboard->GetPress(DIK_DOWN))
	//		{
	//			pos.y -= 5.0f;
	//		}
	//	}
	//	SetVtxPos(pos, nCnt);
	//}

	// �@���̍Đݒ�
	UpdateNor();
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

			if (dot > 0.0f)
			{
				bLanding = true;
				break;
			}
		}
	}

	return bLanding;//�����Ԃ�
}

//================================================
// �g�̐ݒ菈��
//================================================
void CMeshField::SetWave(const D3DXVECTOR3 epicenter, const int nTime,const float fSpeed,const float fInRadius,const float fOutRadius,const float fWaveHeight,const float fcoef)
{
	if (m_Wave.bUse == false)
	{
		m_Wave.epicenter = epicenter;
		m_Wave.fSpeed = fSpeed;
		m_Wave.nTime = nTime;
		m_Wave.fInRadius = fInRadius;
		m_Wave.fOutRadius = fOutRadius;
		m_Wave.fHeight = fWaveHeight;
		m_Wave.fStartHeight = fWaveHeight;
		m_Wave.nCounter = NULL;
		m_Wave.fTime = NULL;
		m_Wave.fcoef = fcoef;

		m_Wave.bUse = true;
	}
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
			// ������������
			else if (nCntZ == nSegZ && nCntX == 0)
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
			// �E�̕�(�p�ȊO)��������
			else if (nCntX == nSegX && nCnt == (nCntX * (nCntZ + 1)) + nCntZ)
			{
				nIdx0 = nCnt - (nSegX + 1);
				nIdx1 = nCnt - 1;
				nIdx2 = nCntX + (nSegX + 1);

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
			// ����ȊO(�[�����ł��p�ł��Ȃ�)
			else
			{
				nIdx0 = nCnt - 1;
				nIdx1 = nCnt - (nSegX + 1);
				nIdx2 = nCnt + 1;
				nIdx3 = nCnt + (nSegX + 1);

				int nNumVertex = ((nSegX + 1) * (nSegZ + 1)) - 1;

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
// �g�̍X�V����
//================================================
void CMeshField::UpdateWave(const int nNumVtx)
{
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		D3DXVECTOR3 pos = GetVtxPos(nCnt);

		D3DXVECTOR3 diff = m_Wave.epicenter - pos;

		float dis = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

		// ���Ԃɉ�����������ݒ�
		float fTimeInRadius = m_Wave.fInRadius + m_Wave.fTime;
		float fTimeOutRadius = m_Wave.fOutRadius + m_Wave.fTime;

		if (dis >= fTimeInRadius && dis <= fTimeOutRadius && m_Wave.bUse == true)
		{
			float dest = m_Wave.fHeight + sinf(dis * m_Wave.fcoef);

			pos.y += (dest - pos.y) * 0.1f;
		}
		else
		{
			pos.y += (0.0f - pos.y) * 0.05f;
		}
		SetVtxPos(pos, nCnt);
	}

	float fRate = (float)m_Wave.nCounter / (float)m_Wave.nTime;

	if (m_Wave.bUse == true)
	{
		if (m_Wave.nCounter >= m_Wave.nTime)
		{
			m_Wave.bUse = false;
			m_Wave.nCounter = 0;
		}

		m_Wave.nCounter++;

		//m_fTime += 5.0f;
		m_Wave.fTime += m_Wave.fSpeed;

		m_Wave.fHeight = m_Wave.fStartHeight + (0.0f - m_Wave.fStartHeight) * fRate;
	}
}
