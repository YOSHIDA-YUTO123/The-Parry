//===================================================
//
// デバッグフォント [debugproc.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"

//***************************************************
// マネージャークラスの定義
//***************************************************
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	HRESULT Init(void);
	void Uninit(void);
	static void Print(const char* fmt...);
	static void Draw(void);
	static void Reset(void) { m_nIdx = 0; }
private:
	static int m_nIdx;				// インデックス
	static LPD3DXFONT m_pFont;		// フォントへのポインタ
	static char m_aStr[MAX_WORD];	// 文字列を格納するバッファ
};
#endif