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

//***************************************************
// 敵の状態クラスの定義(基底クラス)
//***************************************************
class CEnemyState : public CStateBase
{
public:
	// IDの種類
	enum ID
	{
		ID_BASE = 0,
		ID_IDLE,
		ID_MOVE,
		ID_BACKSTEP,
		ID_LANDING,
		ID_SMASH,
		ID_IMPACT,
		ID_DAMAGEL,
		ID_ROAR,
		ID_DASH,
		ID_SPIN,
		ID_HIT,
		ID_DAMAGES,
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
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(ATTACK)クラスの定義
//***************************************************
class CEnemyAttackSmash : public CEnemyState
{
public:
	CEnemyAttackSmash();
	~CEnemyAttackSmash();
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(ATTACKImpact)クラスの定義
//***************************************************
class CEnemyAttackImpact : public CEnemyState
{
public:
	CEnemyAttackImpact();
	~CEnemyAttackImpact();
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(DAMAGELarge)クラスの定義
//***************************************************
class CEnemyDamageL : public CEnemyState
{
public:
	CEnemyDamageL(const bool bBackStatp = false);
	~CEnemyDamageL();
	void Init(void) override;
	void Update(void) override;
private:
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
	~CEnemyDamageS();
	void Init(void) override;
	void Update(void) override;
private:
};

#endif
