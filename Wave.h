//================================================
//
// メッシュウェーブ [wave.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _WAVE_H_
#define _WAVE_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"

//************************************************
// メッシュウェーブクラスの定義
//************************************************
class CMeshWave : public CMesh
{
public:
	CMeshWave();
	~CMeshWave();

	static CMeshWave* Create(const D3DXVECTOR3 pos,const float fRadius,const float fHeight,const D3DXCOLOR col, const int nSegH = 32);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetWave(const int nLife, const float fSpeed);
private:
	void SetVtx(const int nSegH, const float fRadius, const float fHeight); // 頂点の設定
	D3DXCOLOR m_col;	// 色
	float m_fSpeed;		// 速さ
	float m_fHeight;	// 高さ
	float m_fRadius;	// 半径
	float m_fDecAlv;	// α値の減少スピード
	int m_nLife;		// 寿命
};

#endif