//================================================
//
// リザルトのプレイヤー [ResultPlayer.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _RESULTPLAYER_H_
#define _RESULTPLAYER_H_

//************************************************
// インクルードファイル
//************************************************
#include "character3D.h"

//************************************************
// リザルトのプレイヤークラスの定義
//************************************************
class CResultPlayer : public CCharacter3D
{
public:

	// モーションの種類
	typedef enum
	{
		MOTIONTYPE_DOWN_NEUTRAL = 0,
		MOTIONTYPE_MAX
	}MOTIONTYPE;

	CResultPlayer();
	~CResultPlayer();

	static CResultPlayer* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

#endif