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
#include "player.h"

//***************************************************
// タイトルのプレイヤークラスの定義
//***************************************************
class CTitlePlayer : public CPlayer
{
public:
	CTitlePlayer();
	~CTitlePlayer();

	static CTitlePlayer* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	virtual CPlayerMovement* GetMovement(void) { return nullptr; } // 
private:
};

#endif