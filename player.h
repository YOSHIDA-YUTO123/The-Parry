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

//***************************************************
// マクロ定義
//***************************************************
#define NUM_PARTS (15) // モデルの最大数

//***************************************************
// 前方宣言
//***************************************************
class CInputKeyboard;
class CInputJoypad;
class CMotion;
class CShadow;
class CScoreLerper;

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CCharacter3D
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
		MOTIONTYPE_DASH,
		MOTIONTYPE_DAMAGE,
		MOTIONTYPE_PARRY,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CPlayer(int nPriority = 3);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool MoveKeyboard(CInputKeyboard *pKeyboard);
	void MoveJoypad(CInputJoypad* pJoypad);
	void Load(void);
	CCollisionSphere* GetSphere(void);
	void UpdateParry(void);
	D3DXVECTOR3 GetModelPos(const int nIdx) { return math::GetPositionFromMatrix(m_apModel[nIdx]->GetMatrixWorld()); }
	CCollisionFOV* GetFOV(void) { return m_pFOV; }
	void SetMotion(const int type, bool bBlend, const int nFrameBlend);
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff,const float jump);

private:
	void TransitionMotion(void);					// モーションの遷移

	CCollisionFOV* m_pFOV;				// 視界の判定
	CCollisionSphere* m_pCollision;		// 当たり判定
	CMotion *m_pMotion;					// モーションのクラスへのポインタ
	CScoreLerper *m_pScore;				// スコアクラスへのポインタ
	CModel* m_apModel[NUM_PARTS];		// モデルクラスへのポインタ
	CShadow* m_pShadow;					// 影クラスへのポインタ
	CVelocity* m_pMove;					// 移動量
	D3DXVECTOR3 m_posOld;				// 前回の位置

	int m_nParryTime;					// パリィの有効時間
	int m_nParryCounter;				// パリィ―のカウンター

	int m_nNumModel;					// モデルの最大数
	bool m_bJump;						// ジャンプできるかどうか
	bool m_bDash;						// 走ってるかどうか
};

#endif