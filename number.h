//================================================
//
// 数字を操作する処理 [number.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _NUMBER_H_
#define _NUMBER_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include"object.h"
#include"transform.h"

//************************************************
// ナンバークラスの定義
//************************************************
class CNumber
{
public:

	CNumber();
	~CNumber();

	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetSize(const D3DXVECTOR2 Size) { m_Size = Size; }
	void SetPos(const D3DXVECTOR3 pos);
	void SetUV(const int nNum);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pFileName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR2 m_Size;						// 大きさ
	int m_nTextureIdx;						// テクスチャのインデックス
};
#endif