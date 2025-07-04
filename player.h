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

//***************************************************
// プレイヤークラスの定義
//***************************************************
class CPlayer : public CObject
{
public:

	CPlayer(int nPriority = 3);
	~CPlayer();

	static CPlayer* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool MoveKeyboard(CInputKeyboard *pKeyboard);
	void MoveJoypad(CInputJoypad* pJoypad);
	CCollisionSphere* GetSphere(void) { return m_pSphere.get(); }
	void UpdateParry(void);
	D3DXVECTOR3 GetPos(void) const { return m_pCharacter3D->GetPosition(); }
	D3DXVECTOR3 GetModelPos(const int nIdx) { return math::GetPositionFromMatrix(m_apModel[nIdx]->GetMatrixWorld()); }
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff,const float jump);
	bool IsParry(const D3DXVECTOR3 pos);
	CPlayerMotionController* GetMotionController(void) { return m_pMotion.get(); } // モーションコントローラーの取得

private:

	std::unique_ptr<CCharacter3D> m_pCharacter3D;		// キャラクタークラス
	std::unique_ptr<CCollisionFOV> m_pFOV;				// 視界の判定
	std::unique_ptr<CCollisionSphere> m_pSphere;		// 円の当たり判定
	std::unique_ptr<CPlayerMotionController> m_pMotion;	// プレイヤーのモーション制御のクラスのポインタ
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

//***************************************************
// プレイヤーのモーションの制御クラスの定義
//***************************************************
class CPlayerMotionController
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
		TYPE_MAX
	}TYPE;

	CPlayerMotionController();
	~CPlayerMotionController();

	void Load(std::vector<CModel*> &pModel,int *pOutNumModel); // モーションのロード
	void Uninit(void);
	void Update(CModel**ppModel,const int nNumModel);

	// ロードできたかどうか
	bool IsLoad(void) const;
	void SetMotion(const int type, bool bBlend, const int nFrameBlend);

	bool IsParryEvent(const int start, const int end);
	int GetBlendType(void) const;
private:

	// モーションの遷移
	void TransitionMotion(void);
	std::unique_ptr<CMotion> m_pMotion; // モーションのクラスへのポインタ
};
#endif