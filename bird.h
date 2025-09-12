//===================================================
//
// 鳥 [bird.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BIRD_H_
#define _BIRD_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "character3D.h"

//***************************************************
// 前方宣言
//***************************************************
class CStateMachine;
class CBirdState;
class CMotion;
class CVelocity;

//***************************************************
// 鳥クラスの定義
//***************************************************
class CBird : public CCharacter3D
{
public:

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL000 = 0,
		MOTIONTYPE_NEUTRAL001,
		MOTIONTYPE_FLY,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CBird();
	~CBird();

	static CBird* Create(const D3DXVECTOR3 pos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void ChangeState(std::shared_ptr<CBirdState> pNewState);
	bool CheckDistance(const D3DXVECTOR3 otherPos, const float fRadius);
	void SetFly(const D3DXVECTOR3 move);
private:
	void DeleteList(void);

	static bool m_bLoad; // ロードしたかどうか
	std::unique_ptr<CStateMachine> m_pMachine;  // 状態マシーン
	std::unique_ptr<CVelocity> m_pMove;			// 移動クラス
	int m_nLife;								// 寿命
};
#endif