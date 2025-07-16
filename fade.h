//===================================================
//
// フェード処理 [fade.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _FADE_H_
#define _FADE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object2D.h"

class CScene;

//***************************************************
// フェードクラスの定義
//***************************************************
class CFade : public CObject2D
{
public:

//**************************
// フェードの状態列挙型
//**************************
	typedef enum
	{
		FADE_NONE = 0,			// フェードなしの状態
		FADE_IN,				// フェードインの状態
		FADE_OUT,				// フェードアウトの状態
		FAFE_MAX
	}FADE;

	~CFade();

	static CFade* Create(void);
	void SetFade(CScene* pNewScene);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	CFade();
	FADE m_Fade;		// フェード
	D3DXCOLOR m_col;	// フェードの色
	CScene* m_pScene;
};

#endif
