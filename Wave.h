//================================================
//
// メッシュウェーブ [wave.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _WAVE_H_
#define _WAVE_H_

//************************************************
// インクルードファイル
//************************************************
#include"object.h"

//************************************************
// メッシュウェーブクラスの定義
//************************************************
class CMeshWave : public CObject
{
public:
	CMeshWave();
	~CMeshWave();

	static CMeshWave* Create(const D3DXVECTOR3 pos,const float fRadius,const float fHeight,const D3DXCOLOR col, const int nSegH = 32);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetWave(const int nLife, const float fSpeed);
	void SetTextureID(const char* pTextureName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXCOLOR m_col;						// 色
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	float m_fSpeed;							// 速さ
	float m_fHeight;						// 高さ
	float m_fRadius;						// 半径
	float m_fDecAlv;						// α値の減少スピード
	int m_nSegH;							// 横の分割数
	int m_nSegV;							// 縦の分割数
	int m_nNumVtx;							// 頂点の総数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIdx;							// インデックス数
	int m_nTextureIdx;						// テクスチャのID
	int m_nLife;							// 寿命
};

#endif