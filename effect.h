//===================================================
//
// エフェクト [effect.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _EFFECT_H_
#define _EFFECT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"billboard.h"
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CVelocity;

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CEffect3D : public CObjectBillboard
{
public:

	// エフェクトの種類
	enum TYPE
	{
		TYPE_NORAML = 0,
		TYPE_HIT,
		TYPE_MAX
	};

	CEffect3D(int nPriority = 4);
	~CEffect3D();

	static CEffect3D* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col,const TYPE type);
	void SetEffect(const int nLife, const D3DXVECTOR3 move);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CVelocity* GetVelocity(void) { return m_pMove.get(); }
private:
	TYPE m_type;
	std::shared_ptr<CVelocity> m_pMove;	// 移動量
	D3DXCOLOR m_col;					// 色
	float m_decRadius;					// 大きさの減少スピード
	float m_decAlv;						// α値の減少スピード
	float m_fRadius;					// 半径
	int m_nLife;						// 寿命
};

#endif