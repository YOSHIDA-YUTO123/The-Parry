//================================================
//
// メッシュインパクト [impact.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _IMPACT_H_
#define _IMPACT_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"

//************************************************
// メッシュサークル(平面)クラスの定義
//************************************************
class CMeshCircle : public CMesh
{
public:

	CMeshCircle();
	~CMeshCircle();

	static CMeshCircle* Create(const D3DXCOLOR col, const D3DXVECTOR3 pos, const float fInRadius, const float fOutRadius, const int nSegH = 32);
	void SetCircle(const float fHeight, const float fSpeed, const int nLife, const bool bField, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	void SetVtx(const int nSegH, const float InRadius, const float OutRadius);  // 頂点の設定
	float m_fHeight;			 // 高さ
	float m_fSpeed;			 // 速さ
	float m_fInRadius;		 // 内側の半径
	float m_fOutRadius;		 // 外側の半径
	int m_nLife;				 // 寿命
	bool m_bFiledCollision;	 // 地面の高さに合わせるかどうか
	D3DXCOLOR m_Incol, m_Outcol; // 色
	float m_fDecAlv;	// α値の減少スピード
};
#endif