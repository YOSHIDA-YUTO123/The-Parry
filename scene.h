//===================================================
//
// シーン(基底クラス) [scene.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _SCENE_H_
#define _SCENE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// シーンクラスの定義
//***************************************************
class CScene
{
public:

	// モードの種類
	typedef enum
	{
		MODE_TITLE = 0,	// タイトル画面
		MODE_EDIT,		// 編集
		MODE_TUTORIAL,	// チュートリアル画面
		MODE_OPENING,	// 入場ムービー
		MODE_GAME,		// ゲーム画面
		MODE_RESULT,	// リザルト画面
		MODE_RANKING,	// ランキング画面
		MODE_MAX
	}MODE;

	CScene(MODE mode);
	virtual ~CScene();

	MODE GetMode(void) const { return m_mode; }
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

private:
	MODE m_mode; // モード
};
#endif