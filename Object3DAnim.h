//===================================================
//
// アニメーション3Dポリゴン [Object3DAnim.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT3DANIM_H_
#define _OBJECT3DANIM_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object.h"

//***************************************************
// アニメーション3Dポリゴンクラスの定義
//***************************************************
class CObject3DAnim : public CObject
{
public:
	CObject3DAnim(const int nPriority = 5);
	virtual ~CObject3DAnim();

	static CObject3DAnim* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size, const int nSegX, const int nSegY, const int nSpeed);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }
	D3DXVECTOR3 GetSize(void) const { return m_Size; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetSize(const D3DXVECTOR3 Size) { m_Size = Size; }
	void SetTextureID(const char* pTextureName);
	void SetParam(const int nSegX, const int nSegY, const int nSpeed,const bool bLoop);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_Size;						// 大きさ
	int m_nSpeed;							// アニメーションのスピード
	int m_nCounter;							// カウンター
	int m_nPattern;							// パターン
	int m_nSegX;							// 横の分割数
	int m_nSegY;							// 縦の分割数
	int m_nTextureIdx;						// テクスチャのインデックス
	bool m_bLoop;							// ループするかどうか
};

#endif
