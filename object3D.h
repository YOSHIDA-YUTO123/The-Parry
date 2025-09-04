//===================================================
//
// 3Dオブジェクトの描画 [object3D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CObject3D : public CObject
{
public:
	CObject3D(int nPriority = 3);
	~CObject3D();

	static CObject3D* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 size, const char* pTextureName);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetDraw(void);

	CPosition* GetPosition(void);
	CRotation* GetRotaition(void);
	CSize3D* GetSize(void);

	void SetOffsetVtx(const D3DXCOLOR col = Const::WHITE, const int nPosX = 1, const int nPosY = 1);
	void SetTextureID(const char* pTextureName);
	void UpdatePosition(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size);
	void UpdateCol(const D3DXCOLOR col);

private:
	CPosition *m_pPos;	// 位置
	CRotation *m_pRot;	// 向き
	CSize3D *m_pSize;		// 大きさ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	int m_nTextureIdx;						// テクスチャのインデックス
};
#endif