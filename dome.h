//================================================
//
// メッシュドーム [dome.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _DOME_H_
#define _DOME_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"

//************************************************
// メッシュドームクラスの定義
//************************************************
class CMeshDome : public CMesh
{
public:
	CMeshDome();
	~CMeshDome();

	static CMeshDome* Create(const D3DXVECTOR3 pos, const int nSegH, const int nSegV, const float fRadius, const float fHeight, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetDome(const int nSegH, const int nSegV, const float fRadius, const float fHeight);
private:
	int m_nOffsetIdx;
};
#endif