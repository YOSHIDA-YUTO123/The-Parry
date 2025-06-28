//===================================================
//
// オブジェクトXファイル [objectX.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBJECTX_H_
#define _OBJECTX_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//***************************************************
// マクロ定義
//***************************************************
#define MAX_MAT (256) // マテリアルの数

//***************************************************
// オブジェクトXファイルのクラスの定義
//***************************************************
class CObjectX : public CObject
{
public:
	CObjectX(int nPriority = 2);
	~CObjectX();

	static CObjectX* Create(const D3DXVECTOR3 pos, const char* pModelName);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Draw(const float Diffuse); // 透明度を変更できる描画処理

	void DrawShadow(void);

	HRESULT LoadModel(const char* pXFileName);

	CPosition* GetPosition(void) const { return m_pPos; }
	CRotation* GetRotaition(void) const { return m_pRot; }

private:
	CPosition* m_pPos;		// 位置クラスへのポインタ
	CRotation* m_pRot;		// 向きクラスへのポインタ
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	int m_nModelIdx;		// モデルのインデックス
	int *m_pTextureIdx;		// テクスチャのインデックス
};
#endif
