//==============================================
//
// 障害物マネージャー [obstaclemanager.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _OBSTACLEMANAGER_H_
#define _OBSTACLEMANAGER_H_

//**********************************************
// インクルードファイル
//**********************************************
#include"main.h"
#include<list>
#include<memory>

//**********************************************
// 前方宣言
//**********************************************
class CObstacle;
class CExplosionArea;
class CColliderCapsule;
class CTNTBarrel;

//**********************************************
// 障害物のマネージャークラスの定義
//**********************************************
class CObstacleManager
{
public:
	~CObstacleManager();
	static void Create(void);
	static CObstacleManager* GetInstance(void);
	std::list <CObstacle*>::iterator Begin(void);
	std::list <CObstacle*>::iterator End(void);
	std::list <CObstacle*>::iterator Erase(std::list<CObstacle*>::iterator itr);

	void Destroy(CObstacle* pObstacle);
	void Clear(CObstacle* pObstacle);
	void AddObstacle(CObstacle *pObstacle);
	void AddExplosion(CExplosionArea* pExplosionArea) { m_apExplosionArea.push_back(pExplosionArea); }
	void EraseExplosion(CExplosionArea* pExplosionArea);
	void Uninit(void);
	void Update(void);
	int GetObstacleSize(void);
	bool CollisionExplotion(CColliderCapsule* pCapsule, D3DXVECTOR3* pPos = nullptr);
	bool CollisionImpact(CObstacle* pObstacle);
private:
	static constexpr int NUM_TNTPOP_POINT = 4;		// 爆発樽の出現ポイントの数
	CObstacleManager();
	static CObstacleManager* m_pManager;			// 障害物マネージャーへのポインタ
	static std::list <CObstacle*> m_apObstacleList;	// 障害物へのポインタ
	std::list <CExplosionArea*> m_apExplosionArea;	// 爆発のエリア
	D3DXVECTOR3 m_Point[NUM_TNTPOP_POINT];			// 爆発樽の出現する位置
	int m_nTNTPopPosID;								// 爆発樽の位置のID
	int m_nPopTime;									// ポップする時間
};

#endif
