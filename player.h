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
class CShadow;
class CScoreLerper;
class CCharacter3D;
class CCollisionFOV;
class CPlayerMotionController;
class CColliderSphere;
class CColliderFOV;
class CStateMachine;
class CPlayerState;
class CShadowS;

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CObject
{
public:

	// モーションの種類
	typedef enum
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
		TYPE_MAX
	}TYPE;

	CPlayer(int nPriority = 4);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = Const::VEC3_NULL, const D3DXVECTOR3 rot = Const::VEC3_NULL);
	void Load(void); // モーションのロード

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	bool MoveKeyboard(CInputKeyboard *pKeyboard);
	void MoveJoypad(CInputJoypad* pJoypad);
	void UpdateParry(void);
	CColliderSphere* GetSphereCollider(void) { return m_pSphere.get(); }
	D3DXVECTOR3 GetPos(void) const { return m_pCharacter3D->GetPosition(); }
	D3DXVECTOR3 GetModelPos(const int nIdx) { return math::GetPositionFromMatrix(m_apModel[nIdx]->GetMatrixWorld()); }
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff,const float jump);
	bool IsParry(const D3DXVECTOR3 pos);
	void SetAngle(const float angleY);
	bool CollisionObstacle(D3DXVECTOR3* pPos);
	void ChangeState(std::shared_ptr<CPlayerState> pNewState);
	void MoveForward(const float fSpeed);
	CMotion* GetMotion(void) { return m_pMotion.get(); } // モーションの取得

private:
	CShadowS* m_pShadowS; // 影(ステンシル)
	std::unique_ptr<CMotion> m_pMotion;					// モーションのクラスへのポインタ
	std::unique_ptr<CStateMachine> m_pMachine;			// 状態の制御クラス
	std::unique_ptr<CCharacter3D> m_pCharacter3D;		// キャラクタークラス
	std::unique_ptr<CColliderFOV> m_pFOV;				// 視界の判定
	std::unique_ptr<CColliderSphere> m_pSphere;		// 円のコライダー
	CScoreLerper *m_pScore;							// スコアクラスへのポインタ
	std::vector<CModel*> m_apModel;					// モデルクラスのポインタ

	std::unique_ptr<CShadow> m_pShadow;				// 影クラスへのポインタ
	CVelocity* m_pMove;								// 移動量
	D3DXVECTOR3 m_posOld;							// 前回の位置

	int m_nParryTime;								// パリィの有効時間
	int m_nParryCounter;							// パリィ―のカウンター

	int m_nNumModel;								// モデルの最大数
	bool m_bJump;									// ジャンプできるかどうか
	bool m_bDash;									// 走ってるかどうか
};

#endif