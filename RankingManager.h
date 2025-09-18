//================================================
//
// ランキングマネージャー [RankingManager.h]
// Author: YUTO YOSHIDA
// 
// << ランキングのスコアの読み込み、ソート、>>
// << 書き出し、生成を行う					>>
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _RANKINGMANAGER_H_
#define _RANKINGMANAGER_H_

//************************************************
// インクルードファイル
//************************************************
#include "object.h"

//*************************************************
// ランキングマネージャーのクラスの定義
//*************************************************
class CRankingManager : public CObject
{
public:
	~CRankingManager();

	static void Create(void);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	CRankingManager();
	void Load(void);
	void Sort(void);
	void Save(void);
	void SetRanking(void);

	static constexpr int NUM_RANK = 5; // ランキングの順位の数
	int m_aScore[NUM_RANK];			   // ランキングのスコア
	int m_nResultScore;				   // リザルトのスコア
	int m_nUpdateScoreIdx;			   // 更新するランキングスコアインデックス
};
#endif
