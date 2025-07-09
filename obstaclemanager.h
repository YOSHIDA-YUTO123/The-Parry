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

class CObstacle;

//**********************************************
// 障害物のマネージャークラスの定義
//**********************************************
class CObstacleManager
{
public:
	~CObstacleManager();
	static void AddObstacle(CObstacle *pObstacle);
	static CObstacleManager* GetAddress(void);
private:
	CObstacleManager();
	static std::vector<CObstacle*> m_apObstacle; // 障害物へのポインタ
};


#endif
