//===================================================
//
// ロードマネージャー [LoadManager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _LOADMANAGER_H_
#define _LOADMANAGER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

//***************************************************
// ロードのマネージャークラスの定義
//***************************************************
class CLoadManager
{
public:
	CLoadManager();
	~CLoadManager();
	std::istringstream SetInputvalue(std::string input); // = から先の数値を取り出す処理
	std::string GetString(std::string input);	   // std::stringから文字列を取り出す処理
private:
};

#endif
