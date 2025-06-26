//===================================================
//
// 敵 [enemy.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ENEMY_H_
#define _ENEMY_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"character3D.h"
#include "motion.h"
#include "Collision.h"
class CShadow;

//***************************************************
// マクロ定義
//***************************************************
#define ENEMY_MAX_PARTS (16) // モデルの最大数

//***************************************************
// 敵のAIクラス
//***************************************************
class CEnemyAI
{
public:

	// 敵の行動の種類
	typedef enum
	{
		ACTION_IDLE = 0, // 何もしない
		ACTION_MOVE,	 // 移動
		ACTION_ATTACK,	 // 攻撃
		ACTION_WAIT,
		ACTION_MAX
	}ACTION;

	CEnemyAI();
	~CEnemyAI();
	void Init(CMotion* pMotion, const int nBlendFrame = 10, const int nFirstMotion = 0);
	void Update(void);
	int CheckDistance(const D3DXVECTOR3 dest,const D3DXVECTOR3 pos, const float fRadius);
	int IsAttack(void);
	bool Wait(void);
private:
	CMotion* m_pMotion;			// モーションのクラスへのポインタ
	ACTION m_Action;			// 敵の行動パターン
	int m_nCounterAction;		// 行動のカウンター
};

//***************************************************
// 敵クラスの定義
//***************************************************
class CEnemy : public CCharacter3D
{
public:

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0,
		MOTIONTYPE_MOVE,
		MOTIONTYPE_ACTION,
		MOTIONTYPE_JUMP,
		MOTIONTYPE_LANDING,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CEnemy(int nPriority = 3);
	~CEnemy();

	static CEnemy* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void UpdateMoveMotion(void);
private:
	void TransitionMotion(void);
	void SetParent(const int nCnt);
	void Load(void);

	CCollisionSphere* m_pCollision;		// 当たり判定
	CShadow* m_pShadow;					// 影のクラスへのポインタ
	CEnemyAI* m_pAI;					// 敵のAI
	CMotion* m_pMotion;					// モーションのクラスへのポインタ
	CModel* m_apModel[ENEMY_MAX_PARTS];	// モデルクラスへのポインタ
	CVelocity* m_pMove;					// 移動量
	D3DXMATRIX m_weponMatrix;			// 武器のワールドマトリックス
	float m_fSpeed;						// 移動速度
	int m_nNumModel;					// モデルの最大数
};

#endif