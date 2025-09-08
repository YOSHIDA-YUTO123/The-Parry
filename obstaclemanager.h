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

//**********************************************
// 障害物のマネージャークラスの定義
//**********************************************
class CObstacleManager
{
public:
	static void Create(void);
	static CObstacleManager* GetInstance(void);
	std::list <CObstacle*>::iterator Begin(void);
	std::list <CObstacle*>::iterator End(void);
	std::list <CObstacle*>::iterator Erase(std::list<CObstacle*>::iterator itr);

	void Destroy(CObstacle* pObstacle);
	void Clear(CObstacle* pObstacle);
	void AddObstacle(CObstacle *pObstacle);
	void Uninit(void);
	int GetObstacleSize(void);
private:
	CObstacleManager();
	~CObstacleManager();
	static CObstacleManager* m_pManager;			// 障害物マネージャーへのポインタ
	static std::list <CObstacle*> m_apObstacleList;	// 障害物へのポインタ
};


#endif
