
//===================================================
//
// キャラクター [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _CHARACTERMANAGER_H_
#define _CHARACTERMANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include<memory>
#include<vector>
#include <string>

//***************************************************
// 前方宣言
//***************************************************
class CCharacter3D;

//***************************************************
// キャラクターのマネージャークラスの定義
//***************************************************
class CCharacterManager
{
public:
	~CCharacterManager();
	static void Create(void);
	static CCharacterManager* GetInstance(void) { return m_pInstance.get(); }
	void AddCharacter(CCharacter3D* pCharacter);
	void GetCharacter(CCharacter3D* pCharacter);
private:
	CCharacterManager();
	static std::unique_ptr<CCharacterManager> m_pInstance;  // 自分のインスタンス
	std::vector<CCharacter3D*> m_apCharacterList;			// キャラクターのリスト
};

#endif