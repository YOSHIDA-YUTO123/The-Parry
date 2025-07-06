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
class CStateIdle;
class CMeshOrbit;

//***************************************************
// 敵クラスの定義
//***************************************************
class CEnemy : public CObject
{
public:

	CEnemy();
	~CEnemy();

	static CEnemy* Create(const D3DXVECTOR3 pos = VEC3_NULL, const D3DXVECTOR3 rot = VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump);
	bool CollisionWepon(void);
	void SetState(std::unique_ptr<CEnemyState> NewState); // 新しい状態の設定処理
	void ChasePlayer(float chaseScal, const float speedScal = 1.0f); // プレイヤーを追いかける処理
	bool CheckDistane(const float fRadius); // 距離の判定
	void MoveForWard(const float fSpeed); // 自分の向いている方向に向かって進む処理
	void Orbit(const int nSegH, const D3DXCOLOR col, const int nLife);
	CEnemyMotionController* GetMotionController(void) { return m_pMotion.get(); }

private:
	void SetParent(const int nCnt);

	std::unique_ptr<CEnemyState> m_pState;				// 敵の状態パターン
	std::unique_ptr<CCharacter3D> m_pCharactor;			// キャラクタークラス
	std::unique_ptr<CCollisionSphere> m_pSphere;		// 円の当たり判定クラス
	std::unique_ptr<CShadow> m_pShadow;					// 影のクラスへのポインタ
	std::shared_ptr<CEnemyMotionController> m_pMotion;	// 敵のモーションの制御クラスのポインタ
	std::vector<CModel*> m_apModel;						// モデルクラスへのポインタ
	std::shared_ptr<CVelocity> m_pMove;					// 移動クラスの生成
	CMeshOrbit* m_pOrbit;								// 軌跡
	D3DXMATRIX m_weponMatrix;							// 武器のワールドマトリックス
	int m_nNumModel;									// モデルの最大数
};

//***************************************************
// 敵のモーションの制御クラスの定義
//***************************************************
class CEnemyMotionController
{
public:

	// モーションの種類
	typedef enum
	{
		TYPE_NEUTRAL = 0,
		TYPE_MOVE,
		TYPE_SMASH,
		TYPE_JUMP,
		TYPE_LANDING,
		TYPE_DAMAGE,
		TYPE_IMPACT,
		TYPE_ROAR,
		TYPE_DASH,
		TYPE_SPIN,
		TYPE_MAX
	}TYPE;

	CEnemyMotionController();
	~CEnemyMotionController();
	void Load(std::vector<CModel*>& pModel, int* pOutNumModel); // モーションのロード

	void Init(std::shared_ptr<CVelocity> pMove);
	void Uninit(void);
	void Update(CModel** ppModel, const int nNumModel);

	// ロードできたかどうか
	bool IsLoad(void) const;
	void SetMotion(const int type, bool bBlend, const int nFrameBlend);
	int GetBlendType(void) const;
	int GetType(void) const;
	bool IsEventFrame(const int start, const int end, const TYPE type);
	bool IsFinishEndBlend(void);
	bool IsFinishMotion(void);

	// モーションの遷移
	void TransitionMotion(CCharacter3D *pCharacter);
private:

	std::unique_ptr<CMotion> m_pMotion;	// モーションのクラスへのポインタ
	std::shared_ptr<CVelocity> m_pMove;	// 移動クラスの生成
};
#endif