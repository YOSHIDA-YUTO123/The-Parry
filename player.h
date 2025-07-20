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
#include "Collision.h"
#include<vector>
#include"math.h"
#include"model.h"
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
class CObserver;

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CObject
{
public:

	static constexpr int MAX_LIFE = 10;	// HP

	// モーションの種類
	enum TYPE
	{
		TYPE_NEUTRAL = 0,
		TYPE_MOVE,
		TYPE_ACTION,
		TYPE_JUMP,
		TYPE_LANDING,
		TYPE_DASH,
		TYPE_DAMAGE,
		TYPE_PARRY,
		TYPE_DOWN_NEUTRAL,
		TYPE_AVOID,
		TYPE_ROUNDKICK,
		TYPE_MAX
	};

	CPlayer(int nPriority = 4);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);
	void Load(void); // モーションのロード

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	virtual CPlayerMovement* GetMovement(void) { return nullptr; } // 移動制御クラスの取得(派生したプレイヤーが持っている移動制御の取得)

	// ゲッター
	D3DXVECTOR3 GetPos(void) const { return m_pCharacter3D->GetPosition(); }
	D3DXVECTOR3 GetModelPos(const int nIdx) { return math::GetPositionFromMatrix(m_apModel[nIdx]->GetMatrixWorld()); }
	D3DXVECTOR3 GetRotaition(void) const { return m_pCharacter3D->GetRotation()->GetDest(); }

	CMotion* GetMotion(void) { return m_pMotion.get(); } // モーションの取得

	void ChangeState(std::shared_ptr<CPlayerState> pNewState);
	void SetHitStop(const int nTime) { m_pCharacter3D->SetHitStop(nTime); }

protected:
	CCharacter3D* GetCharacter(void) { return m_pCharacter3D.get(); }
private:
	std::unique_ptr<CStateMachine> m_pMachine;		// 状態の制御クラス
	std::unique_ptr<CMotion> m_pMotion;				// モーションのクラスへのポインタ
	std::unique_ptr<CCharacter3D> m_pCharacter3D;	// キャラクタークラス
	std::vector<CModel*> m_apModel;					// モデルクラスのポインタ
	int m_nNumModel;								// モデルの最大数
};

//***************************************************
// プレイヤークラスの定義(ゲーム中)
//***************************************************
class CPlayerGame : public CPlayer
{
public:
	// パリィの成功度
	enum PARRY
	{
		PARRY_MISS = 0,
		PARRY_WEAK,
		PARRY_NORMAL,
		PARRY_PARFECT,
		PARRY_MAX
	};

	CPlayerGame();
	~CPlayerGame();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static CPlayerGame* Create(const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	// ゲッター
	CColliderSphere* GetSphereCollider(void) { return m_pSphere.get(); }
	CPlayerMovement* GetMovement(void) { return m_pMovement.get(); }
	int SuccessParry(const int nParfectTime);

	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump);
	bool IsParry(const D3DXVECTOR3 pos);
	void SetAngle(const float angleY);
	bool CollisionObstacle(D3DXVECTOR3* pPos);
	void SetObserver(CObserver* pObserver) { m_pObserver = pObserver; }
private:
	void Notify(void);
	void UpdateParry(void);

	std::unique_ptr<CPlayerMovement> m_pMovement;	// 移動処理
	std::unique_ptr<CColliderFOV> m_pFOV;			// 視界の判定
	std::unique_ptr<CColliderSphere> m_pSphere;		// 円のコライダー
	std::unique_ptr<CVelocity> m_pMove;				// 移動量
	CObserver* m_pObserver;							// オブザーバークラスへのポインタ
	D3DXVECTOR3 m_posOld;							// 前回の位置
	int m_nParryTime;								// パリィの有効時間
	int m_nParryCounter;							// パリィ―のカウンター

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
	void Set(const D3DXVECTOR3 move) { m_pMove->Set(move); }
private:
	CRotation* m_pRot;	// 向き
	CVelocity* m_pMove;		// 移動量
};

#endif