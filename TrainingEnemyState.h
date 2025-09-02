//================================================
//
// 練習用の敵の状態 [TrainingEnemyState.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _TRAININGENEMYSTATE_H_
#define _TRAININGENEMYSTATE_H_

//************************************************
// インクルードファイル
//************************************************
#include "statebase.h"

//*************************************************
// 前方宣言
//*************************************************
class CTrainingEnemy;

//*************************************************
// 練習用の敵の状態のクラスの定義
//*************************************************
class CTrainingEnemyState : public CStateBase
{
public:
	// IDの種類
	typedef enum
	{
		ID_BASE = 0,	 // 無し
		ID_IDLE,		 // 通常状態
		ID_ACTION,		 // 攻撃
		ID_DAMAGE,		 // ダメージ
		ID_MAX
	}ID;

	CTrainingEnemyState(ID Id);
	virtual ~CTrainingEnemyState();
	virtual void Init(void) {};
	virtual void Update(void) {};
	virtual void Uninit(void) {};
	virtual int GetID(void) const { return m_ID; }
	void SetOwner(CTrainingEnemy* pEnemy) { m_pEnemy = pEnemy; }

protected:
	CTrainingEnemy* GetEnemy(void) { return m_pEnemy; }
private:
	CTrainingEnemy* m_pEnemy; // 敵クラスへのポインタ
	ID m_ID; // IDの取得
};

//*************************************************
// 通常状態クラスの定義
//*************************************************
class CTrainingEnemyIdle : public CTrainingEnemyState
{
public:
	CTrainingEnemyIdle();
	~CTrainingEnemyIdle();

	void Init(void) override;
	void Update(void) override;
private:
};

//*************************************************
// 攻撃状態クラスの定義
//*************************************************
class CTrainingEnemyAction : public CTrainingEnemyState
{
public:
	CTrainingEnemyAction();
	~CTrainingEnemyAction();

	void Init(void) override;
	void Update(void) override;
private:
};

#endif