//===================================================
//
// キャラクター [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _CHARACTER3D_H_
#define _CHARACTER3D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"objectX.h"
#include"transform.h"

//***************************************************
// キャラクター3Dクラスの定義
//***************************************************
class CCharacter3D : public CObject
{
public:

	// 状態の種類
	typedef enum
	{
		STATE_NORMAL = 0,
		STATE_MOVE,
		STATE_ACTION,
		STATE_DAMAGE,
		STATE_MAX
	}STATE;

	CCharacter3D(int nPriority = 2);
	~CCharacter3D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CPosition* GetPosition(void) const { return m_pPos; }
	CRotation* GetRotation(void) const { return m_pRot; }

	void SetCharacter(const int nLife) { m_nLife = nLife; }
	bool HitCharacter(int nDamage);

	void SetState(const STATE state,const int nTime);
	STATE GetState(void) { return m_state; }
private:
	CPosition *m_pPos;		// 位置クラスへのポインタ
	CRotation *m_pRot;		// 向きクラスへのポインタ
	STATE m_state;			// 状態
	int m_nCounterState;	// 状態のカウンター

	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	int m_nLife;			// 寿命
};

#endif