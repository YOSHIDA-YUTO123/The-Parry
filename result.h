//===================================================
//
// リザルトシーン [result.h]
// Author:YUTO YOSHIDA
//
//===================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _RESULT_H_
#define _RESULT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include "scene.h"
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CResultCamera;
class CCamera;

//***************************************************
// リザルト(勝利)クラスの定義
//***************************************************
class CResultWin : public CScene
{
public:

	CResultWin();
	~CResultWin();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	std::unique_ptr<CResultCamera> m_pCamera; // カメラクラスへのポインタ
};

//***************************************************
// リザルト(敗北)クラスの定義
//***************************************************
class CResultLose : public CScene
{
public:

	CResultLose();
	~CResultLose();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
private:
	std::unique_ptr<CCamera> m_pCamera; // カメラクラスへのポインタ
};

#endif