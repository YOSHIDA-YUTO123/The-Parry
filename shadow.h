//================================================
//
// 影 [shadow.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _SHADOW_H_
#define _SHADOW_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include"object3D.h"
#include<memory>

//*************************************************
// 前方宣言
//*************************************************
class CRotation;
class CSize3D;

//*************************************************
// 影クラスの定義
//*************************************************
class CShadow
{
public:
	CShadow();
	~CShadow();
	static std::unique_ptr<CShadow> Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight,const D3DXCOLOR col);
	HRESULT Init(void);
	void Uninit(void);
	void Update(const D3DXVECTOR3 player, const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fMaxHeight, const float fAlv);
	void Draw(void);

	// 影の設定
	void SetFieldAngle(D3DXVECTOR3 Nor, D3DXVECTOR3 up);
private:
	std::unique_ptr<CRotation> m_pRot; // 向きクラスのポインタ
	std::unique_ptr<CSize3D> m_pSize;  // 大きさクラス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer; // 頂点バッファ
	D3DXVECTOR3 m_pos;				   // 位置
	D3DXMATRIX m_mtxWorld;			   // ワールドマトリックス
	D3DXCOLOR m_col;				   // 色
	int m_nTextureIdx;				   // テクスチャのID
};
#endif