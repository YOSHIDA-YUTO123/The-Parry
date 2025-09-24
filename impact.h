//================================================
//
// メッシュインパクト [impact.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _IMPACT_H_
#define _IMPACT_H_

//************************************************
// インクルードファイル
//************************************************
#include"object.h"

//************************************************
// メッシュサークル(平面)クラスの定義
//************************************************
class CMeshCircle : public CObject
{
public:

	CMeshCircle(const int nPriority = 4);
	~CMeshCircle();

	static CMeshCircle* Create(const D3DXCOLOR col, const D3DXVECTOR3 pos, const float fInRadius, const float fOutRadius, const int nSegH = 32);
	void SetCircle(const float fHeight, const float fSpeed, const int nLife, const bool bField, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTextureID(const char* pTextureName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	D3DXCOLOR m_Incol, m_Outcol;			// 色
	float m_fHeight;						// 高さ
	float m_fSpeed;							// 速さ
	float m_fInRadius;						// 内側の半径
	float m_fOutRadius;						// 外側の半径
	float m_fDecAlv;						// α値の減少スピード
	int m_nSegH;							// 横の分割数
	int m_nSegV;							// 縦の分割数
	int m_nNumVtx;							// 頂点の総数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIdx;							// インデックス数
	int m_nTextureIdx;						// テクスチャのID
	int m_nLife;							// 寿命
	bool m_bFiledCollision;					// 地面の高さに合わせるかどうか
};
#endif