//===================================================
//
// キャラクターマネージャー [CharacterManager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "CharacterManager.h"
#include"character3D.h"

using namespace std; // 名前空間stdの使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
unique_ptr<CCharacterManager> CCharacterManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// コンストラクタ
//===================================================
CCharacterManager::CCharacterManager()
{
}

//===================================================
// デストラクタ
//===================================================
CCharacterManager::~CCharacterManager()
{
}

//===================================================
// 生成処理
//===================================================
void CCharacterManager::Create(void)
{
	if (m_pInstance == nullptr)
	{
		// 自分の生成
		m_pInstance.reset(new CCharacterManager);
	}
}

//===================================================
// キャラクターの追加
//===================================================
void CCharacterManager::AddCharacter(CCharacter3D* pCharacter)
{
	// 要素を調べる
	for (auto itr = m_apCharacterList.begin(); itr != m_apCharacterList.end(); ++itr)
	{
		// nullだったらreturn
		if (pCharacter == nullptr || (*itr) == nullptr) continue;

		// 種類が同じだったら
		if (pCharacter->GetType() == (*itr)->GetType())
		{
			// 関数を抜ける
			return;
		}
	}

	// キャラクターの追加
	m_apCharacterList.push_back(pCharacter);
}

//===================================================
// キャラクターの取得
//===================================================
void CCharacterManager::GetCharacter(CCharacter3D* pCharacter)
{
	// 要素を調べる
	for (auto itr = m_apCharacterList.begin(); itr != m_apCharacterList.end(); ++itr)
	{
		// nullだったらreturn
		if (pCharacter == nullptr || (*itr) == nullptr) continue;

		// 種類が同じだったら
		if (pCharacter->GetType() == (*itr)->GetType())
		{
			// 情報のコピー
			(*itr)->Copy(pCharacter);
		}
	}
}
