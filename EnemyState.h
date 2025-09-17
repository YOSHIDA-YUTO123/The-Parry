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
class CPlayer;

//***************************************************
// 敵の状態クラスの定義(基底クラス)
//***************************************************
class CEnemyState : public CStateBase
{
public:
	// IDの種類
	typedef enum
	{
		ID_BASE = 0,	 // 無し
		ID_IDLE,		 // なにもしない状態
		ID_MOVE,		 // 移動
		ID_BACKSTEP,	 // バックステップ
		ID_LANDING,		 // 着地
		ID_SMASH,		 // 振り下ろし攻撃
		ID_IMPACT,		 // 衝撃波攻撃
		ID_DAMAGEL,		 // 大ダメージ
		ID_ROAR,		 // 叫び
		ID_DASH,		 // 走り
		ID_SPIN,		 // 回転攻撃
		ID_HIT,			 // カウンターがヒットした時
		ID_DAMAGES,		 // 小ダメージ
		ID_GUARD,		 // ガード
		ID_STEP,		 // ステップ
		ID_SWING,		 // スイング攻撃
		ID_JUMPATTACK,	 // ジャンプ攻撃
		ID_DEATH,		 // 死亡
		ID_DOWN,		 // ダウン(死亡)
		ID_AWAY,		 // 距離を取る
		ID_SUPER_HIT,	 // 特大ヒット
		ID_COMBO_DAMAGE, // 連続ダメージ
		ID_RIGHT_MOVE,	 // 横歩き(右)
		ID_LEFT_MOVE,	 // 横歩き(左)
		ID_RUSH,		 // 突進攻撃
		ID_ENDRUSH,		 // 突進攻撃終了
		ID_BACKKICK,	 // 後ろ蹴り
		ID_LOOK_BACKL,	 // 後ろを見る(左)
		ID_LOOK_BACKR,	 // 後ろを見る(右)
		ID_SWEEP_RIGHT,	 // 薙ぎ払い(右)
		ID_SWEEP_LEFT,	 // 薙ぎ払い(左)
		ID_MAX
	}ID;

	CEnemyState();
	CEnemyState(ID Id);
	virtual ~CEnemyState();
	virtual void Init(void) {};
	virtual void Update(void) {};
	virtual void Uninit(void) {};
	virtual int GetID(void) const { return m_ID; }
	void SetOwner(CEnemy* pEnemy) { m_pEnemy = pEnemy; }
protected:
	CEnemy* GetEnemy(void) { return m_pEnemy; }
private:
	CEnemy* m_pEnemy; // 敵クラスへのポインタ
	ID m_ID; // IDの取得
};

//***************************************************
// 敵の状態のマネージャークラスの定義
//***************************************************
class CEnemyStateManager
{
public:
	~CEnemyStateManager();
	static CEnemyStateManager* Create(void);
	void SetOnwer(CEnemy* pEnemy) { m_pEnemy = pEnemy; }

	// プレイヤーーの位置を基準としたモーションの設定
	bool SetMotionByPlayerPosition(void);

	// 後ろを見るモーションの設定
	bool SetLookBackMotion(void);
private:
	CEnemyStateManager();
	CEnemy* m_pEnemy;		// 敵のインスタンス
};

//***************************************************
// 敵の状態(IDLE)クラスの定義
//***************************************************
class CEnemyIdle : public CEnemyState
{
public:
	CEnemyIdle();
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
	static constexpr float SIDE_MOVE_DISTANCE = 1000.0f;	// 横歩きしてくる距離
	static constexpr float ACTION_DISTANCE = 250.0f;		// 攻撃してくる距離

	void ElectinMove(CEnemy* pEnemy); // 移動のランダム選出
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
	CEnemyDamageL();
	CEnemyDamageL(const int nDamage,const bool bBackStatp = false);
	~CEnemyDamageL();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int IDLE_TIME = 30; // 何もしない状態の時間
	int m_nDamage;		// ダメージ量
	int m_nIdleTime;	// 何もしない状態の時間
	bool m_bFinish;		// モーションが終わったかどうか
	bool m_bBackStap;	// モーション終わりにバックステップするか判定
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
	CEnemySpin();
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
	CEnemyDamageS();
	CEnemyDamageS(const int nDamage);
	~CEnemyDamageS();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int IDLE_TIME = 30; // 何もしない時間

	int m_nIdleTime;	// 何もしない時間
	int m_nDamage;		// ダメージ量
	int m_nNextAction;	// 次の行動
	bool m_bFinish;		// モーションが終わったかどうか
};

//***************************************************
// 敵の状態(Guard)クラスの定義
//***************************************************
class CEnemyGuard : public CEnemyState
{
public:
	CEnemyGuard();
	CEnemyGuard(const D3DXVECTOR3 ImpactPos, const int nDamage);
	~CEnemyGuard();
	void Init(void) override;
	void Update(void) override;
private:
	D3DXVECTOR3 m_ImpactPos; // インパクトの位置
	int m_nDamage;			 // ダメージ量
	int m_nNextAction;		 // 次の行動
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
	void CollisionPlayer(CPlayer *pPlayer, CMotion* pMotion);
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

//***************************************************
// 敵の状態(SuperHit)クラスの定義
//***************************************************
class CEnemySuperHit : public CEnemyState
{
public:
	CEnemySuperHit();
	~CEnemySuperHit();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(Consecutive_Damage)クラスの定義
//***************************************************
class CEnemyComboDamage : public CEnemyState
{
public:
	CEnemyComboDamage();
	~CEnemyComboDamage();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(RIghtMove)クラスの定義
//***************************************************
class CEnemyRightMove : public CEnemyState
{
public:
	CEnemyRightMove();
	~CEnemyRightMove();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int MOVE_TIME = 120; // 最低移動時間
	static constexpr float OBSTACLE_DISTANCE = 250.0f;	 // 障害物との距離
	static constexpr float ACTION_DISTANCE = 750.0f; // 攻撃してくる距離
	int m_nTime; // 横移動の時間
};

//***************************************************
// 敵の状態(LeftMove)クラスの定義
//***************************************************
class CEnemyLeftMove : public CEnemyState
{
public:
	CEnemyLeftMove();
	~CEnemyLeftMove();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int MOVE_TIME = 120; // 最低移動時間
	static constexpr float ACTION_DISTANCE = 750.0f; // 攻撃してくる距離
	static constexpr float OBSTACLE_DISTANCE = 250.0f;	 // 障害物との距離
	int m_nTime; // 横移動の時間
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
	static constexpr int MAX_TIME = 360; // 最大の時間
	int m_nEndTime;						 // 終了する時間
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
};

//***************************************************
// 敵の状態(LookBackL)クラスの定義
//***************************************************
class CEnemyLookBackL : public CEnemyState
{
public:
	CEnemyLookBackL();
	~CEnemyLookBackL();
	void Init(void) override;
	void Update(void) override;
private:
	float m_fAngle;		// 現在の向き
	float m_fDiffAngle; // 目的の向きまでの距離
	int m_nFrame;		// フレーム
	int m_nCounter;		// カウンター
};

//***************************************************
// 敵の状態(LookBackR)クラスの定義
//***************************************************
class CEnemyLookBackR : public CEnemyState
{
public:
	CEnemyLookBackR();
	~CEnemyLookBackR();
	void Init(void) override;
	void Update(void) override;
private:
	float m_fAngle;		// 現在の向き
	float m_fDiffAngle; // 目的の向きまでの距離
	int m_nFrame;		// フレーム
	int m_nCounter;		// カウンター
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

#endif
