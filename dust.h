//===================================================
//
// 瓦礫 [dust.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _DUST_H_
#define _DUST_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "main.h"
#include "object.h"
#include"transform.h"
#include"objectX.h"
#include"shadow.h"

//***************************************************
// 瓦礫クラスの定義
//***************************************************
class CRubble : public CObject
{
public:

	// 瓦礫の種類
	typedef enum
	{
		TYPE_ONE = 0,
		TYPE_TWO,
		TYPE_THREE,
		TYPE_MAX
	}TYPE;

	CRubble(int nPriority = 3);
	~CRubble();

	static CRubble* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 dir, const int nLife);
	static void Creates(const int nNumDust, const D3DXVECTOR3 move, const D3DXVECTOR3 pos, const int nLife);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	CObjectX* m_pObjectX; // オブジェクトXへのポインタ
	CShadow* m_pShadow;	  // 影クラスの定義
	CPosition m_pos;	  // 位置クラス
	CRotation m_rot;	  // 向きクラス
	CVelocity m_move;	  // 移動クラス

	float m_fDecAlv;	  // アルファ値の減少スピード
	int m_nLife;		  // 寿命
	int m_nMaxLife;		  // 最大のHP
};

#endif

