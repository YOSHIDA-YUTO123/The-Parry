//================================================
//
// 練習用の敵 [TrainingEnemy.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _TRAININGENEMY_H_
#define _TRAININGENEMY_H_

//************************************************
// インクルードファイル
//************************************************
#include "character3D.h"
#include<memory>

//************************************************
// 前方宣言
//************************************************
class CColliderCapsule;
class CStateMachine;
class CTrainingEnemyState;
class CMeshOrbit;

//************************************************
// 練習用の敵のクラスの定義
//************************************************
class CTrainingEnemy : public CCharacter3D
{
public:

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

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_NEUTRAL = 0, // ニュートラル
		MOTIONTYPE_ACTION,		// 攻撃
		MOTIONTYPE_HIT,			// ヒット
		MOTIONTYPE_DAMAGE,		// ダメージ
		MOTIONTYPE_DAMAGES,		// 中ダメージ
		MOTIONTYPE_DAMAGELS,	// 小ダメージ
		MOTIONTYPE_COMB_DAMAGE,	// 連続ダメージ
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

	CTrainingEnemy();
	~CTrainingEnemy();

	static CTrainingEnemy* Create(const D3DXVECTOR3 pos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Orbit(const int nSegH, const D3DXCOLOR col);					// 軌跡の設定
	void DeleteOrbit(void);												// 軌跡の消去

	// 範囲内にいるか
	bool CheckDistance(float fRange);
	void ChangeState(std::shared_ptr<CTrainingEnemyState> pNewState);
	RESULT GetAttackResult(void);
	void CollisionPlayerAttack(void);
	void SelectDamageMotion(int success, const D3DXVECTOR3 ImpactPos);
private:
	bool CollisionPlayer(void);
	bool IsDamage(CMotion* pMotion);
	std::unique_ptr<CColliderCapsule> m_pCapsule; // カプセルコライダー
	std::unique_ptr<CStateMachine> m_pMachine;	  // 状態マシーン
	D3DXMATRIX m_SwordMtx;						  // 剣のマトリックス
	CMeshOrbit* m_pOrbit;							// 軌跡
};

#endif