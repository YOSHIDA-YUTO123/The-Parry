//==============================================
//
// 障害物 [obstacle.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

//**********************************************
// インクルードファイル
//**********************************************
#include"main.h"
#include "object.h"
#include<memory>

//**********************************************
// 前方宣言
//**********************************************
class CObjectX;
class CVelocity;

//**********************************************
// 障害物のクラスの定義
//**********************************************
class CObstacle : public CObject
{
public:
	CObstacle();
	~CObstacle();
	static CObstacle* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	CObjectX* m_pObjectX;				  // Xファイルモデルクラスのポインタ
	std::unique_ptr<CVelocity> m_pMove;	  // 移動量
};

#endif
