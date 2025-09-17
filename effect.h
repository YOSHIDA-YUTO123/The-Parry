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
		TYPE_NORAML = 0, // 通常
		TYPE_HIT,		 // ヒット時
		TYPE_FIRE,		 // 火
		TYPE_ZONE,		 // 集中
		TYPE_MAX
	};

	// エフェクトのデータ
	struct Data
	{
		std::shared_ptr<CVelocity> pMove;	// 移動量
		D3DXCOLOR col;						// 色
		float decRadius;					// 大きさの減少スピード
		float decAlv;						// α値の減少スピード
		float fRadius;						// 半径
		float fInertia;					// 慣性
		int nLife;							// 寿命
	};

	CEffect3D(int nPriority = 4);
	~CEffect3D();

	static CEffect3D* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col, const TYPE type);
	void Set(const int nLife, const D3DXVECTOR3 move, const float fInertia = 0.01f);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	CVelocity* GetVelocity(void) { return m_Data.pMove.get(); }
protected:
	HRESULT SetUp(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);
	Data GetData(void) const { return m_Data; }
	void SetData(const Data data) { m_Data = data; }
private:
	TYPE m_type; // エフェクトの種類
	Data m_Data; // エフェクトのデータ
};

#endif
