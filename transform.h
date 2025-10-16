//===================================================
//
// 空間情報の処理をもつクラス [transform.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "main.h"

//***************************************************
// 空間情報クラスの定義
//***************************************************
class CTransform
{
public:

	// 情報
	struct Info
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 posOld;		// 前回の位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 Size;		// 大きさ
		D3DXMATRIX mtxWorld;	// ワールドマトリックス
		float fRadius;			// 半径
	};
	CTransform();
	~CTransform();

	static CTransform* Create(void) { return new CTransform; }
	void UpdatePosition(const D3DXVECTOR3 move);
	void SetMatrix(void);
	Info GetInfo(void) const { return m_Info; }
	void SetInfo(const Info info) { m_Info = info; }

private:
	Info m_Info; // 情報
};
#endif
