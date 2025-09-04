//===================================================
//
// 3Dポリゴン(マルチテクスチャ) [Object3DMT.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT3DMT_H_
#define _OBJECT3DMT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"

//***************************************************
// オブジェクト3DMTクラスの定義
//***************************************************
class CObject3DMT : public CObject
{
public:
	CObject3DMT();
	~CObject3DMT();

	static CObject3DMT* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 Size);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(const D3DXCOLOR col);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }

	void SetTextureID(const char* pTexture0, const char* pTexture1);
private:

	static constexpr int MAX_TEXTURE = 2;	// テクスチャの数
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR3 m_Size;						// 大きさ
	int m_nTextureIdx[MAX_TEXTURE];			// テクスチャのインデックス
};

#endif
