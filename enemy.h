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
#include<memory>
#include<vector>
#include<list>

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
class CColliderAABB;
class CVelocity;
class CPlayer;

template <class T>
class CObserver;
class CColliderCapsule;
class CColliderFOV;
class CEnemyStateManager;
class CObstacleManager;
class CMeshField;

//***************************************************
// 敵クラスの定義
//***************************************************
class CEnemy : public CCharacter3D
{
public:

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0, // ニュートラル
		MOTIONTYPE_MOVE,		// 移動
		MOTIONTYPE_SMASH,		// 振り下ろし攻撃
		MOTIONTYPE_JUMP,		// ジャンプ
		MOTIONTYPE_LANDING,		// 着地
		MOTIONTYPE_DAMAGEL,		// 大ダメージ
		MOTIONTYPE_IMPACT,		// 衝撃波攻撃
		MOTIONTYPE_ROAR,		// 叫び
		MOTIONTYPE_DASH,		// 走り
		MOTIONTYPE_SPIN,		// 回転攻撃
		MOTIONTYPE_HIT,			// カウンターが当たった時
		MOTIONTYPE_DAMAGES,		// 小ダメージ
		MOTIONTYPE_GUARD,		// ガード
		MOTIONTYPE_STEP,		// ステップ
		MOTIONTYPE_SWING,		// スイング攻撃
		MOTIONTYPE_JUMPATTACK,  // ジャンプ攻撃
		MOTIONTYPE_DEATH,		// 死亡
		MOTIONTYPE_DOWN,		// ダウン
		MOTIONTYPE_SUPER_HIT,	// 特大ヒット
		MOTIONTYPE_COMBODAMAGE, // 連続ダメージ
		MOTIONTYPE_RIGHTMOVE,	// 横歩き(右)
		MOTIONTYPE_LEFTMOVE,	// 横歩き(左)
		MOTIONTYPE_RUSH,		// 突進攻撃
		MOTIONTYPE_END_RUSH,	// 突進攻撃終了
		MOTIONTYPE_BACKKICK,	// 後ろ蹴り
		MOTIONTYPE_START_RUSH,	// 突進開始
		MOTIONTYPE_LOOK_BACKL,	// 後ろを見る(左)
		MOTIONTYPE_LOOK_BACKR,	// 後ろを見る(右)
		MOTIONTYPE_SWEEP_RIGHT,	// 薙ぎ払い(右)
		MOTIONTYPE_SWEEP_LEFT,	// 薙ぎ払い(左)
		MOTIONTYPE_RUSH_SWING,	// 突進なぎ
		MOTIONTYPE_BACK_HIT,	// 後ろからカウンター
		MOTIONTYPE_STANP_DAMAGE,// 踏みつけダメージ
		MOTIONTYPE_STANP_DEATH, // 踏みつけ死亡
		MOTIONTYPE_STANP_DOWN,	// 踏みつけダウン(死亡)
		MOTIONTYPE_BRISK_MOVE,	// 早歩き
		MOTIONTYPE_COUNTER,		// カウンター
		MOTIONTYPE_REVENGE_IMPACT, // 衝撃波の跳ね返し
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	// 攻撃の結果
	typedef enum
	{
		RESULT_NONE = 0,  // 何も無し
		RESULT_PARRY,	  // パリィされた
		RESULT_AVOID,	  // 回避された
		RESULT_HIT,		  // 攻撃が当たった
		RESULT_SPREVENGE, // 絶対反撃モード	  
		RESULT_MAX
	}RESULT;

	// モデルの種類
	typedef enum
	{
		MODEL_NONE = -1, // モデル無し
		MODEL_WAIST,	 // 腰
		MODEL_CHEST,	 // 胸
		MODEL_HEAD,		 // 頭
		MODEL_ARMUR,	 // 右上腕
		MODEL_ARMFR,	 // 右前腕
		MODEL_HANDR,	 // 右手
		MODEL_ARMUL,	 // 左上腕
		MODEL_ARMFL,	 // 左前腕
		MODEL_HANDL,	 // 左手
		MODEL_LEGUR,	 // 右太もも
		MODEL_LEGDR,	 // 右すね
		MODEL_FOOTR,	 // 右足
		MODEL_LEGUL,	 // 左太もも
		MODEL_LEGDL,	 // 左すね
		MODEL_FOOTL,	 // 左足
		MODEL_WEPON,	 // 武器
		MODEL_MAX
	}MODEL;

