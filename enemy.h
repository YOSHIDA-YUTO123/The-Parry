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
#include "Collision.h"
#include<memory>
#include"character3D.h"
#include<vector>

//***************************************************
// 前方宣言
//***************************************************
class CCharacter3D;
class CMotion;
class CEnemyMotionController;
class CModel;
class CEnemyState;
class CEnemyIdle;
class CMeshOrbit;
class CStateMachine;
class CObject3D;
class CColliderSphere;
class CEnemyMovement;

//***************************************************
// 敵クラスの定義
//***************************************************
class CEnemy : public CObject
{
public:

	// モーションの種類
	typedef enum
	{
		MOTION_NEUTRAL = 0,
		MOTION_MOVE,
		MOTION_SMASH,
		MOTION_JUMP,
		MOTION_LANDING,
		MOTION_DAMAGEL,
		MOTION_IMPACT,
		MOTION_ROAR,
		MOTION_DASH,
		MOTION_SPIN,
		MOTION_HIT,
		MOTION_DAMAGES,
		MOTION_MAX
	}MOTION;

	CEnemy();
	~CEnemy();

	static CEnemy* Create(const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	D3DXVECTOR3 GetPosition(void) { return m_pCharactor->GetPosition(); }

	void SelectDamageMotion(int success);	// どのダメージモーションが出るか判定する関数
	bool IsDamageMotion(void);				// ダメージモーションかどうか
	bool CollisionWepon(void);
	void ChasePlayer(float chaseScal, const float speedScal = 1.0f);	// プレイヤーを追いかける処理
	void AngleToPlayer(void);											// プレイヤーの方向を見る処理
	bool CheckDistane(const float fRadius);								// 距離の判定
	void Orbit(const int nSegH, const D3DXCOLOR col);					// 軌跡の設定
	void DeleteOrbit(void);												// 軌跡の消去
	void ChangeState(std::shared_ptr<CEnemyState> pNewState);
	bool CollisionObstacle(D3DXVECTOR3* pPos);
	void SetSuccess(const int success) { m_nParrySuccess = success; }   // 成功度の設定
	CMotion* GetMotion(void) { return m_pMotion.get(); }
	CEnemyMovement* GetMovement(void) { return m_pMovement.get(); }
private:
	void SetParent(const int nCnt);
	void Load(void);
	
	std::unique_ptr<CColliderAABB> m_pAABB;				// AABBのコライダー
	std::unique_ptr<CStateMachine> m_pMachine;
	std::shared_ptr<CCharacter3D> m_pCharactor;			// キャラクタークラス
	std::unique_ptr<CColliderSphere> m_pSphere;			// 円の当たり判定クラス
	std::unique_ptr<CMotion> m_pMotion;					// 敵のモーションの制御クラスのポインタ
	std::shared_ptr<CVelocity> m_pMove;					// 移動クラスの生成
	std::vector<CModel*> m_apModel;						// モデルクラスへのポインタ
	std::unique_ptr<CEnemyMovement> m_pMovement;		// 敵の移動制御クラス
	CMeshOrbit* m_pOrbit;								// 軌跡
	D3DXMATRIX m_weponMatrix;							// 武器のワールドマトリックス
	D3DXVECTOR3 m_posOld;								// 前回の位置
	D3DXVECTOR3 m_Size;									// 敵の全体の大きさ
	int m_nParrySuccess;								// パリィの成功度(保存用)
	int m_nNumModel;									// モデルの最大数
};

//***************************************************
// 敵の移動処理クラスの定義
//***************************************************
class CEnemyMovement
{
public:
	CEnemyMovement();
	~CEnemyMovement();
	void Init(std::shared_ptr<CVelocity> enemyMove, std::shared_ptr<CCharacter3D> enemy);	
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump);		// 吹き飛び関数(戻り値はアタッカーまでの角度)
	void MoveForWard(const float fSpeed);
	void SetMoveDir(const float dir, const float fSpeed); // 移動方向の設定
	void Jump(const float fHeight);
private:
	std::shared_ptr<CVelocity> m_pMove;		// 敵の移動制御用変数
	std::shared_ptr<CCharacter3D> m_pEnemy;	// キャラクター
};
#endif