//================================================
//
// タイマー [timer.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _TIMER_H_
#define _TIMER_H_

//*************************************************
// インクルードファイル
//*************************************************
#include "main.h"
#include "object2D.h"

//*************************************************
// 前方宣言
//*************************************************
class CNumber; 

//************************************************
// タイマークラスの定義
//************************************************
class CTime : public CObject
{
public:

	CTime(int nPriority = 7);
	~CTime();

	static CTime* Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const int nTime,bool bCountUp);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR2 GetSize(void) const { return D3DXVECTOR2(m_fWidth, m_fHeight); }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const float fWidth, const float fHeight);

	void SetMinute(const int nTime) { m_nMinute = nTime / Const::FRAME; }
	void SetSecond(const int nTime) { m_nSecond = nTime % Const::FRAME; }
	void SetColor(const D3DXCOLOR col) { m_col = col; }

	void UpdateCountDown(void);
	void UpdateCountUp(void);

private:

	static constexpr int NUM_DIGIT = 2;		// 桁数

	CNumber* m_apMinute[NUM_DIGIT];			// ナンバーオブジェクトへのポインタ分
	CNumber* m_apSecond[NUM_DIGIT];			// ナンバーオブジェクトへのポインタ秒

	D3DXVECTOR3 m_pos;						// 位置
	D3DXCOLOR m_col;						// 色
	float m_fWidth, m_fHeight;				// 幅、高さ

	int m_nTime;							// 時間
	int m_nMinute;							// 分
	int m_nSecond;							// 秒
	int m_nCounter;							// フレームカウンター
	bool m_bCountUp;						// カウントアップ
};
#endif