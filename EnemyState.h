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
		ID_BASE = 0,		// 無し
		ID_IDLE,			// なにもしない状態
		ID_MOVE,			// 移動
		ID_BACKSTEP,		// バックステップ
		ID_LANDING,			// 着地
		ID_SMASH,			// 振り下ろし攻撃
		ID_IMPACT,			// 衝撃波攻撃
		ID_DAMAGEL,			// 大ダメージ
		ID_ROAR,			// 叫び
		ID_DASH,			// 走り
		ID_SPIN,			// 回転攻撃
		ID_HIT,				// カウンターがヒットした時
		ID_DAMAGES,			// 小ダメージ
		ID_GUARD,			// ガード
		ID_STEP,			// ステップ
		ID_SWING,			// スイング攻撃
		ID_JUMPATTACK,		// ジャンプ攻撃
		ID_DEATH,			// 死亡
		ID_DOWN,			// ダウン(死亡)
		ID_AWAY,			// 距離を取る
		ID_SUPER_HIT,		// 特大ヒット
		ID_COMBO_DAMAGE,	// 連続ダメージ
		ID_RIGHT_MOVE,		// 横歩き(右)
		ID_LEFT_MOVE,		// 横歩き(左)
		ID_RUSH,			// 突進攻撃
		ID_ENDRUSH,			// 突進攻撃終了
		ID_BACKKICK,		// 後ろ蹴り
		ID_LOOK_BACKL,		// 後ろを見る(左)
		ID_LOOK_BACKR,		// 後ろを見る(右)
		ID_SWEEP_RIGHT,		// 薙ぎ払い(右)
		ID_SWEEP_LEFT,		// 薙ぎ払い(左)
		ID_RUSH_SWING,		// 突進なぎ
		ID_BRISK_MOVE,		// 早歩き
		ID_COUNTER,			// カウンター
		ID_REVENGE_IMPACT,  // 衝撃波の跳ね返し
		ID_EVENT_ROAR,		// HPが半分になったときの演出
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
	int GetLife(void) const { return m_nLife; }
	void SetLife(const int nLife) { m_nLife = nLife; }
	void SetMaxLife(const int nLife) { m_nMaxLife = nLife; }
	void SetLowEvent(void);
	bool CheckLowHp(const int nRate);
	bool GetEvent(void) const { return m_bEvent; }
	void EndEvent(void);
private:
	CEnemyStateManager();
	CEnemy* m_pEnemy;		// 敵のインスタンス
	int m_nMaxLife;			// 最大の体力
	int m_nLife;			// 敵の体力
	bool m_bLowEventFinish;	// HPが少ないときの演出が終わったかどうか
	bool m_bEvent;			// イベント中かどうか
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
// 敵の状態(DAMAGELarge)クラスの定義
//***************************************************
class CEnemyDamageL : public CEnemyState
{
public:
	CEnemyDamageL();
	CEnemyDamageL(const int nDamage, const bool bBackStatp = false);
	~CEnemyDamageL();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int IDLE_TIME = 10; // 何もしない状態の時間
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
// 敵の状態(HIT)クラスの定義
//***************************************************
class CEnemyHit : public CEnemyState
{
public:

	// モーションの種類
	typedef enum
	{
		TYPE_NORMAL = 0,
		TYPE_BACK,
		TYPE_MAX
	}TYPE;

	CEnemyHit();
	~CEnemyHit();

	void Init(void) override;
	void Update(void) override;
	void SetType(const TYPE type) { m_type = type; }
private:

	// 定数
	struct Const
	{
		const D3DXVECTOR2 BACK_MOVE_VALUE = { 0.0f, 5.0f }; // 後ろに進む量
		static constexpr float FORWARD_MOVE_VALUE = 5.0f;	// 前に進む量
		static constexpr int PROB_ACTION = 60;				// 攻撃に派生する確率
	};
	
	Const m_Const; // 定数
	TYPE m_type;   // 種類
};

//***************************************************
// 敵の状態(DAMAGESmall)クラスの定義
//***************************************************
class CEnemyDamageS : public CEnemyState
{
public:

	// モーションの種類
	typedef enum
	{
		TYPE_NORMAL = 0,
		TYPE_STANP,
		TYPE_MAX
	}TYPE;

	CEnemyDamageS();
	CEnemyDamageS(const int nDamage);
	~CEnemyDamageS();
	void Init(void) override;
	void Update(void) override;
	void SetType(const TYPE type) { m_type = type; }
private:
	static constexpr int IDLE_TIME = 20; // 何もしない時間
	static constexpr int PROB_ACTION = 40; // 次の行動に派生する確率

	TYPE m_type;		// モーションの種類
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

	// 攻撃モーションの種類
	typedef enum
	{
		ACTION_SMASH = 0,
		ACTION_SWING,
		ACTION_RUSH_SWING,
		ACTION_MAX
	}ACTION;

	CEnemyGuard();
	CEnemyGuard(const D3DXVECTOR3 ImpactPos, const int nDamage);
	~CEnemyGuard();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr int PROB_AWAY = 10; // 距離を取る確率

	D3DXVECTOR3 m_ImpactPos; // インパクトの位置
	int m_nDamage;			 // ダメージ量
};

//***************************************************
// 敵の状態(Death)クラスの定義
//***************************************************
class CEnemyDeath : public CEnemyState
{
public:

	// モーションの種類
	typedef enum
	{
		TYPE_NORMAL = 0,
		TYPE_STANP,
		TYPE_MAX
	}TYPE;

	CEnemyDeath();
	~CEnemyDeath();
	void Init(void) override;
	void Update(void) override;
	void SetType(const TYPE type) { m_type = type; }
private:
	TYPE m_type; // モーションの種類 
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
// 敵の状態(EventRoar)クラスの定義
//***************************************************
class CEnemyEventRoar : public CEnemyState
{
public:
	CEnemyEventRoar();
	~CEnemyEventRoar();
	void Init(void) override;
	void Update(void) override;
private:
	static constexpr float COLLISION_DISTANCE = 1000.0f; // 当たる距離

	void CollisionPlayer(CEnemy* pEnemy);
};
#endif
