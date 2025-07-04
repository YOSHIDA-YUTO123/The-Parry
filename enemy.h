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

//***************************************************
// 敵のAIクラス
//***************************************************
class CEnemyAI
{
public:

	// 敵の行動の種類
	typedef enum
	{
		ACTION_IDLE = 0, // 何もしない
		ACTION_MOVE,	 // 移動
		ACTION_ATTACK,	 // 攻撃
		ACTION_WAIT,	 // 待機
		ACTION_MAX
	}ACTION;

	CEnemyAI();
	~CEnemyAI();
	void Init(std::shared_ptr<CEnemyMotionController> pMotion, const int nBlendFrame = 10, const int nFirstMotion = 0);
	void Update(void);
	int CheckDistance(const D3DXVECTOR3 dest,const D3DXVECTOR3 pos, const float fRadius);
	int IsAttack(void);
	bool Wait(void);
private:
	std::shared_ptr<CEnemyMotionController> m_pMotion;	// 敵のモーションの制御クラスのポインタ
	ACTION m_Action;			// 敵の行動パターン
	int m_nNextCounter;			// 次の行動の抽選カウンター
	int m_nCounterAction;		// 行動のカウンター
};

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
private:
	void SetParent(const int nCnt);

	std::unique_ptr<CCharacter3D> m_pCharactor;			// キャラクタークラス
	std::unique_ptr<CCollisionSphere> m_pSphere;		// 円の当たり判定クラス
	std::unique_ptr<CShadow> m_pShadow;					// 影のクラスへのポインタ
	std::unique_ptr<CEnemyAI> m_pAI;					// 敵のAI
	std::shared_ptr<CEnemyMotionController> m_pMotion;	// 敵のモーションの制御クラスのポインタ
	std::vector<CModel*> m_apModel;						// モデルクラスへのポインタ
	std::shared_ptr<CVelocity> m_pMove;					// 移動クラスの生成
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
		TYPE_ACTION,
		TYPE_JUMP,
		TYPE_LANDING,
		TYPE_DAMAGE,
		TYPE_ACTION2,
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

	// モーションの遷移
	void TransitionMotion(CCharacter3D *pCharacter);
	void ConfigMove(CCharacter3D* pCharacter,const D3DXVECTOR3 dest);
private:

	std::unique_ptr<CMotion> m_pMotion;	// モーションのクラスへのポインタ
	std::shared_ptr<CVelocity> m_pMove;	// 移動クラスの生成
};
#endif