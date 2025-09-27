//================================================
//
// 背景 [background.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"

//************************************************
// 背景のクラスの定義
//************************************************
class CBackGround : public CObject2D
{
public:

	enum TYPE
	{
		TYPE_NONE = 0,
		TYPE_PAUSE,
		TYPE_RESULT,
		TYPE_MAX
	};

	CBackGround();
	~CBackGround();

	static CBackGround* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col,const TYPE type);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override {};
	virtual void Draw(void) override;
private:
	TYPE m_type; // 種類
	D3DXCOLOR m_col; // 色
};

#endif
