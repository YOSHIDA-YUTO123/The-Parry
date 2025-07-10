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
#include<vector>
#include<memory>

class CObstacle;

//**********************************************
// 障害物のマネージャークラスの定義
//**********************************************
class CObstacleManager
{
public:
	static void Create(void);
	static CObstacleManager* GetInstance(void);
	CObstacle* GetObstacle(const int nIdx);
	void AddObstacle(CObstacle *pObstacle);
	void Uninit(void);
	int GetObstacleSize(void);
private:
	CObstacleManager();
	~CObstacleManager();
	static CObstacleManager* m_pManager; // 障害物マネージャーへのポインタ
	static std::vector<CObstacle*> m_apObstacle;		  // 障害物へのポインタ
};


#endif
