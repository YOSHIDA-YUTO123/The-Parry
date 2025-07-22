//===================================================
//
// ビルボードの描画 [billboard.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 4);
	virtual ~CObjectBillboard();

	static CObjectBillboard* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const char* pTextureName);

	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR2 GetSize(void) const { return m_Size; }
	
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetSize(const D3DXVECTOR2 size) { m_Size = size; }

	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName);
	void UpdateVertexPos(const D3DXVECTOR3 pos);

private:
	D3DXVECTOR3 m_pos;						// 位置のクラスのポインタ
	D3DXVECTOR2 m_Size;						// 大きさ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	int m_nTextureIdx;						// テクスチャのインデックス
};
#endif