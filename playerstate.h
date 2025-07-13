//===================================================
//
// プレイヤーの状態 [playerstate.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include "statebase.h"

//***************************************************
// 前方宣言
//***************************************************
class CPlayer;

//***************************************************
// プレイヤーの状態クラスの定義
//***************************************************
class CPlayerState : public CStateBase
{
public:
	CPlayerState();
	virtual ~CPlayerState();
	virtual void Init(void) override {};
	virtual void Update(void) override {};
	virtual void Uninit(void) override {};

	void SetOwner(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
protected:
	CPlayer* m_pPlayer;	// プレイヤーのクラスへのポインタ
private:
};

//***************************************************
// プレイヤーの状態(Normal)の定義
//***************************************************
class CPlayerNormal : public CPlayerState
{
public:
	CPlayerNormal();
	~CPlayerNormal();
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Move)の定義
//***************************************************
class CPlayerMove : public CPlayerState
{
public:
	CPlayerMove();
	~CPlayerMove();
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Damage)の定義
//***************************************************
class CPlayerDamage : public CPlayerState
{
public:
	CPlayerDamage();
	~CPlayerDamage();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(DownNeutral)の定義
//***************************************************
class CPlayerDownNeutral : public CPlayerState
{
public:
	CPlayerDownNeutral();
	~CPlayerDownNeutral();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Avoid)の定義
//***************************************************
class CPlayerAvoid : public CPlayerState
{
public:
	CPlayerAvoid(const float fSpeed);
	~CPlayerAvoid();
	void Init(void) override;
	void Update(void) override;
private:
	float m_fSpeed; // 速さ
};

#endif