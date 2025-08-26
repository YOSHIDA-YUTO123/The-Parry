//===================================================
//
// ライト [light.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _LIGHT_H_
#define _LIGHT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include<vector>

//***************************************************
// ライトのクラスの定義
//***************************************************
class CLight
{
public:
	CLight();
	~CLight();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void SetDirectional(const D3DXCOLOR Diffuse, const D3DXVECTOR3 dir, const D3DXVECTOR3 pos); // ディレクショナルライトの設定処理
	void SetPoint(const D3DXVECTOR3 pos,const float fRange,const D3DXCOLOR Diffuse,const D3DXCOLOR Ambient);			// ポイントライトの設定処理
	void SetLight(void);

	D3DXVECTOR3 GetDir(void) const { return m_aLightInfo[0].aLight.Direction; }
	void DeleteLight(void); // ディレクションライト以外を消去
private:

	static constexpr int MAX_LIGHT = 8; // ライトの最大数

	// ライトの情報
	struct LightInfo
	{
		D3DLIGHT9 aLight;	// ライトの構造体
		bool bUse;			// 使用状態
	};

	LightInfo m_aLightInfo[MAX_LIGHT]; // ライトの構造体
};
#endif