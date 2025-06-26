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
	CObject3D();
	~CObject3D();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDraw(void);

	CPosition* GetPosition(void);
	CRotation* GetRotaition(void);
	CSize3D* GetSize(void);

	//void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	//void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	//void SetSize(const D3DXVECTOR3 size) { GetVector = size; }
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void SetTextureID(const char* pTextureName);
	void UpdatePosition(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size);
	void UpdateCol(const D3DXCOLOR col);

	static CObject3D* Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot,const D3DXVECTOR3 size,const char *pTextureName);
private:
	CPosition *m_pPos;	// 位置
	CRotation *m_pRot;	// 向き
	CSize3D *m_pSize;		// 大きさ
	//D3DXVECTOR3 m_pos;						// 位置
	//D3DXVECTOR3 m_rot;						// 向き
	//D3DXVECTOR3 GetVector;						// 大きさ
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	int m_nTextureIdx;						// テクスチャのインデックス
};
#endif