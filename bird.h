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
private:
	static bool m_bLoad; // ロードしたかどうか
	std::unique_ptr<CStateMachine> m_pMachine;  // 状態マシーン
};
#endif