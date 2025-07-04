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

	// サークルの構成(高さ、速さ、半径内側、半径外側、寿命、地面判定)
	struct Confing
	{
		float fHeight;			 // 高さ
		float fSpeed;			 // 速さ
		float fInRadius;		 // 内側の半径
		float fOutRadius;		 // 外側の半径
		int nLife;				 // 寿命
		bool bFiledCollision;	 // 地面の高さに合わせるかどうか
	};

	CMeshCircle();
	~CMeshCircle();

	static CMeshCircle* Create(const Confing confing,const D3DXCOLOR col, const D3DXVECTOR3 pos,const int nSegX = 32, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	void SetCircle(const int nSegX, const float InRadius, const float OutRadius);  // 頂点の設定
	Confing m_Config;	// 構成
	D3DXCOLOR m_Incol, m_Outcol; // 色
	float m_fDecAlv;	// α値の減少スピード
};
#endif