//===================================================
//
// ロードマネージャー [LoadManager.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "LoadManager.h"

using namespace std; // 名前空間stdを使用

constexpr int MAX_WORD = 1024; // 最大の文字数

//===================================================
// コンストラクタ
//===================================================
CLoadManager::CLoadManager()
{
}

//===================================================
// デストラクタ
//===================================================
CLoadManager::~CLoadManager()
{

}

//===================================================
// = から先を取り出す処理
//===================================================
istringstream CLoadManager::SetInputvalue(string input)
{
	// 先頭の = を消す
	input.erase(0, input.find_first_not_of(" = "));

	// inputから数値を取り出す準備
	istringstream value_Input(input);

	return value_Input;
}

//===================================================
// std::Stringの取得
//===================================================
string CLoadManager::GetString(string input)
{
	// = から先を取り出す
	SetInputvalue(input);

	// inputから数値を取り出す準備
	istringstream value_Input(input);

	// モデルの名前格納用変数
	string FileName;

	// 数値を代入する
	value_Input >> FileName;

	return FileName;
}
