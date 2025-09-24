//================================================
//
// メッシュドーム [dome.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _DOME_H_
#define _DOME_H_

//************************************************
// インクルードファイル
//************************************************
#include"object.h"

//************************************************
// メッシュドームクラスの定義
//************************************************
class CMeshDome : public CObject
{
public:
	CMeshDome();
	~CMeshDome();

	static CMeshDome* Create(const D3DXVECTOR3 pos, const int nSegH, const int nSegV, const float fRadius, const float fHeight, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDome(const int nSegH, const int nSegV, const float fRadius, const float fHeight);
	void SetTextureID(const char* pTextureName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	float m_fHeight;						// 高さ
	float m_fRadius;						// 半径
	int m_nSegH;							// 横の分割数
	int m_nSegV;							// 縦の分割数
	int m_nNumVtx;							// 頂点の総数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIdx;							// インデックス数
	int m_nTextureIdx;						// テクスチャのID
	int m_nOffsetIdx;						// オフセットのインデックス番号
};
#endif