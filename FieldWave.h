//================================================
//
// フィールドのウェーブの処理をするクラス [FieldWave.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _FIELDWAVE_H_
#define _FIELDWAVE_H_

//************************************************
// インクルードファイル
//************************************************
#include "main.h"

//************************************************
// 前方宣言
//************************************************
class CMeshField;

//************************************************
// メッシュのフィールドのウェーブクラスの定義
//************************************************
class CMeshFieldWave
{
public:

	// 波の情報(震源地、内側半径、外側半径、高さ、速さ、係数、発生時間)
	struct Config
	{
		D3DXVECTOR3 epicenter;			// 波の発生地点
		float fInRadius;				// 波の内半径
		float fOutRadius;				// 波の外半径
		float fHeight;					// 波の高さ
		float fSpeed;					// 速さ
		float fcoef;					// 係数
		int nTime;						// 波の発生時間
	};

	CMeshFieldWave();
	~CMeshFieldWave();

	static CMeshFieldWave* Create(Config config);
	void Init(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
private:

	// 波の情報(アライメント対策)
	struct ParamInfo
	{
		float fStartHeight;		// 最初の高さ
		float fTime;			// 高さ
		int nCounter;			// 波のカウンター
	};

	Config m_Confing;		// 波のパラメータ
	ParamInfo m_Info;			// 波の情報
};

#endif
