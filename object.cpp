//===================================================
//
// オブジェクトの描画処理 [object.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"
#include "camera.h"
#include "manager.h"

using namespace Const; // 名前空間Constを使用

//***************************************************
// 静的メンバ変数の宣言
//***************************************************
CObject* CObject::m_pTop[NUM_PRIORITY] = { nullptr }; // 先頭オブジェクトへのポインタ
CObject* CObject::m_pCur[NUM_PRIORITY] = { nullptr }; // 最後尾オブジェクトへのポインタ

//CObject* CObject::m_apObject[MAX_OBJECT][NUM_PRIORITY] = {};	// オブジェクトのポインタ
int CObject::m_nNumAll[NUM_PRIORITY] = { NULL };				// オブジェクトの総数

//===================================================
// コンストラクタ(オーバーロード)
//===================================================
CObject::CObject(int nPriority)
{
	m_type = TYPE_NONE;
	m_nPriority = nPriority;

	// 先頭がnullだったら
	if (m_pTop[nPriority] == nullptr)
	{
		// 先頭を設定
		m_pTop[nPriority] = this;
		m_pTop[nPriority]->m_pPrev = nullptr;
		m_pTop[nPriority]->m_pNext = nullptr;
	}

	// 最後尾がnullだったら
	if (m_pCur[nPriority] == nullptr)
	{
		// 最後尾を設定
		m_pCur[nPriority] = this;
		m_pPrev = nullptr;
		m_pNext = nullptr;
	}
	else
	{
		// 今の最後尾を一つ前にする
		m_pPrev = m_pCur[nPriority];

		if (m_pPrev != nullptr)
		{
			// 最後尾から見た次に代入
			m_pPrev->m_pNext = this;
		}

		// 最後尾を変更
		m_pCur[nPriority] = this;
		m_pCur[nPriority]->m_pNext = nullptr;
		m_pCur[nPriority]->m_pPrev = m_pPrev;
	}

	// 総数をカウント
	m_nNumAll[nPriority]++;
}
//===================================================
// デストラクタ
//===================================================
CObject::~CObject()
{
}
//===================================================
// すべてのオブジェクトの破棄
//===================================================
void CObject::ReleaseAll(void)
{
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// 先頭オブジェクトを代入
		CObject* pObject = m_pTop[nCntPriority]; 

		// nullじゃないなら
		while (pObject != nullptr)
		{
			// 次のオブジェクトのポインタを代入
			CObject* pObjectNext = pObject->m_pNext; 

			// 更新処理
			pObject->Uninit();

			// 次のオブジェクトを代入
			pObject = pObjectNext; 
		}
	}
}
//===================================================
// すべてのオブジェクトの更新処理
//===================================================
void CObject::UpdateAll(void)
{
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// 先頭オブジェクトを代入
		CObject* pObject = m_pTop[nCntPriority]; 

		// nullじゃないなら
		while (pObject != nullptr)
		{
			// 次のオブジェクトのポインタを代入
			CObject* pObjectNext = pObject->m_pNext; 

			// 更新処理
			pObject->Update();

			// 次のオブジェクトを代入
			pObject = pObjectNext;
		}
	}
}
//===================================================
// すべてのオブジェクトの描画処理
//===================================================
void CObject::DrawAll(void)
{
	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	pCamera->SetCamera();

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority]; // 先頭オブジェクトを代入

		// nullじゃないなら
		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext; // 次のオブジェクトのポインタを代入

			// 更新処理
			pObject->Draw();

			pObject = pObjectNext; // 次のオブジェクトを代入
		}
	}
}

//===================================================
// オブジェクトの破棄
//===================================================
void CObject::Release(void)
{
	// 自分の優先順位
	int nPriority = m_nPriority;

	// 先頭だったら
	if (this == m_pTop[nPriority] && m_pNext != nullptr)
	{
		// 先頭を次のオブジェクトにする
		m_pTop[nPriority] = m_pNext;
		m_pNext->m_pPrev = nullptr;
	}
	if (this == m_pCur[nPriority] && m_pPrev != nullptr)
	{
		// 先頭を次のオブジェクトにする
		m_pCur[nPriority] = m_pPrev;
		m_pPrev->m_pNext = nullptr;
	}
	if(m_pPrev != nullptr && m_pNext != nullptr)
	{
		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;
	}
	
	
	if (this != nullptr)
	{
		// 残りが一つしか無かったら
		if (m_pTop[nPriority] == this)
		{
			m_pTop[nPriority] = nullptr;
		}

		// 残りが一つしか無かったら
		if (m_pCur[nPriority] == this)
		{
			m_pCur[nPriority] = nullptr;
		}

		delete this;

		m_pNext = nullptr;
		m_pPrev = nullptr;

		m_nNumAll[nPriority]--;
	}	
}
