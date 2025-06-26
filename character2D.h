//===================================================
//
// キャラクター [character.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _CHARACTER2D_H_
#define _CHARACTER2D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object2D.h"

//***************************************************
// キャラクタークラスの定義
//***************************************************
class CCharacter2D : public CObject2D
{
public:
	CCharacter2D(int nPriority = 5);
	virtual ~CCharacter2D();

	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetTexture(const char* pFilePath);
	void Flash(const int nCount, const float fSpeed);
	bool HitCharacter(const int nDamage);

private:
	D3DXCOLOR m_col;	// 色
	int m_nIdxTexture;	// テクスチャのインデックス
	int m_nLife;		// 体力
};

//***************************************************
// アニメーションするキャラクターのクラスの定義
//***************************************************
class CAnimCharacter2D : public CCharacter2D
{
public:
	CAnimCharacter2D();
	~CAnimCharacter2D();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTexPos(const int nPosX, const int nPosY);
	void SetAnimationSpeed(const int nAnimSpeed = 1) { m_nAnimSpeed = nAnimSpeed; }
	void SetTexDir(const int nOffTexPosH) { m_nOffTexPosH = nOffTexPosH; }
	void StopAnim(const int nPattern) { m_nPatternAnim = nPattern; }

private:
	float m_fPosTexU;		// テクスチャ座標UV
	float m_fPosTexH;		// テクスチャ座標HV
	int m_nOffTexPosH;		// テクスチャ座標のオフセットHV
	int m_nAnimSpeed;		// アニメーションの速さ
	int m_nPatternAnim;		// アニメーションのパターン
	int m_nCounterAnim;		// アニメーションのカウンター
	int m_nPosX;			// テクスチャの分割数X
	int m_nPosY;			// テクスチャの分割数Y
};

#endif