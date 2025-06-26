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

	static CMeshWave* Create(const D3DXVECTOR3 pos, const float fRadius, const float fHeight, const float speed, const int nLife, const int nSegX = 32, const D3DXCOLOR col = WHITE, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetWave(const int nSegX, const float fRadius, const float fHeight); // 頂点の設定
private:
	D3DXCOLOR m_col;	// 色
	int m_nLife;		// 寿命
	float m_fDecAlv;	// α値の減少スピード
	float m_fSpeed;		// 速さ
	float m_fHeight;	// 高さ
	float m_fRadius;	// 半径
};

#endif