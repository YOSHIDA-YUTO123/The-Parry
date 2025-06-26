//================================================
//
// 文字テスト [testword.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "testword.h"

//================================================
// コンストラクタ
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
// デストラクタ
//================================================
CTestWord::~CTestWord()
{
}

//================================================
// 初期化処理
//================================================
HRESULT CTestWord::Init(void)
{
	// 文字の生成
	m_pWord = new CWord;

	if (m_pWord != nullptr && FAILED(m_pWord->Init(CWord::WORD_MAX, 1, m_pos, m_fWidth, m_fHeight)))
	{
		return E_FAIL;
	}
	m_pWord->SetTextureID("data/TEXTURE/word001.png");

	return S_OK;
}

//================================================
// 終了処理
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
// 更新処理
//================================================
void CTestWord::Update(void)
{
	// 文字の更新
	if (m_pWord != nullptr)
	{
		m_pWord->SetUV(m_nWord);
	}
}

//================================================
// 描画処理
//================================================
void CTestWord::Draw(void)
{
	if (m_pWord != nullptr)
	{
		m_pWord->Draw();
	}
}

//================================================
// 生成処理
//================================================
CTestWord* CTestWord::Create(const int nWord, const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
{
	CTestWord* pWord = nullptr;

	// 文字の生成
	pWord = new CTestWord;

	// 優先順位の取得
	int nPriority = pWord->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pWord != nullptr)
	{
		// 自分のポインタの解放
		pWord->Release();

		// nullにする
		pWord = nullptr;

		// オブジェクトを消す
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
