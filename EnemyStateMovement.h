//===================================================
//
// 敵の移動状態の処理 [EnemyStateMovemet.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ENEMYSTATEMOVEMEMT_H_
#define _ENEMYSTATEMOVEMEMT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "EnemyState.h"

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
// 敵の横移動クラスの定義
//***************************************************
class CEnemySideMove : public CEnemyState
{
public:
	CEnemySideMove();
	CEnemySideMove(ID id);
	virtual ~CEnemySideMove();
	virtual void Init(void) override;
	virtual void Update(void) override;
protected:
	static constexpr int MOVE_TIME = 60; // 最低移動時間
	static constexpr float OBSTACLE_DISTANCE = 450.0f;	 // 障害物との距離
	static constexpr float ACTION_DISTANCE = 750.0f; // 攻撃してくる距離

	void SetParam(const float fMoveAngle, const float fMoveValue);
private:
	float m_fMoveAngle;	// 移動する方向
	float m_fMoveValue;	// 移動量		
	int m_nTime;		// 横移動の時間
};

//***************************************************
// 敵の状態(RIghtMove)クラスの定義
//***************************************************
class CEnemyRightMove : public CEnemySideMove
{
public:
	CEnemyRightMove();
	~CEnemyRightMove();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(LeftMove)クラスの定義
//***************************************************
class CEnemyLeftMove : public CEnemySideMove
{
public:
	CEnemyLeftMove();
	~CEnemyLeftMove();
	void Init(void) override;
	void Update(void) override;
private:
};

//***************************************************
// 敵の状態(Brisk_Move)クラスの定義
//***************************************************
class CEnemyBriskMove : public CEnemyState
{
public:
	CEnemyBriskMove();
	~CEnemyBriskMove();
	void Init(void) override;
	void Update(void) override;
private:
	D3DXVECTOR3 m_destPos; // 目的の位置
};
#endif
