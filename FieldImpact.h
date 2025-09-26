//================================================
//
// フィールドの衝撃波の処理をするクラス [FieldImpact.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _FIELDIMPACT_H_
#define _FIELDIMPACT_H_

//************************************************
// インクルードファイル
//************************************************
#include "main.h"
#include <memory>

//************************************************
// 前方宣言
//************************************************
class CVelocity;
class CMeshField;
class CColliderSphere;
class CMeshFieldImpact;

//************************************************
// メッシュフィールドのインパクトクラスの定義
//************************************************
class CMeshFieldImpact
{
public:

	// アタッカーの種類
	typedef enum
	{
		OBJ_NONE = 0,
		OBJ_PLAYER,
		OBJ_ENEMY,
		OBJ_MAX
	}OBJ;

	// 衝撃波の構成(位置、発射位置、方向、サークルの色、放ったオブジェクト、当たり判定の半径、高さ、速さ、発生時間)
	struct Config
	{
		D3DXVECTOR3 pos;	     // 位置
		D3DXVECTOR3 FirstPos;	 // 最初の位置
		D3DXVECTOR3 dir;		 // 方向
		D3DXCOLOR Circlecol;	 // 色
		OBJ ObjType;			 // 誰が放った衝撃波か
		float fRadius;	         // 半径
		float fHeight;	         // 高さ
		float fSpeed;			 // 速さ
		int nTime;		         // 時間
	};

	CMeshFieldImpact();
	~CMeshFieldImpact();

	static CMeshFieldImpact* Create(Config config);
	void Uninit(void);
	bool Update(CMeshField* pMeshField, const int nNumVtx);
	bool Collision(const D3DXVECTOR3 pos, const float fRadius, const OBJ myObj, D3DXVECTOR3* pFirstPos = nullptr, D3DXVECTOR3* pImpactPos = nullptr);
	D3DXVECTOR3 GetPosition(void) const { return m_Config.pos; }
	D3DXVECTOR3 GetFirstPos(void) const { return m_Config.FirstPos; }
	void Reset(D3DXVECTOR3 dir, const OBJ obj, const D3DXVECTOR3 FirstPos, const D3DXCOLOR Circlecol);
private:

	// 衝撃波の情報()
	struct Info
	{
		int nCounter;		         // カウンター
	};

	Config m_Config;			 // 構成
	Info m_Info;				 // 情報

	std::unique_ptr<CColliderSphere> m_pSphere; // 円のコライダー
	CVelocity* m_pMove;							// 移動量クラスへのポインタ
};

#endif