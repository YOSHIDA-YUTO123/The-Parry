//===================================================
//
// オブジェクト2Dの描画処理 [object2D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT2D_H_
#define _OBJECT2D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// オブジェクト2Dクラスの定義
//***************************************************
class CObject2D : public CObject
{
public:

	CObject2D(int nPriority = 7);
	virtual ~CObject2D();

	static CObject2D* Create(const float fWidth, const float fHeight, const D3DXVECTOR3 pos,const D3DXVECTOR3 rot = Const::VEC3_NULL);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR2 GetSize(void) const { return m_Size; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }

	void SetVtx(const D3DXCOLOR col);
	void SetSize(const D3DXVECTOR2 Size) { m_Size = Size; }
	void SetSize(const float fWidth, const float fHeight);
	void SetSize(const float leftWidth,const float rightWdth,const float topHeight,const float buttom);

	void UpdateVertex(void);
	
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName = NULL);

private:

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR2 m_Size;						// 大きさ
	float m_fAngle;							// 角度
	float m_Length;							// 長さ
	int m_nTextureIdx;						// テクスチャのインデックス
};

#endif