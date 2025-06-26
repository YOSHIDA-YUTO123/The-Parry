//================================================
//
// メッシュフィールド [meshfield.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//************************************************
// インクルードファイル
//************************************************
#include"mesh.h"
#include<vector>

//************************************************
// メッシュフィールドクラスの定義
//************************************************
class CMeshField : public CMesh
{
public:
	CMeshField();
	~CMeshField();

	static CMeshField* Create(const D3DXVECTOR3 pos,const int nSegX,const int nSegZ,const D3DXVECTOR2 Size, const D3DXVECTOR3 rot = VEC3_NULL);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMeshField(const int nSegX, const int nSegZ, const  D3DXVECTOR3 pos, const D3DXVECTOR2 Size);
	bool Collision(const D3DXVECTOR3 pos, float* pOutHeight);
	void SetWave(const D3DXVECTOR3 epicenter, const int nTime, const float fSpeed, const float fInRadius, const float fOutRadius, const float fWaveHeight, const float fcoef);
	void UpdateNor(void);
	D3DXVECTOR3 GetNor(void) const { return m_Nor; }
private:
	void UpdateWave(const int nNumVtx);

	// 波の情報
	struct Wave
	{
		D3DXVECTOR3 epicenter;
		float fInRadius, fOutRadius;	// 波の半径
		float fHeight;					// 波の高さ
		float fStartHeight;				// 最初の高さ
		float fTime;					// 高さ
		float fSpeed;					// 速さ
		float fcoef;					// 係数
		int nCounter;					// 波のカウンター
		int nTime;						// 波の発生時間
		bool bUse;						// 使用しているかどうか
	};

	D3DXVECTOR3 m_Nor;					// 法線
	Wave m_Wave;						// ウェーブの情報
	float m_fWidth, m_fHeight;			// 横幅,高さ
};
#endif