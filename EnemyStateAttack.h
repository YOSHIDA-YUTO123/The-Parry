//===================================================
//
// 敵の攻撃状態の処理 [enemyStateAttack.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ENEMYSTATEATTACK_H_
#define _ENEMYSTATEATTACK_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "EnemyState.h"

//***************************************************
// 敵の状態(ATTACK)クラスの定義
//***************************************************
class CEnemyAttackSmash : public CEnemyState
{
public:
	CEnemyAttackSmash();
	~CEnemyAttackSmash();
	void Init(void) override;
	void Update(void) override;
private:

	int m_nNextAction; // 次の行動
};

//***************************************************
// 敵の状態(ATTACKImpact)クラスの定義
//***************************************************
class CEnemyAttackImpact : public CEnemyState
{
public:
	CEnemyAttackImpact();
	~CEnemyAttackImpact();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(SPIN)クラスの定義
//***************************************************
class CEnemySpin : public CEnemyState
{
public:
	CEnemySpin();
	CEnemySpin(const int nTime);
	~CEnemySpin();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr float ATTACK_DISTANCE = 300.0f; // 攻撃に派生する距離
	static constexpr int PROB_ATTACK = 40;				 // 攻撃に派生する確率
	int m_nTime;			// 回転する時間
	int m_nMaxTime;			// 最大の時間
	int m_ProbAttack;		// 別の攻撃に派生する確率
};

//***************************************************
// 敵の状態(Swing)クラスの定義
//***************************************************
class CEnemySwing : public CEnemyState
{
public:
	CEnemySwing();
	~CEnemySwing();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int PROB_ACTION = 30; // 攻撃に派生する確率

	int m_nProbAction;  //  攻撃に派生する確率
	int m_nNextAction; // 次の行動
};

//***************************************************
// 敵の状態(JumpAttack)クラスの定義
//***************************************************
class CEnemyJumpAttack : public CEnemyState
{
public:
	CEnemyJumpAttack();
	~CEnemyJumpAttack();
	void Init(void) override;
	void Update(void) override;
	void CollisionPlayer(CPlayer* pPlayer, CMotion* pMotion);
private:
};

//***************************************************
// 敵の状態(Rush)クラスの定義
//***************************************************
class CEnemyRush : public CEnemyState
{
public:
	CEnemyRush();
	~CEnemyRush();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int MAX_TIME = 360;			// 最大の時間
	static constexpr int PROB_SMASH = 40;			// 振り下ろし攻撃に派生する確率 
	static constexpr float SMASH_DISTANCE = 300.0f;	// 振り下ろし攻撃に派生する距離

	int m_nProbSmash;	// 振り下ろしに派生するか
	int m_nEndTime;		// 終了する時間
};

//***************************************************
// 敵の状態(EndRush)クラスの定義
//***************************************************
class CEnemyEndRush : public CEnemyState
{
public:
	CEnemyEndRush();
	CEnemyEndRush(const float fInertia);
	~CEnemyEndRush();
	void Init(void) override;
	void Update(void) override;
private:
	float m_fInertia; // 慣性
};

//***************************************************
// 敵の状態(BackKick)クラスの定義
//***************************************************
class CEnemyBackKick : public CEnemyState
{
public:
	CEnemyBackKick();
	~CEnemyBackKick();
	void Init(void) override;
	void Update(void) override;
private:
	bool m_bHit; // プレイヤーに攻撃があたったかどうか
};

//***************************************************
// 敵の状態(SweepRight)クラスの定義
//***************************************************
class CEnemySweepRight : public CEnemyState
{
public:
	CEnemySweepRight();
	~CEnemySweepRight();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(SweepLeft)クラスの定義
//***************************************************
class CEnemySweepLeft : public CEnemyState
{
public:
	CEnemySweepLeft();
	~CEnemySweepLeft();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(RushSwing)クラスの定義
//***************************************************
class CEnemyRushSwing : public CEnemyState
{
public:

	// 攻撃モーションの種類
	typedef enum
	{
		ACTION_SMASH = 0,
		ACTION_SWING,
		ACTION_MAX
	}ACTION;

	CEnemyRushSwing();
	~CEnemyRushSwing();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr float MOVE_FRAME = 15.0f; // 移動フレーム
	static constexpr int PROB_ACTION = 40; // 次の攻撃に派生する確率

	void CollisionPlayer(CEnemy* pEnemy, CMotion* pMotion);
};

//***************************************************
// 敵の状態(Counter)クラスの定義
//***************************************************
class CEnemyCounter : public CEnemyState
{
public:
	CEnemyCounter();
	~CEnemyCounter();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(Revenge_impact)クラスの定義
//***************************************************
class CEnemyRevengeImpact : public CEnemyState
{
public:
	CEnemyRevengeImpact();
	~CEnemyRevengeImpact();
	void Init(void) override;
	void Update(void) override;
private:
};

#endif
