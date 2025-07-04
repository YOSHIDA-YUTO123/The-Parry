//===================================================
//
// モデル [model.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MODEL_H_
#define _MODEL_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// モデルクラスの定義
//***************************************************
class CModel
{
public:
	CModel();
	~CModel();
	static CModel* Create(const char* pModelName);
	static int LoadCharctorSet(const char* pLoadFileName, CModel** pModel,const int nMaxSize);

	HRESULT Init(const char* pModelName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawShadow(void);

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetRotaition(void) const { return m_rot; }
	D3DXVECTOR3 GetOffPos(void) const { return m_offpos; }
	D3DXVECTOR3 GetOffRot(void) const { return m_offrot; }

	D3DXVECTOR3 GetSize(void) const { return m_Size; }

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRotaition(const D3DXVECTOR3 rot) { m_rot = rot; }
	void SetOffPos(const D3DXVECTOR3 pos) { m_offpos = pos; }
	void SetOffRot(const D3DXVECTOR3 rot) { m_offrot = rot; }

	void SetParent(CModel* pModel) { m_pParent = pModel; }
	D3DXMATRIX GetMatrixWorld(void) const { return m_mtxWorld; }
private:
	D3DXVECTOR3 m_Size;			// 大きさ
	D3DXVECTOR3 m_offpos;			// 基準の位置
	D3DXVECTOR3 m_offrot;			// 基準の向き
	D3DXVECTOR3 m_pos;				// 位置
	D3DXVECTOR3 m_rot;				// 向き
	D3DXMATRIX m_mtxWorld;			// ワールドマトリックス
	CModel* m_pParent;				// 親モデルへのポインタ
	int m_nModelIdx;				// モデルのインデックス
	int* m_pTextureIdx;				// テクスチャのインデックス
};
#endif