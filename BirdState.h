//===================================================
//
// 鳥の状態 [BirdState.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BIRDSTATE_H_
#define _BIRDSTATE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "statebase.h"

//***************************************************
// 前方宣言
//***************************************************
class CBird;

//***************************************************
// 鳥の状態のクラス
//***************************************************
class CBirdState : public CStateBase
{
public:
	// IDの種類
	typedef enum
	{
		ID_BASE = 0,	 // 無し
		ID_IDLE,		 // 通常状態
		ID_FLY,			 // 飛行
		ID_FLYMOVE,		 // 飛行移動
		ID_MAX
	}ID;

	CBirdState();
	CBirdState(ID Id);
	virtual ~CBirdState();
	virtual void Init(void) {};
	virtual void Update(void) {};
	virtual void Uninit(void) {};
	virtual int GetID(void) const { return m_ID; }
	void SetOwner(CBird* pBird) { m_pBird = pBird; }

protected:
	CBird* GetBird(void) { return m_pBird; }
private:
	CBird* m_pBird; // 鳥クラスへのポインタ
	ID m_ID; // IDの取得
};

//***************************************************
// 鳥の状態クラス(通常状態)の定義
//***************************************************
class CBirdIdle : public CBirdState
{
public:
	CBirdIdle();
	~CBirdIdle();

	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 鳥の状態クラス(飛行状態)の定義
//***************************************************
class CBirdFly : public CBirdState
{
public:
	CBirdFly();
	~CBirdFly();

	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 鳥の状態クラス(飛行移動状態)の定義
//***************************************************
class CBirdFlyMove : public CBirdState
{
public:
	CBirdFlyMove();
	~CBirdFlyMove();

	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int WING_DROP_TIME = 300; // 羽を落とす時間
	int m_nTime;							   // 時間
};

#endif