//===================================================
//
// 勝利時のリザルトのマネージャー [WinResultManager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _WINRESULTMANAGER_H_
#define _WINRESULTMANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"

//***************************************************
// 勝利時のリザルトのマネージャーのクラスの定義
//***************************************************
class CWinResultManager : public CObject
{
public:

	// メニューの種類
	typedef enum
	{
		MENU_PARRY_PARFECT = 0,
		MENU_PARRY_NORMAL,
		MENU_PARRY_WEAK,
		MENU_CLEAR_TIME,
		MENU_SCORE,
		MENU_MAX
	}MENU;

	~CWinResultManager();

	static void Create(void);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	CWinResultManager();
	void SaveScore(void);

	static CWinResultManager* m_pInstance; // 自分のインスタンス
	MENU m_menu;						   // メニュー
	int m_nCounter;						   // カウンター
	int m_nPerfectCnt;					   // パーフェクトの回数
	int m_nNormalCnt;					   // 普通の回数
	int m_nWeakCnt;						   // 弱いの回数
	int m_nScore;						   // スコア
	bool m_bSkip;						   // スキップしたかどうか
};

#endif