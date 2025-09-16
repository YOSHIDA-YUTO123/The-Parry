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
#include "pause.h"

using namespace Const; // 名前空間Constを使用

//***************************************************
// 静的メンバ変数の宣言
//***************************************************
CObject* CObject::m_pTop[NUM_PRIORITY] = { nullptr }; // 先頭オブジェクトへのポインタ
CObject* CObject::m_pCur[NUM_PRIORITY] = { nullptr }; // 最後尾オブジェクトへのポインタ
int CObject::m_nNumAll[NUM_PRIORITY] = { NULL };	  // オブジェクトの総数

//===================================================
// コンストラクタ(オーバーロード)
//===================================================
CObject::CObject(int nPriority)
{
	m_type = TYPE_NONE;
	m_nPriority = nPriority;
	m_bDeath = false;

	m_pPrev = nullptr;
	m_pNext = nullptr;

	// 先頭がnullだったら
	if (m_pTop[nPriority] == nullptr)
	{
		// 先頭を設定
		m_pTop[nPriority] = this;
		m_pCur[nPriority] = this;
	}
	else
	{
		// 既存リストの最後尾に追加
		m_pPrev = m_pCur[nPriority];
		m_pCur[nPriority]->m_pNext = this;
		m_pCur[nPriority] = this;
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

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// 先頭オブジェクトを代入
		CObject* pObject = m_pTop[nCntPriority];

		// nullじゃないなら
		while (pObject != nullptr)
		{
			// 次のオブジェクトのポインタを代入
			CObject* pObjectNext = pObject->m_pNext;

			// 死亡フラグがたっていたら
			if (pObject->m_bDeath == true)
			{
				// オブジェクトの破棄
				pObject->Destroy(pObject);
			}
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

			// ポーズ状態の取得
			bool bPause = CPauseManager::GetPause();

			// ポーズ中だったらポーズ以外のオブジェクトの処理をしない
			if (pObject->GetType() != TYPE_PAUSE && bPause == true)
			{
				// 次のオブジェクトを代入
				pObject = pObjectNext;

				continue;
			}

			// 更新処理
			pObject->Update();

			// 次のオブジェクトを代入
			pObject = pObjectNext;
		}
	}

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		// 先頭オブジェクトを代入
		CObject* pObject = m_pTop[nCntPriority];

		// nullじゃないなら
		while (pObject != nullptr)
		{
			// 次のオブジェクトのポインタを代入
			CObject* pObjectNext = pObject->m_pNext;

			// 死亡フラグがたっていたら
			if (pObject->m_bDeath == true)
			{
				// オブジェクトの破棄
				pObject->Destroy(pObject);
			}
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
	// 優先順位の数分だけ調べる
	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		CObject* pObject = m_pTop[nCntPriority]; // 先頭オブジェクトを代入

		// nullじゃないなら
		while (pObject != nullptr)
		{
			CObject* pObjectNext = pObject->m_pNext; // 次のオブジェクトのポインタを代入

			// ポーズ状態の取得
			bool bPause = CPauseManager::GetPause();

			// オブジェクトがポーズでポーズ中じゃないならポーズを描画しない
			if (pObject->GetType() == TYPE_PAUSE && bPause == false)
			{
				pObject = pObjectNext; // 次のオブジェクトを代入

				// 処理を飛ばす
				continue;
			}

			// 更新処理
			pObject->Draw();

			pObject = pObjectNext; // 次のオブジェクトを代入
		}
	}
}

//===================================================
// 死亡フラグがたっているすべてのオブジェクトの破棄
//===================================================
void CObject::DestroyAll(void)
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

			// 死亡フラグがたっていたら
			if (pObject->m_bDeath == true)
			{
				// オブジェクトの破棄
				pObject->Destroy(pObject);
			}
			// 次のオブジェクトを代入
			pObject = pObjectNext;
		}
	}
}

//===================================================
//死亡フラグの設定
//===================================================
void CObject::Release(void)
{
	// 死亡フラグをオンにする
	m_bDeath = true;
}

//===================================================
// オブジェクトの破棄
//===================================================
void CObject::Destroy(CObject *pObject)
{
	// 自分の優先順位
	int nPriority = m_nPriority;

	// 先頭だったら
	if (pObject == m_pTop[nPriority] && m_pNext != nullptr)
	{
		// 先頭を次のオブジェクトにする
		m_pTop[nPriority] = m_pNext;
		m_pNext->m_pPrev = nullptr;
	}
	// 最後尾だったら
	if (pObject == m_pCur[nPriority] && m_pPrev != nullptr)
	{
		// 最後尾をひとつ前のオブジェクトにする
		m_pCur[nPriority] = m_pPrev;
		m_pPrev->m_pNext = nullptr;
	}
	// 最後尾でも先頭でも無かったら
	if (m_pPrev != nullptr && m_pNext != nullptr)
	{
		// 間をつなげる
		m_pPrev->m_pNext = m_pNext;
		m_pNext->m_pPrev = m_pPrev;
	}

	// 残りが一つしか無かったら
	if (m_pTop[nPriority] == pObject)
	{
		m_pTop[nPriority] = nullptr;
	}

	// 残りが一つしか無かったら
	if (m_pCur[nPriority] == pObject)
	{
		m_pCur[nPriority] = nullptr;
	}

	m_pNext = nullptr;
	m_pPrev = nullptr;

	// オブジェクトの数を減らす
	m_nNumAll[nPriority]--;

	delete pObject;

	pObject = nullptr;
}
