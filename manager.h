//===================================================
//
// マネージャー [manager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MANAGER_H_
#define _MANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "main.h"
#include<memory>
#include"scene.h"

//***************************************************
// 前方宣言
//***************************************************
class CCollisionManager;
class CRenderer;
class CScene;
class CInputKeyboard;
class CInputJoypad;
class CInputMouse;
class CSound;
class CTextureManager;
//class CCamera;
class CLight;
class CModelManager;
class CSlow;
class CFade;

//***************************************************
// マネージャークラスの定義
//***************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CRenderer* GetRenderer(void);
	static CInputKeyboard* GetInputKeyboard(void);
	static CInputJoypad* GetInputJoypad(void);
	static CInputMouse* GetInputMouse(void);
	static CSound* GetSound(void);
	void SetFps(const int fps) { m_fps = fps; }
	static int GetFrameCounter(void);
	static bool GetShowDebug(void) { return m_bShowDebug; }
	static CTextureManager* GetTexture(void);
	//static CCamera* GetCamera(void);
	static CLight* GetLight(void);
	static CModelManager* GetModel(void);
	static CSlow* GetSlow(void) { return m_pSlow; }
	static void SetMode(std::unique_ptr<CScene> pNewScene);
	static CScene::MODE GetMode(void) { return m_pScene->GetMode(); }
	static CFade* GetFade(void) { return m_pFade; }
	static void DrawScene(void);
private:
	static CRenderer* m_pRenderer;				// レンダラーのポインタ
	static CInputKeyboard* m_pInputKeyboard;	// キーボードのポインタ
	static CInputJoypad* m_pInputJoypad;		// パッドのポインタ
	static CInputMouse* m_pInputMouse;			// マウスのポインタ
	static CSound* m_pSound;					// サウンドのポインタ
	static CTextureManager* m_pTexture;			// テクスチャクラスへのポインタ	
	//static CCamera* m_pCamera;					// カメラのポインタ
	static CLight* m_pLight;					// ライトへのポインタ
	static CModelManager* m_pModel;				// モデルクラスへのポインタ
	static CSlow* m_pSlow;						// スローモーションクラスへのポインタ
	static std::unique_ptr<CScene> m_pScene;	// シーンのクラスへのポインタ
	static CFade* m_pFade;				
	static int m_nFrameCounter;					// フレームカウンター
	static bool m_bShowDebug;					// デバッグ表示をするかしないか
	int m_fps;									// fps格納用変数
};
#endif