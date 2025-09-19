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
class CCharacter3D;

//***************************************************
// プレイヤーの状態クラスの定義
//***************************************************
class CPlayerState : public CStateBase
{
public:

	// IDの種類
	enum ID
	{
		ID_BASE = 0,
		ID_NORMAL,
		ID_MOVE,
		ID_JUMP,
		ID_LANDING,
		ID_DAMAGE,
		ID_DOWNNEUTRAL,
		ID_AVOID,
		ID_ROUNDKICK,
		ID_DASH,
		ID_REVENGE,
		ID_REVENGEATTACK,
		ID_DAMAGE_BACK,
		ID_DOWN_NEUTRAL_BACK,
		ID_JUMP_ATTACK,
		ID_MAX
	};

	CPlayerState();
	CPlayerState(ID Id);
	virtual ~CPlayerState();
	virtual void Init(void) override {};
	virtual void Update(void) override {};
	virtual void Uninit(void) override {};
	virtual int GetID(void) const { return m_ID; }

	void SetOwner(CPlayer* pPlayer);
protected:
	CPlayer* GetPlayer(void) { return m_pPlayer; }
private:
	CPlayer* m_pPlayer;			// プレイヤーのクラスへのポインタ
	ID m_ID; // ID
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

	// 音の種類
	typedef enum
	{
		TYPE_NORMAL = 0,
		TYPE_SPIKE,
		TYPE_MAX
	}TYPE;

	CPlayerDamage();
	CPlayerDamage(int nDamage);
	~CPlayerDamage();
	void Init(void) override;
	void Update(void) override;
	void SetSound(const TYPE type) { m_type = type; }
private:
	TYPE m_type;	// 種類
	int m_nDamage;	// ダメ―ジ量
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
	CPlayerAvoid();
	CPlayerAvoid(const float fSpeed);
	~CPlayerAvoid();
	void Init(void) override;
	void Update(void) override;
private:
	float m_fSpeed; // 速さ
};

//***************************************************
// プレイヤーの状態(RoundKick)の定義
//***************************************************
class CPlayerRoundKick : public CPlayerState
{
public:
	CPlayerRoundKick();
	~CPlayerRoundKick();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(RoundKick)の定義
//***************************************************
class CPlayerDash : public CPlayerState
{
public:
	CPlayerDash();
	~CPlayerDash();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Jump)の定義
//***************************************************
class CPlayerJump : public CPlayerState
{
public:
	CPlayerJump();
	~CPlayerJump();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Landing)の定義
//***************************************************
class CPlayerLanding : public CPlayerState
{
public:
	CPlayerLanding();
	~CPlayerLanding();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Revenge)の定義
//***************************************************
class CPlayerRevenge : public CPlayerState
{
public:
	CPlayerRevenge();
	~CPlayerRevenge();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(RevengeAttack)の定義
//***************************************************
class CPlayerRevengeAttack : public CPlayerState
{
public:
	CPlayerRevengeAttack();
	~CPlayerRevengeAttack();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(Damage_Back)の定義
//***************************************************
class CPlayerDamageBack : public CPlayerState
{
public:
	CPlayerDamageBack();
	CPlayerDamageBack(const int nDamage);
	~CPlayerDamageBack();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nDamage; // ダメージ
};

//***************************************************
// プレイヤーの状態(DownNeutralBk)の定義
//***************************************************
class CPlayerDownNeutralBk : public CPlayerState
{
public:
	CPlayerDownNeutralBk();
	~CPlayerDownNeutralBk();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// プレイヤーの状態(JumpAttack)の定義
//***************************************************
class CPlayerJumpAttack : public CPlayerState
{
public:
	CPlayerJumpAttack();
	~CPlayerJumpAttack();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr float JUMP_MOVE_FRAME = 20.0f; // ジャンプ攻撃の移動フレーム
	static constexpr float JUMP_VALUE = 20.0f;		// ジャンプ量
};
#endif