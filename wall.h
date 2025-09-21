//===================================================
//
// 3Dポリゴンの壁を設置 [wall.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _WALL_H_
#define _WALL_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"

//***************************************************
// 壁クラスの定義
//***************************************************
class CWall : public CObject
{
public:
	CWall();
	~CWall();

	static CWall* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size, const D3DXVECTOR3 rot);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer; // 頂点バッファへのポインタ
	D3DXMATRIX m_mtxWorld;				  // ワールドマトリックス
	D3DXVECTOR3 m_pos;					  // 位置
	D3DXVECTOR3 m_rot;					  // 向き
	D3DXVECTOR3 m_Size;					  // 大きさ
	int m_nTextureIdx;					  // テクスチャのID
};

#endif