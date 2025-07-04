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
class CCharacter3D
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

	CCharacter3D();
	~CCharacter3D();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool Hit(int nDamage);

	// ゲッター
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	CRotation* GetRotation(void) const { return m_pRot; }
	STATE GetState(void) { return m_state; }
	float GetSpeed(void) { return m_fSpeed; }

	// セッター
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCharacter(const int nLife, const float fSpeed);
	void SetState(const STATE state,const int nTime);
private:
	D3DXVECTOR3 m_pos;		// 位置
	CRotation *m_pRot;		// 向きクラスへのポインタ
	STATE m_state;			// 状態
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	float m_fSpeed;			// 足の速さ
	int m_nCounterState;	// 状態のカウンター
	int m_nLife;			// 寿命
};

#endif