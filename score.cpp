//================================================
//
// �X�R�A [score.cpp]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// �C���N���[�h�t�@�C��
//*************************************************
#include "score.h"
#include"manager.h"
#include"renderer.h"

//*************************************************
// �ÓI�����o�ϐ��̐錾
//*************************************************
CNumber* CScore::m_apNumber[MAX_DIGIT] = {};	// �i���o�[�I�u�W�F�N�g�ւ̃|�C���^

//=================================================
// �R���X�g���N�^
//=================================================
CScore::CScore(int nPriority) : CObject(nPriority)
{
	m_fHeight = NULL;
	m_fWidth = NULL;
	m_pos = VEC3_NULL;
	m_nScore = NULL;
}

//=================================================
// �f�X�g���N�^
//=================================================
CScore::~CScore()
{
}

//=================================================
// ����������
//=================================================
HRESULT CScore::Init(void)
{
	// �����̊��������߂�
	float fWidth = m_fWidth / MAX_DIGIT;

	// �I�t�Z�b�g
	float offPosX = fWidth * 2.0f;

	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// �i���o�[�̃������̊m��
		if (m_apNumber[nCnt] == NULL)
		{
			m_apNumber[nCnt] = new CNumber;

			// ����������
			m_apNumber[nCnt]->Init(9, 1, D3DXVECTOR3(m_pos.x - (offPosX * nCnt), m_pos.y, 0.0f), fWidth, m_fHeight);
			m_apNumber[nCnt]->SetTextureID("data/TEXTURE/number005.png");
		}
	}

	return S_OK;
}

//=================================================
// �I������
//=================================================
void CScore::Uninit(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// �i���o�[�I�u�W�F�N�g�̉��
		if (m_apNumber[nCnt] != NULL)
		{
			m_apNumber[nCnt]->Uninit();

			delete m_apNumber[nCnt];

			m_apNumber[nCnt] = NULL;
		}
	}

	Release();
}

//=================================================
// �X�V����
//=================================================
void CScore::Update(void)
{
	// �X�R�A�̐���
	if (m_nScore >= MAX_SCORE)
	{
		m_nScore = MAX_SCORE;
	}
	else if (m_nScore <= MIN_SCORE)
	{
		m_nScore = MIN_SCORE;
	}

	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// �i���o�[�I�u�W�F�N�g�̍X�V
		if (m_apNumber[nCnt] != NULL)
		{
			m_apNumber[nCnt]->Update();

			// �X�R�A������l
			int nData = 10;

			// ���ɉ������l�����߂�
			int nDivi = (int)pow((double)nData, (double)nCnt);

			// �e�N�X�`�����W�����߂�
			int nNumber = m_nScore / nDivi % 10;

			m_apNumber[nCnt]->SetUV(nNumber);
		}
	}
}

//=================================================
// �`�揈��
//=================================================
void CScore::Draw(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// �i���o�[�I�u�W�F�N�g�̕`��
		if (m_apNumber[nCnt] != NULL)
		{
			// �`�揈��
			m_apNumber[nCnt]->Draw();
		}
	}

}

//=================================================
// �傫���̐ݒ菈��
//=================================================
void CScore::SetSize(const float fWidth, const float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}

//=================================================
// �쐬����
//=================================================
CScore* CScore::Create(const TYPE type, const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
{
	// �X�R�A�I�u�W�F�N�g�ւ̃|�C���^
	CScore* pScore = NULL;

	// ��ނ̑J��
	switch (type)
	{
	case TYPE::TYPE_NORMAL:
		// �X�R�A�̃������̊m��
		pScore = new CScore;
		break;
	case TYPE::TYPE_LERPER:
		// �X�R�A�̃������̊m��
		pScore = new CScoreLerper;
		break;
	default:
		return nullptr;
		break;
	}

	// �����̗D�揇�ʂ̎擾
	const int nPriority = pScore->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pScore != nullptr)
	{
		// �����̃|�C���^�̉��
		pScore->Release();

		// null�ɂ���
		pScore = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	// NULL���������΂�
	if (pScore == nullptr) return nullptr;

	// �傫���̐ݒ菈��
	pScore->SetSize(fWidth, fHeight);

	// �ʒu�̐ݒ�
	pScore->SetPosition(pos);

	// ����������
	pScore->Init();

	return pScore;
}

//=================================================
// �R���X�g���N�^
//=================================================
CScoreLerper::CScoreLerper(int nPriority) : CScore(nPriority)
{
	m_nDestScore = NULL;
}

//=================================================
// �f�X�g���N�^
//=================================================
CScoreLerper::~CScoreLerper()
{
}

//=================================================
// ����������
//=================================================
HRESULT CScoreLerper::Init(void)
{
	// �X�R�A�̏���������
	if (FAILED(CScore::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//=================================================
// �I������
//=================================================
void CScoreLerper::Uninit(void)
{
	// �I������
	CScore::Uninit();
}

//=================================================
// �X�V����
//=================================================
void CScoreLerper::Update(void)
{
	// �X�R�A�̎擾
	int score = GetScore();

	// �������v�Z
	float fRateTime = (float)m_nCounter / (float)m_nTime;
	 
	// �J�E���^�[�̐���
	if (m_nCounter >= m_nTime)
	{
		m_nCounter = m_nTime;
	}
	else
	{
		m_nCounter++;
	}

	// �X�R�A��ړI�̃X�R�A�ɋ߂Â���
	score += (int)((m_nDestScore - score) * fRateTime);

	// �X�R�A�̐ݒ�
	CScore::SetScore(score);

	// �X�V����
	CScore::Update();
}

//=================================================
// �`�揈��
//=================================================
void CScoreLerper::Draw(void)
{
	// �`�揈��
	CScore::Draw();
}

//=================================================
// �X�R�A�̐ݒ菈��
//=================================================
void CScoreLerper::SetDestScore(const int value, const int nTime)
{
	m_nDestScore += value;
	m_nTime = nTime;
	m_nCounter = NULL;
}
