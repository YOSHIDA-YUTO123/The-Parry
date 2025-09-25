//===================================================
//
// タイトルのプレイヤー [TitlePlayer.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _TITLEPLAYER_H_
#define _TITLEPLAYER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "character3D.h"
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CVelocity;
class CPlayerMovement;

//***************************************************
// タイトルのプレイヤークラスの定義
//***************************************************
class CTitlePlayer : public CCharacter3D
{
public:

	enum MOTIONTYPE
	{
		MOTIONTYPE_NEUTRAL = 0,
		MOTIONTYPE_DASH,
		MOTIONTYPE_MAX
	};

	CTitlePlayer();
	~CTitlePlayer();

	static CTitlePlayer* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	std::unique_ptr<CPlayerMovement> m_pMoveMent;	// 移動制御クラスへのポインタ
};

#endif