//===================================================
//
// オープニングの敵 [OpeningEnemy.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OPENINGENEMY_H_
#define _OPENINGENEMY_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "character3D.h"

//***************************************************
// 前方宣言
//***************************************************
class CEnemyMovement;
class CMeshField;

//***************************************************
// オープニングの敵の定義
//***************************************************
class COpeningEnemy : public CCharacter3D
{
public:
	COpeningEnemy();
	~COpeningEnemy();

	static COpeningEnemy* Create(const D3DXVECTOR3 pos, const float fAngle);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	void SetFieldWave(CMeshField* pMeshField, const D3DXVECTOR3 pos);

	D3DXVECTOR3 m_move;		// 移動量
	int m_nNextSceneCnt;	// 次のシーンに行くまでのカウント
	bool m_bLanding;		// 着地しているかどうか
};
#endif
