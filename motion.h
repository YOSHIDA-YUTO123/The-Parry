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
#include<memory>

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
		std::vector <Key> aKey;				// 各キーパーツのキー要素
	};

	// モーション情報の構造体
	struct Info
	{
		bool bLoop;						// ループするかどうか
		int nNumkey;					// キーの総数
		std::vector<Key_Info> aKeyInfo;	// キー情報
		std::vector<int> aStartFrame;	// スタートフレーム
		std::vector<int> aEndFrame;		// 終了フレーム
		int nNumEventFrame;				// イベントフレームの総数
	};

	CMotion();
	~CMotion();

	static std::unique_ptr<CMotion> Load(const char* pLoadFileName, std::vector<CModel*> &pModel, int* pOutModel, const int nNumMotion, LOAD type);

	void GetInfo(CMotion* pMotion);

	void Uninit(void);
	void Update(CModel** pModel, const int nNumModel);
	void SetMotion(const int motiontype, bool bBlend, const int nBlendFrame,const int nBlendType = 0);
	void SetEndBlendType(const int nBlendType) { m_nEndBlendMotion = nBlendType; }
	bool IsEndMotion(void);
	bool IsEndLoopMotion(void);
	bool IsFinishEndBlend(void);
	int GetType(void) const { return m_nType; }
	int GetBlendType(void) const { return m_nTypeBlend; }
	bool IsEventFrame(const int nStartFrame, const int nEndFrame, const int nType);
	bool IsEventFrame(const int nType);
	bool IsEventFrame(const int nType,const int nIdx);
	bool IsLoad(void);
	bool FinishMotion(void);
	void Debug(void);
private:
	void UpdateCurrentMotion(CModel** pModel, int nIdx);
	void UpdateBlendMotion(CModel** pModel, int nIdx);
	void FinishFirstBlend(void);

	std::unique_ptr<CMotionLoader> m_pLoader;   // モーションのローダー
	std::vector<Info> m_aInfo;	// モーション情報へのポインタ
	int m_nType;				// モーションの種類
	int m_nNumKey;				// キーの総数
	int m_nKey;					// 現在のキーNo.
	int m_nCount;				// モーションのカウンター
	int m_nextKey;				// 次のキー
	int m_nAllFrame;			// 全体のフレーム数
	int m_nAllCounter;			// 全体フレームのカウンター
	bool m_bLoopMotion;			// ループするかどうか

	bool m_bBlend;				// ブレンドがあるかどうか
	bool m_bLoopBlend;			// ループするかどうか
	bool m_bFinish;				// モーションが終わったかどうか
	bool m_bFirst;				// モーションが始まったフラグ

	int m_nEndBlendMotion;		// モーションが終わった時につなげるモーションの種類
	int m_nTypeBlend;			// ブレンドの種類
	int m_nFrameBlend;			// ブレンドのフレーム数
	int m_nCounterBlend;		// ブレンドカウンター
	int m_nNumKeyBlend;			// ブレンドモーションの最大のキー
	int m_nKeyBlend;			// ブレンドモーションの現在のキー
	int m_nNextKeyBlend;		// ブレンドモーションの次のキー
	int m_nCounterMotionBlend;	// ブレンドのカウンター

	bool m_bSlowMag;			// スローの倍率がかかっているかどうか
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

	void GetInfo(CMotionLoader *pLoader);
protected:
	std::vector<CMotion::Info> m_aInfo;  // モーションの情報
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
	static std::unique_ptr<CLoderText> LoadTextFile(const char* pFileName, std::vector<CMotion::Info>& Info, std::vector<CModel*>& pModel, int* OutNumModel, const int nNumMotion);
private:
	bool LoadModel(std::vector<CModel*>& pModel, int nCnt, std::string input, std::string line);
	bool LoadCharacterSet(std::vector<CModel*>& pModel, std::string line, std::string input);
	void LoadMotionSet(CLoderText* pLoader, std::ifstream& File, std::string nowLine, const int nNumMotion);
	std::istringstream SetInputvalue(std::string input);
};
#endif