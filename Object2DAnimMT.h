//===================================================
//
// マルチテクスチャ2Dオブジェクトのアニメーション [Object2DAnimMT.h]
// Author:YUTO YOSHIDA
// 
// << マルチテクスチャの2Dポリゴンのテクスチャ	>>
// << アニメーションに特化したクラス			>>
// 
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT2DANIMMT_H_
#define _OBJECT2DANIMMT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"

//***************************************************
// マルチテクスチャ2Dアニメーションクラスの定義
//***************************************************
class CObject2DAnimMT : public CObject
{
public:
	CObject2DAnimMT();
	~CObject2DAnimMT();

	static CObject2DAnimMT* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int UV, const int HV);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	void SetAnim(const int nSpeed,const bool bLoop);
	void SetAnim(const int nUV,const int nHV);
	void SetTextureID(const char* pTextureName0, const char* pTextureName1);

	// ゲッター
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	
	// セッター
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const D3DXVECTOR2 Size) { m_Size = Size; }

private:

	static constexpr int MAX_TEXTURE = 2;	// テクスチャの数

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR2 m_Size;						// 大きさ
	int m_nPattern;							// パータン
	int m_nCounter;							// カウンター
	int m_nUV;								// テクスチャの分割横
	int m_nHV;								// テクスチャの分割縦
	int m_nSpeed;							// アニメーションスピード
	int m_nTextureIdx[MAX_TEXTURE];			// テクスチャのID
	bool m_bLoop;							// ループするかどうか
};

#endif
