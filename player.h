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

template <class T>
class CObserver;

class CMeshOrbit;
class CMeshField;
class CGameCamera;
class CColliderAABB;
class CVelocity;
class CRotation;
class CModel;

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CCharacter3D
{
public:

	static constexpr int MAX_LIFE = 10;				// HP
	static constexpr float MAX_STAMINA = 100.0f;	// スタミナ

	// モーションの種類
	enum TYPE
	{
		TYPE_NEUTRAL = 0,
		TYPE_MOVE,
		TYPE_PARRY,
		TYPE_JUMP,
		TYPE_LANDING,
		TYPE_DASH,
		TYPE_DAMAGE,
		TYPE_PUNCH,
		TYPE_DOWN_NEUTRAL,
		TYPE_AVOID,
		TYPE_ROUNDKICK,
		TYPE_STANCE,
		TYPE_MAX
	};

	// パリィの成功度
	enum PARRY
	{
		PARRY_MISS = 0,
		PARRY_WEAK,
		PARRY_NORMAL,
		PARRY_PARFECT,
		PARRY_MAX
	};

	// オブザーバーの種類
	enum OBSERVER
	{
		OBSERVER_HP = 0,
		OBSERVER_STAMINA,
		OBSERVER_SPECAL,
		OBSERVER_MAX
	};

	CPlayer(int nPriority = 4);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);
	void Load(void); // モーションのロード

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	void SetPosition(const D3DXVECTOR3 pos) { CCharacter3D::SetPosition(pos); }

	void ChangeState(std::shared_ptr<CPlayerState> pNewState);
	void SetHitStop(const int nTime) { CCharacter3D::SetHitStop(nTime); }
	void UpdateAvoid(void);	 // 回避の更新処理

		// ゲッター
	CColliderSphere* GetSphereCollider(void) { return m_pSphere.get(); }
	CColliderAABB* GetAABB(void) { return m_pAABB.get(); }
	CPlayerMovement* GetMovement(void) { return m_pMovement.get(); }

	// セッター
	void SetHpObserver(CObserver<int>* pObserver) { m_pHpObserver = pObserver; }
	void SetStaminaObserver(CObserver<float>* pObserver) { m_pStaminaObserver = pObserver; }

	int SuccessParry(void);

	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump);
	bool IsParry(const D3DXVECTOR3 pos);
	void SetAngle(const float angleY);
	bool CollisionObstacle(D3DXVECTOR3* pPos);
	void Orbit(const int nSegH, const D3DXCOLOR col); // 軌跡の処理
	void DeleteOrbit(void);							  // 軌跡のリセット
	void SetStance(void);							  // 構えモーションの設定
	void SetStamina(const float fStamina);

private:
	void CollisionImpact(CMeshField* pMeshField, D3DXVECTOR3* pPos, CMotion* pMotion); // インパクトの当たり判定
	bool IsMove(CMotion* pMotion);		// 移動できるか判定
	bool IsStance(CMotion* pMotion);	// 構えをだせるか判定
	bool IsAvoid(CMotion* pMotion);		// 回避を出せるか判定
	void Notify(void);					// オブザーバーへの通知処理
	void UpdateParry(void);
	void SetMoveAngle(CGameCamera* pCamera, CInputKeyboard* pKeyboard, CInputJoypad* pJoypad);
	void UpdateCollider(D3DXVECTOR3 pos);
	void UpdateStamina(void);

	std::unique_ptr<CStateMachine> m_pMachine;		// 状態の制御クラス
	std::unique_ptr<CPlayerMovement> m_pMovement;	// 移動処理
	std::unique_ptr<CColliderFOV> m_pFOV;			// 視界の判定
	std::unique_ptr<CColliderSphere> m_pSphere;		// 円のコライダー
	std::unique_ptr<CVelocity> m_pMove;				// 移動量
	std::unique_ptr<CColliderAABB> m_pAABB;			// コライダーAABB
	CMeshOrbit* m_pOrbit;							// 軌跡の処理
	CObserver<int>* m_pHpObserver;					// HPオブザーバークラスへのポインタ
	CObserver<float>* m_pStaminaObserver;			// スタミナオブザーバークラスへのポインタ
	D3DXVECTOR3 m_posOld;							// 前回の位置
	float m_fStamina;								// スタミナ
	int m_nParryTime;								// パリィの有効時間
	int m_nParryCounter;							// パリィのカウンター
	int m_nAttackCounter;							// 攻撃の有効時間
	bool m_bJump;									// ジャンプできるかどうか
	bool m_bDash;									// 走ってるかどうか
};

//***************************************************
// プレイヤーの移動処理の定義
//***************************************************
class CPlayerMovement
{
public:
	CPlayerMovement();
	~CPlayerMovement();

	// プレイヤーのmoveを受け取る
	void Init(CVelocity* pMove, CRotation* pRot);
	bool MoveKeyboard(CInputKeyboard* pKeyboard, const float fSpeed, float* pRotDest);
	bool MoveJoypad(CInputJoypad* pJoypad, const float fSpeed, float* pRotDest);
	void MoveForward(const float fSpeed);
	void Set(const D3DXVECTOR3 move);
private:
	CRotation* m_pRot;	// 向き
	CVelocity* m_pMove;		// 移動量
};

#endif