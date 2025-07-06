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

//***************************************************
// 前方宣言
//***************************************************
class CEnemy;
class CMotion;

//***************************************************
// 敵の状態クラスの定義(基底クラス)
//***************************************************
class CEnemyState
{
public:

	// 敵の状態のID
	typedef enum
	{
		TYPE_IDLE = 0,
		TYPE_MOVE,
		TYPE_SMASH,
		TYPE_IMPACT,
		TYPE_ROAR,
		TYPE_DAMAGE,
		TYPE_DASH,
		TYPE_SPIN,
		TYPE_MAX
	}TYPE;

	CEnemyState(TYPE id);
	virtual ~CEnemyState();
	virtual void Update(CEnemy *pEnemy) = 0;
	TYPE GetID(void) const { return m_ID; }
private:
	TYPE m_ID; // 状態のID
};

//***************************************************
// 敵の状態(IDLE)クラスの定義
//***************************************************
class CStateIdle : public CEnemyState
{
public:
	CStateIdle(int nNextCount);
	~CStateIdle();
	void Update(CEnemy* pEnemy) override;
private:
	int m_nNextStateCount; // 次のステートに移るカウンター
};

//***************************************************
// 敵の状態(Move)クラスの定義
//***************************************************
class CStateMove : public CEnemyState
{
public:
	CStateMove();
	~CStateMove();
	void Update(CEnemy* pEnemy) override;
private:
	int m_nNextStateCount;  // 次のステートに移るカウンター
};

//***************************************************
// 敵の状態(ATTACK)クラスの定義
//***************************************************
class CStateAttackSmash : public CEnemyState
{
public:
	CStateAttackSmash();
	~CStateAttackSmash();
	void Update(CEnemy* pEnemy) override;
private:
};

//***************************************************
// 敵の状態(ATTACKImpact)クラスの定義
//***************************************************
class CStateAttackImpact : public CEnemyState
{
public:
	CStateAttackImpact();
	~CStateAttackImpact();
	void Update(CEnemy* pEnemy) override;
private:
};

//***************************************************
// 敵の状態(DAMAGE)クラスの定義
//***************************************************
class CStateDamage : public CEnemyState
{
public:
	CStateDamage();
	~CStateDamage();
	void Update(CEnemy* pEnemy) override;
private:
};

//***************************************************
// 敵の状態(ROAR)クラスの定義
//***************************************************
class CStateRoar : public CEnemyState
{
public:
	CStateRoar();
	~CStateRoar();
	void Update(CEnemy* pEnemy) override;
private:
};

//***************************************************
// 敵の状態(DASH)クラスの定義
//***************************************************
class CStateDash : public CEnemyState
{
public:
	CStateDash();
	~CStateDash();
	void Update(CEnemy* pEnemy) override;
private:
};

//***************************************************
// 敵の状態(SPIN)クラスの定義
//***************************************************
class CStateSpin : public CEnemyState
{
public:
	CStateSpin(const int nTime);
	~CStateSpin();
	void Update(CEnemy* pEnemy) override;
private:
	int m_nTime; // 回転する時間
};

#endif
