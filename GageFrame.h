//================================================
//
// ゲージフレーム [GageFrame.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _GAGEFRAME_H_
#define _GAGEFRAME_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"

//************************************************
// ゲージのフレームのクラスの定義
//************************************************
class CGageFrame : public CObject2D
{
public:

	// HPゲージの種類
	enum TYPE
	{
		TYPE_HP_PLAYER = 0,
		TYPE_HP_ENEMY,
		TYPE_STAMINA,
		TYPE_MAX
	};

	CGageFrame(int nPriority = 4);
	~CGageFrame();

	static CGageFrame* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const TYPE type);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	TYPE m_type; // 種類
};

#endif