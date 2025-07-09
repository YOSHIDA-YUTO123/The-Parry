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
class CShadow;
class CMotion;
class CEnemyMotionController;
class CModel;
class CEnemyState;
class CEnemyIdle;
class CMeshOrbit;
class CStateMachine;
class CObject3D;

//***************************************************
// 敵クラスの定義
//***************************************************
class CEnemy : public CObject
{
public:

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0,
		MOTIONTYPE_MOVE,
		MOTIONTYPE_SMASH,
		MOTIONTYPE_JUMP,
		MOTIONTYPE_LANDING,
		MOTIONTYPE_DAMAGE,
		MOTIONTYPE_IMPACT,
		MOTIONTYPE_ROAR,
		MOTIONTYPE_DASH,
		MOTIONTYPE_SPIN,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CEnemy();
	~CEnemy();

	static CEnemy* Create(const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump);
	bool CollisionWepon(void);
	void ChasePlayer(float chaseScal, const float speedScal = 1.0f);	// プレイヤーを追いかける処理
	void AngleToPlayer(void);												// プレイヤーの方向を見る処理
	bool CheckDistane(const float fRadius);								// 距離の判定
	void MoveForWard(const float fSpeed);								// 自分の向いている方向に向かって進む処理
	void Orbit(const int nSegH, const D3DXCOLOR col, const int nLife);	// 軌跡の設定
	CMotion* GetMotion(void) { return m_pMotion.get(); }
	void ChangeState(std::shared_ptr<CEnemyState> pNewState);
private:
	void SetParent(const int nCnt);
	void Load(void);

	std::unique_ptr<CStateMachine> m_pMachine;
	//std::unique_ptr<CEnemyState> m_pState;				// 敵の状態パターン
	std::unique_ptr<CCharacter3D> m_pCharactor;			// キャラクタークラス
	std::unique_ptr<CCollisionSphere> m_pSphere;		// 円の当たり判定クラス
	std::unique_ptr<CShadow> m_pShadow;					// 影のクラスへのポインタ
	std::unique_ptr<CMotion> m_pMotion;					// 敵のモーションの制御クラスのポインタ
	std::unique_ptr<CVelocity> m_pMove;					// 移動クラスの生成
	std::vector<CModel*> m_apModel;						// モデルクラスへのポインタ
	CMeshOrbit* m_pOrbit;								// 軌跡
	D3DXMATRIX m_weponMatrix;							// 武器のワールドマトリックス
	int m_nNumModel;									// モデルの最大数
};

#endif