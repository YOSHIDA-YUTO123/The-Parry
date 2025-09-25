//================================================
//
// 軌跡 [orbit.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _ORBIT_H_
#define _ORBIT_H_

//************************************************
// インクルードファイル
//************************************************
#include"object.h"

//************************************************
// 軌跡のクラスの定義
//************************************************
class CMeshOrbit : public CObject
{
public:
	CMeshOrbit();
	~CMeshOrbit();

	static CMeshOrbit* Create(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom, const int nSegH, const D3DXCOLOR col);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom);
	void SetTextureID(const char* pTextureName);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3 m_Top;						// 上の位置
	D3DXVECTOR3 m_Bottom;					// 下の位置
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXCOLOR m_col;						// 色
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	int m_nSegH;							// 横の分割数
	int m_nSegV;							// 縦の分割数
	int m_nNumVtx;							// 頂点の総数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIdx;							// インデックス数
	int m_nTextureIdx;						// テクスチャのID
};

#endif
