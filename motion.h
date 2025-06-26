//===================================================
//
// モーション [motion.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MOTION_H_
#define _MOTION_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"model.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

using namespace std;

//***************************************************
// マクロ定義
//***************************************************
#define MAX_PARTS (56) // パーツの最大数
#define MAX_KEY (20)   // キーの最大数

//***************************************************
// 前方宣言
//***************************************************
class CModel;
class CMotionLoader;

//***************************************************
// モーションクラスの定義
//***************************************************
class CMotion
{
public:

	// 読み込み形式
	typedef enum
	{
		LOAD_TEXT = 0,
		LOAD_BIN,
		LOAD_JSON,
		LOAD_MAX
	}LOAD;

	// キーの構造体
	struct Key
	{
		float fPosX; // 位置X
		float fPosY; // 位置Y
		float fPosZ; // 位置Z

		float fRotX; // 向きX
		float fRotY; // 向きY
		float fRotZ; // 向きZ
	};

	// キー情報の構造体
	struct Key_Info
	{
		int nFrame;						// 再生フレーム
		vector <Key> aKey;				// 各キーパーツのキー要素
	};

	// モーション情報の構造体
	struct Info
	{
		bool bLoop;						// ループするかどうか
		int nNumkey;					// キーの総数
		vector<Key_Info> aKeyInfo;		// キー情報
	};

	CMotion();
	~CMotion();

	static CMotion* Load(const char* pLoadFileName, CModel** ppModel, const int nMaxSize, int* pOutModel, const int nNumMotion, LOAD type);
	void Uninit(void);
	void Update(CModel** pModel, const int nNumModel);
	void SetMotion(const int motiontype, bool bBlend, const int nBlendFrame);
	bool IsEndMotion(void);
	bool IsFinishEndBlend(void);
	int GetMotionType(void) const { return m_nType; }
	int GetBlendMotionType(void) const { return m_nTypeBlend; }
	bool IsIventFrame(const int nStartFrame, const int nEndFrame, const int nType);
	bool GetLoadResult(void);

	void Debug(void);
private:
	void UpdateCurrentMotion(CModel** pModel, int nIdx);
	void UpdateBlendMotion(CModel** pModel, int nIdx);
	void FinishFirstBlend(void);

	CMotionLoader* m_pLoader;   // モーションのローダー
	vector<Info> m_aInfo;		// モーション情報へのポインタ
	int m_nType;				// モーションの種類
	int m_nNumKey;				// キーの総数
	int m_nKey;					// 現在のキーNo.
	int m_nCount;				// モーションのカウンター
	int m_nextKey;				// 次のキー
	int m_nAllFrame;			// 全体のフレーム数
	int m_nAllCounter;			// 全体フレームのカウンター
	bool m_bLoopMotion;				// ループするかどうか

	bool m_bBlend;						// ブレンドがあるかどうか
	bool m_bLoopBlend;					// ループするかどうか
	bool m_bFinish;						// モーションが終わったかどうか
	bool m_bFirst;						// モーションが始まったフラグ

	int m_nTypeBlend;					// ブレンドの種類
	int m_nFrameBlend;					// ブレンドのフレーム数
	int m_nCounterBlend;				// ブレンドカウンター
	int m_nNumKeyBlend;					// ブレンドモーションの最大のキー
	int m_nKeyBlend;					// ブレンドモーションの現在のキー
	int m_nNextKeyBlend;				// ブレンドモーションの次のキー
	int m_nCounterMotionBlend;			// ブレンドのカウンター
};

//***************************************************
// モーションのロードクラスの定義
//***************************************************
class CMotionLoader
{
public:

	CMotionLoader();
	~CMotionLoader();

	void SetNumModel(const int nNumModel) { m_nNumModel = nNumModel; }
	void SetNumMotion(const int nNumMotion) { m_nNumMotion = nNumMotion; }

	int GetNumModel(void) const { return m_nNumModel; }
	int GetNumMotion(void) const { return m_nNumMotion; }

protected:
	vector<CMotion::Info> m_aInfo;  // モーションの情報
private:
	int m_nNumModel;				// モデルの最大数
	int m_nNumMotion;				// モーションの総数
};

//***************************************************
// モーションのロードクラス(textFile)の定義
//***************************************************
class CLoderText : public CMotionLoader
{
public:
	static CLoderText* LoadTextFile(const char* pFileName, vector<CMotion::Info>& Info, CModel** ppModel, int* OutNumModel, const int nMaxSize, const int nNumMotion);
private:
	bool LoadModel(CModel** ppModel, const int nMaxSize, int nCnt, string input, string line);
	bool LoadCharacterSet(CModel** ppModel, string line, string input);
	void LoadMotionSet(CLoderText* pLoader, ifstream& File, string nowLine, const int nNumMotion);
	istringstream SetInputvalue(string input);
};
#endif