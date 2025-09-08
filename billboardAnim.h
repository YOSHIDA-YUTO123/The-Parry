//===================================================
//
// ビルボードアニメーションの描画 [billboardAnim.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BILLBOARDANIM_H_
#define _BILLBOARDANIM_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// ビルボード(アニメーション)クラスの定義
//***************************************************
class CBillboardAnimation : public CObject
{
public:
	CBillboardAnimation(int nPriority = 4);
	virtual ~CBillboardAnimation();

	static CBillboardAnimation* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const int nSpeed,const int U,const int V,const bool bLoop);
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR2 GetSize(void) const { return m_Size; }

	void SetAnim(const int nAnimSpeed, const int U, const int V, const bool bLoop);
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const D3DXVECTOR2 size) { m_Size = size; }

	void SetTextureVtx(const D3DXVECTOR2 tex, const D3DXVECTOR2 offtex);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName);
	void UpdateVertexPos(const D3DXVECTOR3 pos);

private:
	D3DXVECTOR3 m_pos;						// 位置のクラスのポインタ
	D3DXVECTOR2 m_Size;						// 大きさ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	float m_fPosTexU;						// テクスチャ座標UV
	float m_fPosTexH;						// テクスチャ座標HV
	int m_nAnimSpeed;						// アニメーションの速さ
	int m_nPatternAnim;						// アニメーションのパターン
	int m_nCounterAnim;						// アニメーションのカウンター
	int m_nH, m_nV;							// テクスチャのUVの分割数
	int m_nTextureIdx;						// テクスチャのインデックス
	bool m_bLoop;							// ループするかどうか
};

#endif