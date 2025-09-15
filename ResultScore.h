//===================================================
//
// リザルトのスコア [ResultScore.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _RESULTSCORE_H_
#define _RESULTSCORE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"

//***************************************************
// 前方宣言
//***************************************************
class CNumber;

//***************************************************
// リザルトのスコアクラスの定義
//***************************************************
class CResultScore : public CObject
{
public:
	CResultScore();
	~CResultScore();

	static CResultScore* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nScore);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	static const int MAX_DIGIT = 9; // 最大の桁数

	CNumber* m_pNumber[MAX_DIGIT];  // ナンバークラスへのポインタ
	D3DXVECTOR3 m_pos;				// 位置
	D3DXVECTOR2 m_Size;				// 大きさ
	D3DXCOLOR m_col;				// 色
	float m_fAddAlv;				// アルファ値の加算値
	int m_nScore;					// スコア
};

#endif