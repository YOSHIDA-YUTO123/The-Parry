//================================================
//
// 軌跡 [orbit.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _ORBIT_H_
#define _ORBIT_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include"mesh.h"

//************************************************
// 軌跡のクラスの定義
//************************************************
class CMeshOrbit : public CMesh
{
public:
	CMeshOrbit();
	~CMeshOrbit();

	static CMeshOrbit* Create(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom, const int nSegH, const D3DXCOLOR col);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom);
private:
	D3DXVECTOR3 m_Top;		// 上の位置
	D3DXVECTOR3 m_Bottom;	// 下の位置
	D3DXCOLOR m_col;		// 色
};

#endif
