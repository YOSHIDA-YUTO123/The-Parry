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

	// 構成情報(色、速さ、高さ、半径、寿命)
	struct Config
	{
		D3DXCOLOR col;	// 色
		float fSpeed;	// 速さ
		float fHeight;	// 高さ
		float fRadius;	// 半径
		int nLife;		// 寿命
	};

	CMeshWave();
	~CMeshWave();

	static CMeshWave* Create(Config config ,const D3DXVECTOR3 pos, const int nSegH = 32, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetWave(const int nSegH, const float fRadius, const float fHeight); // 頂点の設定
private:
	Config m_Config;	// 波の構成
	float m_fDecAlv;	// α値の減少スピード
};

#endif