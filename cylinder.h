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
#include"mesh.h"

//************************************************
// メッシュフィールドクラスの定義
//************************************************
class CMeshCylinder : public CMesh
{
public:
	CMeshCylinder(int nPriority = 7);
	~CMeshCylinder();

	static CMeshCylinder* Create(const D3DXVECTOR3 pos, const int nSegX, const int nSegZ,const float fRadius, const float fHeight, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetCylinder(const int nSegX, const int nSegZ, const float fRadius, const float fHeight);
	bool Collision(D3DXVECTOR3* pPos);
private:
	D3DXVECTOR3 m_CenterPos; // 円柱の中心の座標
	float m_fRadius;		 // 半径
};
#endif