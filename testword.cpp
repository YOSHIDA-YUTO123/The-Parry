//================================================
//
// �����e�X�g [testword.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// �C���N���[�h�t�@�C��
//************************************************
#include "testword.h"

//================================================
// �R���X�g���N�^
//================================================
CTestWord::CTestWord()
{
	m_nWord = NULL;
	m_pos = VEC3_NULL;
	m_fWidth = NULL;
	m_fHeight = NULL;
	m_pWord = nullptr;
}

//================================================
// �f�X�g���N�^
//================================================
CTestWord::~CTestWord()
{
}

//================================================
// ����������
//================================================
HRESULT CTestWord::Init(void)
{
	// �����̐���
	m_pWord = new CWord;

	if (m_pWord != nullptr && FAILED(m_pWord->Init(CWord::WORD_MAX, 1, m_pos, m_fWidth, m_fHeight)))
	{
		return E_FAIL;
	}
	m_pWord->SetTextureID("data/TEXTURE/word001.png");

	return S_OK;
}

//================================================
// �I������
//================================================
void CTestWord::Uninit(void)
{
	if (m_pWord != nullptr)
	{
		m_pWord->Uninit();

		delete m_pWord;

		m_pWord = nullptr;
	}

	Release();
}

//================================================
// �X�V����
//================================================
void CTestWord::Update(void)
{
	// �����̍X�V
	if (m_pWord != nullptr)
	{
		m_pWord->SetUV(m_nWord);
	}
}

//================================================
// �`�揈��
//================================================
void CTestWord::Draw(void)
{
	if (m_pWord != nullptr)
	{
		m_pWord->Draw();
	}
}

//================================================
// ��������
//================================================
CTestWord* CTestWord::Create(const int nWord, const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
{
	CTestWord* pWord = nullptr;

	// �����̐���
	pWord = new CTestWord;

	// �D�揇�ʂ̎擾
	int nPriority = pWord->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pWord != nullptr)
	{
		// �����̃|�C���^�̉��
		pWord->Release();

		// null�ɂ���
		pWord = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pWord == nullptr) return nullptr;

	pWord->m_pos = pos;
	pWord->m_fWidth = fWidth;
	pWord->m_fHeight = fHeight;
	pWord->m_nWord = nWord;
	pWord->Init();

	return pWord;
}
