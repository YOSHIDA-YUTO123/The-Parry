//===================================================
//
// スローモーション [slow.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _SLOW_H_
#define _SLOW_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// スロークラスの定義
//***************************************************
class CSlow
{
public:
	CSlow();
	~CSlow();
	void Update(void);
	void Start(const int nTime, const float fLevel);
	void End(void);
	float GetLevel(bool bAdd) { return bAdd ? m_fLevel : (1.0f / m_fLevel); }
private:
	float m_fLevel;	// スローモーションのレベル(どのくらいおそくなるか)
	int m_nTime;	// スローモーションの時間
	int m_nCount;	// スローモーションのカウンター
};
#endif
