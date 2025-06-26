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
#include"transform.h"

//***************************************************
// マクロ定義
//***************************************************

//***************************************************
// レンダラークラスの定義
//***************************************************
class CObject2D : public CObject
{
public:

	// オブジェクトの種類
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_BACKGROUND,
		TYPE_PLAYER,
		TYPE_MAX
	}TYPE;

	CObject2D(int nPriority = 3);
	~CObject2D();

	static CObject2D* Create(const float fWidth, const float fHeight, const D3DXVECTOR3 pos);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetSize(const float fWidth, const float fHeight);
	void SetSize(const float leftWidth,const float rightWdth,const float topHeight,const float buttom);
	void SetOffsetVtx(const D3DXCOLOR col = WHITE, const int nPosX = 1, const int nPosY = 1);
	void UpdateVertex(void);
	void SetUvPos(const D3DXVECTOR2 OffPosTex,const D3DXVECTOR2 PosTex);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pTextureName = NULL);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	CPosition *m_pPos;	// 位置
	CRotation *m_pRot;	// 向き
	CSize2D *m_pSize;	// 大きさ
	float m_fAngle;							// 角度
	float m_Length;							// 長さ
	int m_nTextureIdx;						// テクスチャのインデックス
};

#endif