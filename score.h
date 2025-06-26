//================================================
//
// スコア [score.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _SCORE_H_
#define _SCORE_H_

//*************************************************
// インクルードファイル
//*************************************************
#include"main.h"
#include"object2D.h"
#include "number.h"

//*************************************************
// マクロ定義
//*************************************************
#define MAX_DIGIT (9)			// 最大の桁数
#define MAX_SCORE (999999999)	// 最大のスコア
#define MIN_SCORE (0)			// 最小のスコア

//*************************************************
// スコアクラスの定義
//*************************************************
class CScore : public CObject
{
public:

	// スコアの種類
	typedef enum
	{
		TYPE_NORMAL = 0, // 普通
		TYPE_LERPER,	 // 補間
		TYPE_MAX
	}TYPE;

	CScore(int nPriority = NUM_PRIORITY - 1);
	~CScore();
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR2 GetSize(void) const { return D3DXVECTOR2(m_fWidth,m_fHeight); }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const float fWidth, const float fHeight);

	static CScore* Create(const TYPE type,const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void AddScore(const int value) { m_nScore += value; }
	void SetScore(const int value) { m_nScore = value; }
	int GetScore(void) const { return m_nScore; }
private:
	static CNumber* m_apNumber[MAX_DIGIT];	// ナンバーオブジェクトへのポインタ
	int m_nScore;							// スコア
	D3DXVECTOR3 m_pos;						// 位置	
	float m_fWidth, m_fHeight;				// 幅、高さ
};

//*************************************************
// スコアクラス(徐々に補間)の定義
//*************************************************
class CScoreLerper : public CScore
{
public:
	CScoreLerper(int nPriority = NUM_PRIORITY - 1);
	~CScoreLerper();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDestScore(const int value,const int nTime);
private:
	int m_nDestScore; // 目的のスコア
	int m_nTime;	  // 目的のスコアになるまでの時間
	int m_nCounter;	  // カウンター
};
#endif