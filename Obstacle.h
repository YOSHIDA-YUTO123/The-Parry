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
#include "objectX.h"
#include<memory>

//**********************************************
// 前方宣言
//**********************************************
class CVelocity;
class CCollision;
class CCollisionAABB;
class CColliderAABB;

//**********************************************
// 障害物のクラスの定義
//**********************************************
class CObstacle : public CObjectX
{
public:

	// 障害物の距離
	typedef enum
	{
		TYPE_SPIKE_TRAP = 0, // スパイクトラップ
		TYPE_TNT_BARREL,		// 爆発樽
		TYPE_MAX
	}TYPE;

	CObstacle(const TYPE type);
	virtual ~CObstacle();

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	virtual bool Collision(CColliderAABB* pCollider,D3DXVECTOR3 *pushPos) = 0;
	TYPE GetType(void) const { return m_type; }
protected:
	CColliderAABB* GetCollider(void) { return m_pAABB.get(); }
	void CreateCollider(void);
	CVelocity* GetMove(void) { return m_pMove.get(); }
private:
	std::unique_ptr<CVelocity> m_pMove;		 // 移動量
	std::unique_ptr<CColliderAABB> m_pAABB;  // 矩形のコライダー
	TYPE m_type;							 // 種類
	D3DXVECTOR3 m_posOld;					 // 前回の位置
	D3DXVECTOR3 m_CenterPos;				 // 真ん中の位置
};

//**********************************************
// スパイクトラップクラスの定義
//**********************************************
class CSpikeTrap : public CObstacle
{
public:
	CSpikeTrap();
	~CSpikeTrap();

	static CSpikeTrap* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,const int nDamageFace);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Collision(CColliderAABB* pCollider, D3DXVECTOR3* pushPos) override;
private:
	int m_nDamageFace;						 // ダメージを食らう面
};

//**********************************************
// 爆発樽クラスの定義
//**********************************************
class CTNTBarrel : public CObstacle
{
public:
	CTNTBarrel();
	~CTNTBarrel();

	static CTNTBarrel* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 DestPos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Collision(CColliderAABB* pCollider, D3DXVECTOR3* pushPos) override;
private:
	void SetQuaternion(void); // クォータニオンの設定処理
	D3DXQUATERNION m_quat;	 // クォータニオン
	D3DXMATRIX m_mtxRot;	 // 回転行列
	D3DXVECTOR3 m_axis;		 // 回転軸
	float m_fDirDest;		 // 目的の移動方向
	float m_fDir;			 // 移動方向
	float m_fCircumference;  // 半径
	float m_fDestValueRot;	 // 目的の回転量
	float m_fValueRot;		 // 回転量
};

#endif
