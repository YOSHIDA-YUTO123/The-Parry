//===================================================
//
// 敵の状態 [enemyState.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ENEMYSTATE_H_
#define _ENEMYSTATE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include<memory>
#include"statebase.h"

//***************************************************
// 前方宣言
//***************************************************
class CEnemy;
class CMotion;
class CPlayerGame;

//***************************************************
// 敵の状態クラスの定義(基底クラス)
//***************************************************
class CEnemyState : public CStateBase
{
public:
	// IDの種類
	enum ID
	{
		ID_BASE = 0,	// 無し
		ID_IDLE,		// なにもしない状態
		ID_MOVE,		// 移動
		ID_BACKSTEP,	// バックステップ
		ID_LANDING,		// 着地
		ID_SMASH,		// 振り下ろし攻撃
		ID_IMPACT,		// 衝撃波攻撃
		ID_DAMAGEL,		// 大ダメージ
		ID_ROAR,		// 叫び
		ID_DASH,		// 走り
		ID_SPIN,		// 回転攻撃
		ID_HIT,			// カウンターがヒットした時
		ID_DAMAGES,		// 小ダメージ
		ID_GUARD,		// ガード
		ID_STEP,		// ステップ
		ID_SWING,		// スイング攻撃
		ID_JUMPATTACK,	// ジャンプ攻撃
		ID_DEATH,		// 死亡
		ID_DOWN,		// ダウン(死亡)
		ID_AWAY,		// 距離を取る
		ID_MAX
	};

	CEnemyState(ID Id);
	virtual ~CEnemyState();
	virtual void Init(void) {};
	virtual void Update(void) {};
	virtual void Uninit(void) {};
	virtual int GetID(void) const { return m_ID; }
	void SetOwner(CEnemy* pEnemy) { m_pEnemy = pEnemy; }
protected:
	CEnemy* m_pEnemy; // 敵クラスへのポインタ
private:
	ID m_ID; // IDの取得
};

//***************************************************
// 敵の状態(IDLE)クラスの定義
//***************************************************
class CEnemyIdle : public CEnemyState
{
public:
	CEnemyIdle(int nNextCount);
	~CEnemyIdle();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nNextStateCount; // 次のステートに移るカウンター
};

//***************************************************
// 敵の状態(Move)クラスの定義
//***************************************************
class CEnemyMove : public CEnemyState
{
public:
	CEnemyMove();
	~CEnemyMove();
	void Update(void) override;
private:
	int m_nNextStateCount;  // 次のステートに移るカウンター
};

//***************************************************
// 敵の状態(BackStep)クラスの定義
//***************************************************
class CEnemyBackStep : public CEnemyState
{
public:
	CEnemyBackStep();
	~CEnemyBackStep();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(Landing)クラスの定義
//***************************************************
class CEnemyLanding : public CEnemyState
{
public:
	CEnemyLanding();
	~CEnemyLanding();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nNextAction; // 次の行動
};

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
// 敵の状態(DAMAGELarge)クラスの定義
//***************************************************
class CEnemyDamageL : public CEnemyState
{
public:
	CEnemyDamageL(const int nDamage,const bool bBackStatp = false);
	~CEnemyDamageL();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nDamage;	  // ダメージ量
	bool m_bBackStap; // モーション終わりにバックステップするか判定
};

//***************************************************
// 敵の状態(ROAR)クラスの定義
//***************************************************
class CEnemyRoar : public CEnemyState
{
public:
	CEnemyRoar();
	~CEnemyRoar();
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(DASH)クラスの定義
//***************************************************
class CEnemyDash : public CEnemyState
{
public:
	CEnemyDash();
	~CEnemyDash();
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(SPIN)クラスの定義
//***************************************************
class CEnemySpin : public CEnemyState
{
public:
	CEnemySpin(const int nTime);
	~CEnemySpin();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nTime;	// 回転する時間
	int m_nMaxTime;	// 最大の時間
};

//***************************************************
// 敵の状態(HIT)クラスの定義
//***************************************************
class CEnemyHit : public CEnemyState
{
public:
	CEnemyHit();
	~CEnemyHit();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(DAMAGESmall)クラスの定義
//***************************************************
class CEnemyDamageS : public CEnemyState
{
public:
	CEnemyDamageS(const int nDamage);
	~CEnemyDamageS();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nDamage; // ダメージ量
	int m_nNextAction; // 次の行動
};

//***************************************************
// 敵の状態(Guard)クラスの定義
//***************************************************
class CEnemyGuard : public CEnemyState
{
public:
	CEnemyGuard(const D3DXVECTOR3 ImpactPos, const int nDamage);
	~CEnemyGuard();
	void Init(void) override;
	void Update(void) override;
private:
	D3DXVECTOR3 m_ImpactPos; // インパクトの位置
	int m_nDamage; // ダメージ量
	int m_nNextAction; // 次の行動
};

//***************************************************
// 敵の状態(Step)クラスの定義
//***************************************************
class CEnemyStep : public CEnemyState
{
public:
	CEnemyStep();
	~CEnemyStep();
	void Init(void) override;
	void Update(void) override;
private:
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
	void CollisionPlayer(CPlayerGame *pPlayer, CMotion* pMotion);
private:
};

//***************************************************
// 敵の状態(Death)クラスの定義
//***************************************************
class CEnemyDeath : public CEnemyState
{
public:
	CEnemyDeath();
	~CEnemyDeath();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(Down)クラスの定義
//***************************************************
class CEnemyDown : public CEnemyState
{
public:
	CEnemyDown();
	~CEnemyDown();
	void Init(void) override;
	void Update(void) override;
private:
	int m_nTrackingTime; // 追従する時間
};

//***************************************************
// 敵の状態(Away)クラスの定義
//***************************************************
class CEnemyAway : public CEnemyState
{
public:
	CEnemyAway();
	~CEnemyAway();
	void Init(void) override;
	void Update(void) override;
private:
	D3DXVECTOR3 m_pos; // 移動先
};

#endif
