//===================================================
//
// ポリゴンの描画処理 [renderer.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _RENDERER_H_
#define _RENDERER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include "debugproc.h"

//***************************************************
// レンダラークラスの定義
//***************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(const int fps);
	LPDIRECT3DDEVICE9 GetDevice(void);

	void onWireFrame();
	void offWireFrame();

private:
	LPDIRECT3D9 m_pD3D;						// Directxデバイスへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// Directxデバイスへのポインタ
	static CDebugProc* m_pDebug;			// デバッグフォントへのポインタ
};

#endif