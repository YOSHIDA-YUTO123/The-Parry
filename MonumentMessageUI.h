//===================================================
//
// 石碑ブロックのメッセージ表示処理 [MonumentMessageUI.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MONUMENTMESSAGEUI_H_
#define _MONUMENTMESSAGEUI_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "FadeInObject2D.h"

//***************************************************
// モニュメントのメッセージUIクラスの定義
//***************************************************
class CMonumentMessageUI : public CFadeInObject2D
{
public:
	typedef enum
	{
		TYPE_ONE = 0,
		TYPE_TWO,
		TYPE_MAX
	}TYPE;

	CMonumentMessageUI();
	~CMonumentMessageUI();

	static CMonumentMessageUI* Create(const D3DXVECTOR3 pos,const TYPE type);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Enable(const bool bEnable) { m_bShow = bEnable; }
private:
	TYPE m_type;	// 種類
	bool m_bShow;	// 表示するかどうか
};

#endif
