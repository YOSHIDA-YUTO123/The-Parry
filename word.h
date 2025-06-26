//================================================
//
// 文字を操作する処理 [word.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// 多重インクルード防止
//************************************************
#ifndef _WORD_H_
#define _WORD_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"

//************************************************
// ワードクラスの定義
//************************************************
class CWord
{
public:

	// 文字の種類
	typedef enum
	{
		WORD_A = 0,
		WORD_B,
		WORD_C,
		WORD_D,
		WORD_E,
		WORD_F,
		WORD_G,
		WORD_H,
		WORD_I,
		WORD_J,
		WORD_K,
		WORD_L,
		WORD_M,
		WORD_N,
		WORD_O,
		WORD_P,
		WORD_Q,
		WORD_R,
		WORD_S,
		WORD_T,
		WORD_U,
		WORD_V,
		WORD_W,
		WORD_X,
		WORD_Y,
		WORD_Z,
		WORD_MAX
	}WORD;

	CWord();
	~CWord();

	HRESULT Init(const int nPosX, const int nPosY, const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetSize(const float fWIdth, const float fHeight);
	void SetPos(const D3DXVECTOR3 pos);
	void SetUV(const int nNum);
	void SetColor(const D3DXCOLOR col);
	void SetTextureID(const char* pFileName);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;	// 頂点へのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	float m_fWidth, m_fHeight;				// 幅、高さ
	int m_nTextureIdx;						// テクスチャのインデックス
};
#endif