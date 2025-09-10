//===================================================
//
// 爆発,煙の描画 [explosion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"billboardAnim.h"

//***************************************************
// 爆発、煙クラスの定義
//***************************************************
class CExplosion : public CBillboardAnimation
{
public:

	// 種類
	typedef enum
	{
		TYPE_SMOKE = 0,
		TYPE_EXPLOSION,
		TYPE_FIRE_EXPLOSION,
		TYPE_FIRE,
		TYPE_MAX
	}TYPE;

	CExplosion(int nPriority = 4);
	~CExplosion();

	static CExplosion* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed, const TYPE type);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetParam(const D3DXVECTOR3 move);
private:
	TYPE m_type;		// 種類
	D3DXVECTOR3 m_move;	// 移動量
	D3DXCOLOR m_col;	// 色
	float m_fDestMoveY; // 目的の移動量
};

//***************************************************
// 爆発のマネージャークラスの定義
//***************************************************
class CExplosionManager
{
public:
	~CExplosionManager();

	static CExplosionManager* SetParam(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed);
	void Create(const CExplosion::TYPE type, const D3DXVECTOR2 move, const int nNum);
private:
	CExplosionManager();
	D3DXVECTOR3 m_pos; // 位置
	D3DXVECTOR2 m_Size; // 大きさ
	D3DXCOLOR m_col;	// 色
	int m_nUV, m_nHV;	// 縦、横の分割
	int m_nAnimSpeed;		// アニメーションの速度
	static CExplosionManager* m_pInstance; // 自分のインスタンス
};
#endif