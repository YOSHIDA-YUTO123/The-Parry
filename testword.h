//================================================
//
// 文字テスト [testword.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// 多重インクルード防止
//************************************************
#ifndef _TESTWORD_H_
#define _TESTWORD_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include"object.h"
#include"word.h"

//************************************************
// 文字クラスの定義
//************************************************
class CTestWord : public CObject
{
public:
	CTestWord();
	~CTestWord();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CTestWord* Create(const int nWord, const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
private:
	CWord* m_pWord;		// 文字のクラスへのポインタ
	D3DXVECTOR3 m_pos;	// 位置
	float m_fWidth;		// 横幅
	float m_fHeight;	// 高さ
	int m_nWord;		// 文字
};
#endif