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

	void ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU); // レンダリングターゲットの変更

	LPDIRECT3DDEVICE9 GetDevice(void);
	LPDIRECT3DTEXTURE9 GetTextureMT(void) const { return m_pTextureMT; }

	void onWireFrame();
	void offWireFrame();

private:
	LPDIRECT3DTEXTURE9 m_pTextureMT;		// レンダリングターゲット用テクスチャ
	LPDIRECT3DSURFACE9 m_pRenderMT;			// レンダリングターゲット用インターフェース
	LPDIRECT3DSURFACE9 m_pZBuffMT;			// レンダリングターゲット用Zバッファ
	D3DVIEWPORT9 m_viewport;				
	LPDIRECT3D9 m_pD3D;						// Directxデバイスへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// Directxデバイスへのポインタ
	static CDebugProc* m_pDebug;			// デバッグフォントへのポインタ
};

#endif