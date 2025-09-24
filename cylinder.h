//================================================
//
// メッシュシリンダー [cylinder.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _CYLINDER_H_
#define _CYLINDER_H_

//************************************************
// インクルードファイル
//************************************************
#include"object.h"

//************************************************
// メッシュフィールドクラスの定義
//************************************************
class CMeshCylinder : public CObject
{
public:

	// シリンダーの種類の設定
	typedef enum
	{
		TYPE_WALL = 0, // 壁
		TYPE_VIEW,	   // 判定可視化用
		TYPE_MAX
	}TYPE;

	CMeshCylinder(int nPriority = 5);
	~CMeshCylinder();

	static CMeshCylinder* Create(const D3DXVECTOR3 pos, const int nSegH, const int nSegV,const float fRadius, const float fHeight, const D3DXVECTOR3 rot = Const::VEC3_NULL);
	void Set(const TYPE type) { m_Type = type; }

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetCylinder(const int nSegH, const int nSegV, const float fRadius, const float fHeight);
	bool Collision(D3DXVECTOR3* pPos);
	void SetTextureID(const char* pTextureName);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;	// インデックスバッファへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	D3DXCOLOR m_col;						// 色
	TYPE m_Type;							// 種類
	float m_fRadius;						// 半径
	float m_fHeight;						// 高さ
	int m_nSegH;							// 横の分割数
	int m_nSegV;							// 縦の分割数
	int m_nNumVtx;							// 頂点の総数
	int m_nNumPolygon;						// ポリゴン数
	int m_nNumIdx;							// インデックス数
	int m_nTextureIdx;						// テクスチャのID
};
#endif