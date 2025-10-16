//===================================================
//
// プレイヤー [player.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PLAYER_H_
#define _PLAYER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"character3D.h"
#include<vector>
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CMotion;
class CCharacter3D;
class CCollisionFOV;
class CColliderSphere;
class CColliderFOV;
class CStateMachine;
class CPlayerState;
class CShadowS;
class CPlayerMovement;

template <class T> class CObserver;

class CMeshOrbit;
class CMeshField;
class CGameCamera;
class CColliderAABB;
class CVelocity;
class CRotation;
class CModel;
class CColliderCapsule;

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CCharacter3D
{
public:

	static constexpr float MAX_STAMINA = 100.0f;	// スタミナ
	static constexpr float MAX_REVENGE = 100.0f;	// 反撃ゲージの最大

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0,		// ニュートラル
		MOTIONTYPE_MOVE,			// 移動
		MOTIONTYPE_PARRY,			// パリィ
		MOTIONTYPE_JUMP,			// ジャンプ
		MOTIONTYPE_LANDING,			// 着地
		MOTIONTYPE_DASH,			// ダッシュ
		MOTIONTYPE_DAMAGE,			// ダメージ
		MOTIONTYPE_PUNCH,			// パンチ
		MOTIONTYPE_DOWN_NEUTRAL,	// ダウンニュートラル
		MOTIONTYPE_AVOID,			// 回避
		MOTIONTYPE_ROUNDKICK,		// 回し蹴り
		MOTIONTYPE_STANCE,			// 構え
		MOTIONTYPE_REVENGE,			// 反撃
		MOTIONTYPE_REVENGEATTACK,	// 反撃(攻撃)
		MOTIONTYPE_BACK_DAMAGE,		// 後ろから攻撃を受けた
		MOTIONTYPE_DOWN_NEUTRA_BACK,// ダウンニュートラル(Back)
		MOTIONTYPE_JUMP_ATTACK,		// ジャンプ攻撃
		MOTIONTYPE_LEFT_MOVE,		// 左移動
		MOTIONTYPE_RIGHT_MOVE,		// 右移動
		MOTIONTYPE_BACK_MOVE,		// 後ろ移動
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	// パリィの成功度
	typedef enum 
	{
		PARRY_MISS = 0,
		PARRY_WEAK,
		PARRY_NORMAL,
		PARRY_PARFECT,
		PARRY_MAX
	}PARRY;

	// オブザーバーの種類
	typedef enum
	{
		OBSERVER_HP = 0,
		OBSERVER_STAMINA,
		OBSERVER_SPECAL,
		OBSERVER_MAX
	}OBSERVER;

	// モデルの種類
	typedef enum
	{
		MODEL_WAIST = 0, // 腰
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
		MODEL_MAX
	}MODEL;

	// 反撃の種類
	typedef enum
	{
		PARRYMOTION_KICK = 0,
		PARRYMOTION_JUMP,
		PARRYMOTION_MAX
	}PARRYMOTION;

	CPlayer();
	~CPlayer();

	static CPlayer* Create(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size, const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void ChangeState(std::shared_ptr<CPlayerState> pNewState);
	void SetHitStop(const int nTime) { CCharacter3D::SetHitStop(nTime); }

	// ゲッター
	CColliderSphere* GetSphereCollider(void) { return m_pSphere.get(); }
	CColliderAABB* GetAABB(void) { return m_pAABB.get(); }
	CPlayerMovement* GetMovement(void) { return m_pMovement.get(); }
	CColliderCapsule* GetCapsuleCollider(void) { return m_Capsule.get(); }

	// セッター
	void SetHpObserver(CObserver<int>* pObserver) { m_pHpObserver = pObserver; }
	void SetStaminaObserver(CObserver<float>* pObserver) { m_pStaminaObserver = pObserver; }
	void SetRevengeObserver(CObserver<float>* pObserver) { m_pRevengeObserver = pObserver; }

	int SuccessParry(void);

	bool IsParry(const D3DXVECTOR3 pos);
	void SetAngle(const float angleY);
	bool CollisionObstacle(D3DXVECTOR3* pPos);
	void Orbit(const int nSegH, const D3DXCOLOR col); // 軌跡の処理
	void DeleteOrbit(void);							  // 軌跡のリセット
	void SetStance(const D3DXVECTOR3 enemyPos,const PARRYMOTION parry = PARRYMOTION_KICK);		 // 構えモーションの設定
	void SetStamina(const float fStamina);

	/**
	* @brief 矩形の判定
	* @param AABBのコライダー
	* @return 衝突したかどうか
	*/
	bool CollisionAABB(CColliderAABB* pAABB);
	bool CollisionCapsule(CColliderCapsule* pCapsule,const bool bPush = true);
	void SetRevengeEffect(void);					// 絶対反撃のエフェクトの設定
	void EnableGravity(const bool bEnable) { m_bGravity = bEnable; } // 重力の判定の設定
	void SetDamageMotion(const D3DXVECTOR3 AttackerPos, const int nDamage, const float fBack = 100.0f, const float fForward = 20.0f);
	void SetRubble(const D3DXVECTOR3 pos);
	bool GetRevengeState(void) const { return m_bRevenge; }
	void SetParryResult(const int nParry);
private:
	void CollisionImpact(CMeshField* pMeshField, D3DXVECTOR3* pPos, CMotion* pMotion); // インパクトの当たり判定
	bool IsMove(CMotion* pMotion);		// 移動できるか判定
	bool IsStance(CMotion* pMotion);	// 構えをだせるか判定
	bool IsAvoid(CMotion* pMotion);		// 回避を出せるか判定
	bool IsDamage(CMotion* pMotion);	// ダメージ状態かどうか
	void Notify(void);					// オブザーバーへの通知処理
	void UpdateParry(void);
	void SetMoveAngle(CGameCamera* pCamera, CInputKeyboard* pKeyboard, CInputJoypad* pJoypad);
	void UpdateCollider(D3DXVECTOR3 pos);
	void UpdateStamina(void);
	bool CollisionBlock(D3DXVECTOR3 *pPos);
	void Config(const int nLife,const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size);
	void UpdateMove(CMotion* pMotion,const bool bAlive, CInputKeyboard *pKeyboard, CInputJoypad *pJoyPad, CGameCamera* pGameCamera);
	void UpdateRockOnMove(CMotion* pMotion, const bool bAlive, CInputKeyboard* pKeyboard, CInputJoypad* pJoyPad, CGameCamera* pGameCamera);
	bool GetMoveState(CMotion* pMotion);
	void DebugInfo(void);

	static constexpr int NUM_RUBBLE = 16;			// 瓦礫の数

	std::unique_ptr<CStateMachine> m_pMachine;		// 状態の制御クラス
	std::unique_ptr<CPlayerMovement> m_pMovement;	// 移動処理
	std::unique_ptr<CColliderFOV> m_pFOV;			// 視界の判定
	std::unique_ptr<CColliderSphere> m_pSphere;		// 円のコライダー
	std::unique_ptr<CColliderCapsule> m_Capsule;	// カプセルコライダー
	std::unique_ptr<CColliderAABB> m_pAABB;			// コライダーAABB
	CMeshOrbit* m_pOrbit;							// 軌跡の処理
	CObserver<int>* m_pHpObserver;					// HPオブザーバークラスへのポインタ
	CObserver<float>* m_pStaminaObserver;			// スタミナオブザーバークラスへのポインタ
	CObserver<float>* m_pRevengeObserver;			// 反撃オブザーバークラスへのポインタ
	D3DXVECTOR3 m_posOld;							// 前回の位置
	PARRY m_ParryResult;							// パリィの結果
	PARRYMOTION m_ParryMotion;						// パリィモーション
	float m_fStamina;								// スタミナ
	float m_fRevengeValue;							// 反撃ゲージ量
	float m_fDestRevengeValue;						// 目的の反撃ゲージ量
	int m_nPerfectCnt;								// パリィのパーフェクトの回数
	int m_nNormalCnt;								// パリィの普通の回数
	int m_nWeakCnt;									// パリィの弱いの回数
	int m_nParryTime;								// パリィの有効時間
	int m_nParryCounter;							// パリィのカウンター
	int m_nAttackCounter;							// 攻撃の有効時間
	bool m_bGravity;								// 重力をつけるかどうか
	bool m_bJump;									// ジャンプできるかどうか
	bool m_bDash;									// 走ってるかどうか
	bool m_bRevenge;								// 絶対反撃できるかどうか
};

#endif