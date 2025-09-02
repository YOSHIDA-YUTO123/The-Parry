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
#include<memory>

class CScene;

//***************************************************
// フェードクラスの定義
//***************************************************
class CFade
{
public:
	typedef enum
	{
		FADE_NONE = 0,			// フェードなしの状態
		FADE_IN,				// フェードインの状態
		FADE_OUT,				// フェードアウトの状態
		FAFE_MAX
	}FADE;

	~CFade();

	static CFade* Create(void);
	void SetFade(std::unique_ptr<CScene> pNewScene,const D3DXCOLOR col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	FADE GetState(void) const { return  m_Fade; }
private:
	CFade();
	FADE m_Fade;							// フェード
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;   // 頂点バッファへのポインタ
	D3DXCOLOR m_col;						// フェードの色
	std::unique_ptr<CScene> m_pScene;						// シーンのポインタ
};

#endif
