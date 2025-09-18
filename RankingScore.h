//================================================
//
// ランキングのスコアを描画する [RankingScore.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _RANKINGSCORE_H_
#define _RANKINGSCORE_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"

//************************************************
// 前方宣言
//************************************************
class CNumber;

//************************************************
// ランキングのスコアの表示
//************************************************
class CRankingScore : public CObject
{
public:
	CRankingScore();
	~CRankingScore();

	static CRankingScore* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nScore, const bool bHilight);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	static constexpr int MAX_DIGIT = 8; // 最大の桁数

	CNumber* m_apNumber[MAX_DIGIT];		// ナンバークラスの定義
	D3DXCOLOR m_col;					// 色
	D3DXVECTOR3 m_pos;					// 位置
	D3DXVECTOR2 m_Size;					// 大きさ
	D3DXVECTOR2 m_DestSize;				// 目的の大きさ
	int m_nScore;						// スコア
	int m_nDestScore;					// 目的の値
	float m_fCounter;					// カウンター
	bool m_bHilight;					// 強調表示するかどうか
};

#endif