	CEnemy();
	~CEnemy();

	static CEnemy* Create(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size,const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	D3DXVECTOR3 GetPosition(void);
	CEnemyMovement* GetMovement(void);

	void SelectDamageMotion(int success, const D3DXVECTOR3 ImpactPos); // どのダメージモーションが出るか判定する関数
	bool IsDamageMotion(void);
	bool CollisionWepon(void);
	void ChasePlayer(float chaseScal, const float speedScal = 1.0f);	// プレイヤーを追いかける処理
	void AngleToPlayer(void);											// プレイヤーの方向を見る処理
	bool CheckDistane(const float fRadius);								// 距離の判定
	void Orbit(const int nSegH, const D3DXCOLOR col);					// 軌跡の設定
	void DeleteOrbit(void);												// 軌跡の消去
	void ChangeState(std::shared_ptr<CEnemyState> pNewState);
	bool CollisionObstacle(D3DXVECTOR3* pPos);
	void SetObserver(CObserver<int>* pObserver) { m_pObserver = pObserver; }
	void SetRubble(void);
	void Hit(const int nDamage);
	void SetAngle(const float fAngle);
	bool CollisionFOV(const D3DXVECTOR3 pos);
	bool CollisionFOV(const D3DXVECTOR3 Targetpos, const float fLeftAngle, const float fRightAngle);
	void RushEffect(void);

	CEnemyStateManager* GetStateManager(void) { return m_pStateManager.get(); }

	// 武器攻撃の結果
	RESULT WeponAttackResult(CPlayer* pPlayer);

	// 攻撃の結果
	RESULT AttackResult(CPlayer* pPlayer,const MODEL model, const float fRadius);

	bool CheckObstacleDistance(const float fRange);
private:
	void CollisionPlayer(CMotion* pPlayerMotion, CPlayer* pPlayer);
	void SetParent(const int nCnt, const D3DXVECTOR3 offPos, D3DXMATRIX* pMatrixOut);
	void Notify(void);										// オブザーバーへの通知処理
	void UpdateCollider(const D3DXVECTOR3 pos);				// コライダーの更新
	void Config(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size);
	bool CollisionObstacleToWepon(CObstacle* pObstacle);	// 障害物と武器の当たり判定
	bool SetTNTEffect(CObstacle* pObstacle);				// 障害物の爆発の演出
	void CollisionImpact(CMotion* pMotion,CMeshField *pMeshField,D3DXVECTOR3 *pPos);			// インパクトの当たり判定

	std::unique_ptr<CColliderCapsule> m_pCapsule;			// カプセルコライダー
	std::unique_ptr<CColliderAABB> m_pAABB;					// AABBのコライダー
	std::unique_ptr<CStateMachine> m_pMachine;				// 状態マシーン
	std::unique_ptr<CColliderSphere> m_pSphere;				// 円の当たり判定クラス
	std::unique_ptr<CEnemyMovement> m_pMovement;			// 敵の移動制御クラス
	std::unique_ptr<CColliderFOV> m_pFOV;					// 視界の判定
	std::unique_ptr<CEnemyStateManager> m_pStateManager;	// 状態マネージャーの生成
	CObserver<int>* m_pObserver;							// オブザーバークラスへのポインタ
	CMeshOrbit* m_pOrbit;									// 軌跡
	D3DXMATRIX m_weponMatrix;								// 武器のワールドマトリックス
	D3DXMATRIX m_RushEffectMtx;								// 突進攻撃のエフェクトのマトリックス
	D3DXVECTOR3 m_posOld;									// 前回の位置
	bool m_bSetMatrix;										// ワールドマトリックスの設定をしたかどうか
};

#endif