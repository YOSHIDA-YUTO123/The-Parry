//===================================================
//
// モーション [motion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "motion.h"
#include<stdio.h>
#include"object.h"
#include"math.h"
#include "debugproc.h"
#include "manager.h"

//***************************************************
// マクロ定義
//***************************************************
#define NEUTRAL (0) // ニュートラル

//===================================================
// コンストラクタ
//===================================================
CMotion::CMotion()
{
	//m_aInfo = nullptr;
	m_bLoopMotion = false;
	m_nCount = NULL;
	m_nextKey = NULL;
	m_nKey = NULL;
	m_nNumKey = NULL;
	m_nType = NULL;
	m_nTypeBlend = NULL;
	m_bFinish = false;
	m_bFirst = false;
	m_bBlend = false;
	m_bLoopBlend = false;
	m_nCounterBlend = NULL;
	m_nCounterMotionBlend = NULL;
	m_nNextKeyBlend = NULL;
	m_nFrameBlend = NULL;
	m_nKeyBlend = NULL;
	m_nNumKeyBlend = NULL;
	m_nAllCounter = NULL;
	m_nAllFrame = NULL;
}

//===================================================
// デストラクタ
//===================================================
CMotion::~CMotion()
{
}

//===================================================
// モーションのロード処理
//===================================================
CMotion* CMotion::Load(const char* pLoadFileName, CModel** ppModel, const int nMaxSize, int* pOutModel,const int nNumMotion, LOAD type)
{
	// モーションを生成
	CMotion* pMotion = new CMotion;

	if (pMotion != nullptr)
	{
		switch (type)
		{
		case LOAD_TEXT:
			// モーションのロード処理(textFile)
			pMotion->m_pLoader = CLoderText::LoadTextFile(pLoadFileName, pMotion->m_aInfo, ppModel, pOutModel, nMaxSize, nNumMotion);
			break;
		default:
			MessageBox(NULL, "この形式は読み込めません", "エラー", MB_OK | MB_ICONWARNING);
			break;
		}
	}

	return pMotion;
}

//===================================================
// イベントフレームの判定
//===================================================
bool CMotion::IsIventFrame(const int nStartFrame, const int nEndFrame,const int nType)
{
	// イベントフレームの範囲に入ったら
	if (m_nAllCounter >= nStartFrame &&
		m_nAllCounter <= nEndFrame &&
		m_nType == nType)
	{
		return true;
	}

	return false;
}

//===================================================
// ロードできたかどうか
//===================================================
bool CMotion::GetLoadResult(void)
{
	// 読み込み ? 成功true : 失敗false
	return m_pLoader ? true : false;
}

//===================================================
// ループモーションの終了判定
//===================================================
bool CMotion::FinishLoopMotion(void)
{
	if (m_aInfo[m_nType].bLoop == true && m_aInfo[m_nType].nNumkey - 1 <= m_nKey)
	{
		return true;
	}
	return false;
}

//===================================================
// デバッグ
//===================================================
void CMotion::Debug(void)
{
	// デバッグ情報の描画
	CDebugProc::Print("FRAMEALL = %d / %d\n", m_nAllCounter, m_nAllFrame);
	//CDebugProc::Print(" = %d / %d");
}

//===================================================
// 終了処理
//===================================================
void CMotion::Uninit(void)
{
	// ローダーの破棄
	if (m_pLoader != nullptr)
	{
		delete m_pLoader;
		m_pLoader = nullptr;
	}
}

//===================================================
// 最初のモーションのブレンドが終わったら
//===================================================
void CMotion::FinishFirstBlend(void)
{
	// モーションの出だしのブレンドが終了した
	if (m_bFirst == true && m_nCounterBlend >= m_nFrameBlend && m_bFinish == false)
	{
		// もとに戻す
		m_bFirst = false;

		m_nKey = 0;

		// モーションをブレンドしたモーションにする
		m_nType = m_nTypeBlend;

		m_nCount = m_nCounterMotionBlend;
		m_nAllCounter = m_nCount;

		m_nCounterBlend = 0;
	}
}

//===================================================
// 現在のモーションの更新処理
//===================================================
void CMotion::UpdateCurrentMotion(CModel** pModel, int nIdx)
{
	// 次のキーのアドレスの取得
	Key* pKeyNext = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// 現在のキーのアドレスの取得
	Key* pKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// キー情報のアドレスの取得
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	// 次のキーまでの距離を求める
	float fDiffPosX = pKeyNext->fPosX - pKey->fPosX;
	float fDiffPosY = pKeyNext->fPosY - pKey->fPosY;
	float fDiffPosZ = pKeyNext->fPosZ - pKey->fPosZ;

	float fDiffRotX = pKeyNext->fRotX - pKey->fRotX;
	float fDiffRotY = pKeyNext->fRotY - pKey->fRotY;
	float fDiffRotZ = pKeyNext->fRotZ - pKey->fRotZ;

	// 角度の正規化
	NormalizeRot(&fDiffRotX);
	NormalizeRot(&fDiffRotY);
	NormalizeRot(&fDiffRotZ);

	// フレームと最大フレームの割合を求める
	float fRateFrame = (float)m_nCount / (float)pKeyInfo->nFrame;

	// 次のキーの位置までフレームに応じた位置を設定する
	float fPosX = LerpDiff(pKey->fPosX, fDiffPosX, fRateFrame);
	float fPosY = LerpDiff(pKey->fPosY, fDiffPosY, fRateFrame);
	float fPosZ = LerpDiff(pKey->fPosZ, fDiffPosZ, fRateFrame);

	// 次のキーの向きまでフレームに応じた向きを設定する
	float fRotX = LerpDiff(pKey->fRotX, fDiffRotX, fRateFrame);
	float fRotY = LerpDiff(pKey->fRotY, fDiffRotY, fRateFrame);
	float fRotZ = LerpDiff(pKey->fRotZ, fDiffRotZ, fRateFrame);

	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// ブレンドのモーションの更新処理
//===================================================
void CMotion::UpdateBlendMotion(CModel** pModel, int nIdx)
{
	// 現在のキーのアドレスを取得
	Key* pCurrentKey = &m_aInfo[m_nType].aKeyInfo[m_nKey].aKey[nIdx];

	// 次のキーのアドレスを取得
	Key* pNextKey = &m_aInfo[m_nType].aKeyInfo[m_nextKey].aKey[nIdx];

	// キー情報のアドレスの取得
	Key_Info* pKeyInfo = &m_aInfo[m_nType].aKeyInfo[m_nKey];

	float fRateMotion = (float)m_nCount / (float)pKeyInfo->nFrame; // 相対値
	float fRateMotionBlend = (float)m_nCounterMotionBlend / (float)m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame;

	float fRateBlend = (float)m_nCounterBlend / (float)m_nFrameBlend;

	// 現在のモーションの角度の差分
	float fDiffMotionRX = pNextKey->fRotX - pCurrentKey->fRotX;
	float fDiffMotionRY = pNextKey->fRotY - pCurrentKey->fRotY;
	float fDiffMotionRZ = pNextKey->fRotZ - pCurrentKey->fRotZ;

	// 角度の正規化
	NormalizeRot(&fDiffMotionRX);
	NormalizeRot(&fDiffMotionRY);
	NormalizeRot(&fDiffMotionRZ);

	// 現在のモーションのフレームに応じた角度を設定
	float fRotXCurrent = LerpDiff(pCurrentKey->fRotX, fDiffMotionRX, fRateMotion);
	float fRotYCurrent = LerpDiff(pCurrentKey->fRotY, fDiffMotionRY, fRateMotion);
	float fRotZCurrent = LerpDiff(pCurrentKey->fRotZ, fDiffMotionRZ, fRateMotion);

	// ブレンドのキーのアドレスを取得
	Key* pCurrentBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nIdx];

	// ブレンドの次のキーのアドレスを取得
	Key* pNextBlendKey = &m_aInfo[m_nTypeBlend].aKeyInfo[m_nNextKeyBlend].aKey[nIdx];

	//ブレンドモーションの角度の差分を求める
	float fDiffMotionBlendRX = pNextBlendKey->fRotX - pCurrentBlendKey->fRotX;
	float fDiffMotionBlendRY = pNextBlendKey->fRotY - pCurrentBlendKey->fRotY;
	float fDiffMotionBlendRZ = pNextBlendKey->fRotZ - pCurrentBlendKey->fRotZ;

	// 角度の正規化
	NormalizeRot(&fDiffMotionBlendRX);
	NormalizeRot(&fDiffMotionBlendRY);
	NormalizeRot(&fDiffMotionBlendRZ);

	// ブレンドモーションのフレームに応じた角度の設定
	float fRotXBlend = LerpDiff(pCurrentBlendKey->fRotX, fDiffMotionBlendRX, fRateMotionBlend);
	float fRotYBlend = LerpDiff(pCurrentBlendKey->fRotY, fDiffMotionBlendRY, fRateMotionBlend);
	float fRotZBlend = LerpDiff(pCurrentBlendKey->fRotZ, fDiffMotionBlendRZ, fRateMotionBlend);

	//現在のモーションの位置の差分を求める
	float fDiffMotionPX = pNextKey->fPosX - pCurrentKey->fPosX;
	float fDiffMotionPY = pNextKey->fPosY - pCurrentKey->fPosY;
	float fDiffMotionPZ = pNextKey->fPosZ - pCurrentKey->fPosZ;

	// 現在のモーションのフレームに応じた位置の設定
	float fPosXCurrent = LerpDiff(pCurrentKey->fPosX, fDiffMotionPX, fRateMotion);
	float fPosYCurrent = LerpDiff(pCurrentKey->fPosY, fDiffMotionPY, fRateMotion);
	float fPosZCurrent = LerpDiff(pCurrentKey->fPosZ, fDiffMotionPZ, fRateMotion);

	// ブレンドモーションの位置の差分を求める
	float fDiffMotionBlendPX = pNextBlendKey->fPosX - pCurrentBlendKey->fPosX;
	float fDiffMotionBlendPY = pNextBlendKey->fPosY - pCurrentBlendKey->fPosY;
	float fDiffMotionBlendPZ = pNextBlendKey->fPosZ - pCurrentBlendKey->fPosZ;

	// ブレンドモーションのフレームに応じた位置の設定
	float fPosXBlend = LerpDiff(pCurrentBlendKey->fPosX, fDiffMotionBlendPX, fRateMotionBlend);
	float fPosYBlend = LerpDiff(pCurrentBlendKey->fPosY, fDiffMotionBlendPY, fRateMotionBlend);
	float fPosZBlend = LerpDiff(pCurrentBlendKey->fPosZ, fDiffMotionBlendPZ, fRateMotionBlend);

	// 現在の角度とブレンドする角度の差分を求める
	float fDiffBlendRX = fRotXBlend - fRotXCurrent;
	float fDiffBlendRY = fRotYBlend - fRotYCurrent;
	float fDiffBlendRZ = fRotZBlend - fRotZCurrent;

	// ブレンドのフレームに応じた向きの設定
	float fRotX = LerpDiff(fRotXCurrent, fDiffBlendRX, fRateBlend);
	float fRotY = LerpDiff(fRotYCurrent, fDiffBlendRY, fRateBlend);
	float fRotZ = LerpDiff(fRotZCurrent, fDiffBlendRZ, fRateBlend);

	// 現在の位置とブレンドする位置の差分を求める
	float fDiffBlendPX = fPosXBlend - fPosXCurrent;
	float fDiffBlendPY = fPosYBlend - fPosYCurrent;
	float fDiffBlendPZ = fPosZBlend - fPosZCurrent;

	// ブレンドのフレームに応じた位置の設定
	float fPosX = LerpDiff(fPosXCurrent, fDiffBlendPX, fRateBlend);
	float fPosY = LerpDiff(fPosYCurrent, fDiffBlendPY, fRateBlend);
	float fPosZ = LerpDiff(fPosZCurrent, fDiffBlendPZ, fRateBlend);

	// 位置を設定
	pModel[nIdx]->SetPosition(D3DXVECTOR3(fPosX, fPosY, fPosZ));

	// 向きを設定
	pModel[nIdx]->SetRotaition(D3DXVECTOR3(fRotX, fRotY, fRotZ));
}

//===================================================
// 更新処理
//===================================================
void CMotion::Update(CModel** pModel,const int nNumModel)
{
	if (GetLoadResult() == false) return;

	for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
	{
		// キーの総数
		m_nNumKey = m_aInfo[m_nType].nNumkey;
		m_nNumKeyBlend = m_aInfo[m_nTypeBlend].nNumkey;

		// ループするかどうか
		m_bLoopMotion = m_aInfo[m_nType].bLoop;

		//if (motiontype < 0 || motiontype >= TYPE::TYPE_MAX)
		//{
		//	return;
		//}
		
		// 次のキーを増やす
		m_nextKey = (m_nKey + 1) % m_aInfo[m_nType].nNumkey;

		// 次のブレンドキーを増やす
		m_nNextKeyBlend = (m_nKeyBlend + 1) % m_nNumKeyBlend;

		if (m_bFinish == false && m_bFirst == false)
		{
			// 現在のモーションの更新処理
			UpdateCurrentMotion(pModel, nCntModel);
		}
		if ((m_bFinish == true || m_bFirst == true) && m_bBlend == true)
		{
			// ブレンドのモーションの更新処理
			UpdateBlendMotion(pModel, nCntModel);
		}
	}	
	
	// モーションが終了したら
	if (IsEndMotion())
	{
		m_nCounterMotionBlend = 0;
		m_nKeyBlend = 0;
		m_nAllCounter = 0;
		m_nCounterBlend = 0;
		m_bFinish = true;
		m_nFrameBlend = m_aInfo[m_nType].aKeyInfo[m_nNumKey - 1].nFrame;
		m_nTypeBlend = NEUTRAL;
	}

	// モーションの出だしのブレンドが終了した
	FinishFirstBlend();

	// キーが最大かつブレンドのカウントが最大になった
	if (IsFinishEndBlend() == true)
	{
		m_bFinish = false;				// もとに戻す
		m_bBlend = false;				// もとに戻す
		m_nCount = m_nCounterMotionBlend;	    // フレームをブレンドした先のフレームに合わせる
		m_nAllCounter = m_nCounterMotionBlend;
		m_nType = NEUTRAL;				// モーションタイプをニュートラルにする
		m_nCounterBlend = 0;
	}

	// フレームが最大フレームを超えたら
	if (m_nCount >= m_aInfo[m_nType].aKeyInfo[m_nKey].nFrame)
	{
		// キーを増やす
		m_nKey++;

		// 範囲内にラップする
		m_nKey = Wrap(m_nKey, 0, m_aInfo[m_nType].nNumkey - 1);

		// カウンターをリセット
		m_nCount = 0;
	}

	// ブレンドキーを進める
	if (m_nCounterMotionBlend >= m_aInfo[m_nTypeBlend].aKeyInfo[m_nKeyBlend].nFrame && (m_bFinish == true || m_bFirst == true))
	{
		// キーを増やす
		m_nKeyBlend++;

		// 範囲内にラップする
		m_nKeyBlend = Wrap(m_nKeyBlend, 0, m_aInfo[m_nTypeBlend].nNumkey - 1);

		m_nCounterMotionBlend = 0;
	}

	if (m_bFirst == false)
	{
		m_nAllCounter++;
		m_nCount++;
	}

	// ブレンドが始まったら
	if (m_bFinish == true || m_bFirst == true)
	{
		m_nCounterBlend++;
	}


	int nAllFrame = 0;

	for (int nCnt = 0; nCnt < m_aInfo[m_nType].nNumkey; nCnt++)
	{
		nAllFrame += m_aInfo[m_nType].aKeyInfo[nCnt].nFrame;
	}

	m_nAllFrame = nAllFrame;

	// 最大を超えたら
	if (m_nAllCounter >= m_nAllFrame)
	{
		m_nCounterMotionBlend++;
		m_nAllCounter = 0;
	}

	CDebugProc::Print("%d / %d\n", m_nAllCounter, m_nAllFrame);
}

//===================================================
// モーションの設定処理
//===================================================
void CMotion::SetMotion(const int motiontype,bool bBlend,const int nBlendFrame)
{
	// 同じ種類のモーションだったら
	if (m_nTypeBlend == motiontype || m_nType == motiontype) return;

	// モーションの総数を超えていたら
	if (m_pLoader->GetNumMotion() <= motiontype)
	{
		return;
	}

	// ブレンドがあるなら
	if (bBlend == true)
	{
		m_nKeyBlend = 0;

		if (m_bFirst == false)
		{
			m_nCounterBlend = 0;		 // ブレンドカウンターをリセット
			m_nFrameBlend = nBlendFrame; // ブレンドフレームを設定する
			m_bFirst = true;			 // 最初のブレンド開始フラグをtrueにする
		}

		m_bFinish = false;			 // モーションが終わっていない
		m_nTypeBlend = motiontype;   // ブレンド先の種類を設定
	}
	else
	{
		m_nKey = 0;						// 最初から始める
		m_nCount = 0;					// 最初から始める
		m_nType = motiontype;			// ブレンドするモーションのタイプを代入
		m_nTypeBlend = motiontype;		// ブレンドするモーションのタイプを代入
		m_bFinish = false;
	}

	m_bBlend = bBlend; // ブレンドするかどうか判定
	m_nCounterMotionBlend = 0;
	m_nAllCounter = 0; // 全体のフレームのカウンターをリセットする
}

//===================================================
// モーションが終わったかどうか
//===================================================
bool CMotion::IsEndMotion(void)
{
	// モーションが終了したら
	if (m_bFinish == false &&
		m_nKey >= m_nNumKey - 1 &&
		m_bBlend == true &&
		m_bLoopMotion == false &&
		m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// 終わりのブレンドが完了したか判定
//===================================================
bool CMotion::IsFinishEndBlend(void)
{
	if (m_bFinish == true && m_nFrameBlend <= m_nCounterBlend && m_bFirst == false)
	{
		return true;
	}
	return false;
}

//===================================================
// コンストラクタ
//===================================================
CMotionLoader::CMotionLoader()
{
	m_nNumModel = NULL;
	m_nNumMotion = NULL;
}

//===================================================
// デストラクタ
//===================================================
CMotionLoader::~CMotionLoader()
{
}

//===================================================
// テキストファイルのロード処理
//===================================================
CLoderText* CLoderText::LoadTextFile(const char* pFileName, vector<CMotion::Info>& Info, CModel** ppModel, int* OutNumModel, const int nMaxSize,const int nNumMotion)
{
	// ファイルをロードする
	ifstream File(pFileName);
	string line;
	string input;

	int nNumModel = 0;

	D3DXVECTOR3 offset = VEC3_NULL;

	bool bCharacterSet = false;	// キャラクターの設定をしたかどうか

	if (File.is_open() == true)
	{
		// モーションを生成
		CLoderText* pLoder = new CLoderText;

		// モーション情報構造体のメモリの確保
		pLoder->m_aInfo.resize(nNumMotion);

		// 最後の行になるまで読み込む
		while (getline(File, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			// モデルのロード処理
			if (pLoder->LoadModel(ppModel, nMaxSize, nNumModel, input, line))
			{
				if (nNumModel <= nMaxSize - 1)
				{
					nNumModel++;
				}
			}

			// パーツの設定が終わって無かったら
			if (bCharacterSet == false)
			{
				bCharacterSet = pLoder->LoadCharacterSet(ppModel, line, input);
			}

			// モーションの設定の読み込み
			pLoder->LoadMotionSet(pLoder, File, line, nNumMotion);

			// モーションの数が最大まで行ったら
			if (pLoder->GetNumMotion() >= nNumMotion)
			{
				break;
			}
		}

		// 情報の受け渡し
		Info = pLoder->m_aInfo;

		// モデルの情報の受け渡し
		*OutNumModel = nNumModel;

		// ファイルを閉じる
		File.close();

		return pLoder;
	}
	else
	{
		// メッセージボックス
		MessageBox(NULL, pFileName, "ファイルが開けませんでした", MB_OK);

		return nullptr;
	}

}

//===================================================
// モデルのロード処理
//===================================================
bool CLoderText::LoadModel(CModel** ppModel, const int nMaxSize, int nCnt, string input, string line)
{
	if (line.find("MODEL_FILENAME") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// モデルの名前格納用変数
		string modelName;

		// 数値を代入する
		value_Input >> modelName;

		// モデルの名前を代入
		const char* MODEL_NAME = modelName.c_str();

		// サイズ以上に読み込むとエラーが出るため制限
		if (nCnt <= nMaxSize - 1)
		{
			// モデルの生成
			ppModel[nCnt] = CModel::Create(MODEL_NAME);

			return true;
		}
		else
		{
			MessageBox(NULL, MODEL_NAME, "これ以上読み込めません", MB_OK);
		}
	}
	return false;
}

//===================================================
// キャラクターのロード処理
//===================================================
bool CLoderText::LoadCharacterSet(CModel** ppModel, string line, string input)
{
	static int nIdx = 0;
	int nNumParts = 0;
	int nParent = 0;
	D3DXVECTOR3 offset = VEC3_NULL;

	if (line.find("NUM_PARTS") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> nNumParts;

		// パーツの最大数を設定
		SetNumModel(nNumParts);
	}

	if (line.find("INDEX") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> nIdx;
	}

	if (line.find("PARENT") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> nParent;

		if (nParent != -1)
		{// 親が存在していたら
			// 親のモデルの設定
			ppModel[nIdx]->SetParent(ppModel[nParent]);
		}
		else
		{// 親が存在していなかったら
			ppModel[nIdx]->SetParent(nullptr);
		}
	}

	if (line.find("POS") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffPos(offset);
	}

	if (line.find("ROT") != string::npos)
	{
		// 数値を読み込む準備
		istringstream value_Input = SetInputvalue(input);

		// 数値を代入する
		value_Input >> offset.x;
		value_Input >> offset.y;
		value_Input >> offset.z;

		ppModel[nIdx]->SetOffRot(offset);
	}

	if (line.find("END_CHARACTERSET") != string::npos)
	{
		return true;
	}

	return false;
}

//===================================================
// モーションのロード処理
//===================================================
void CLoderText::LoadMotionSet(CLoderText* pLoader, ifstream& File, string nowLine, const int nNumMotion)
{
	string line, input;

	int loop = 0;
	int nKey = 0;
	int nCntModel = 0;

	if (nowLine.find("MOTIONSET") != string::npos)
	{
		while (getline(File, line))
		{
			int nNum = GetNumMotion();

			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			if (line.find("LOOP") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> loop;

				// ループするかどうか
				m_aInfo[nNum].bLoop = (loop == 1) ? true : false;
			}

			if (line.find("NUM_KEY") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);
				
				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);
				
				// 数値を代入する
				value_Input >> m_aInfo[nNum].nNumkey;
				
				// キーの総数を代入
				int nNumKey = m_aInfo[nNum].nNumkey;
				
				m_aInfo[nNum].aKeyInfo.resize(nNumKey);
				
				// キーのサイズの確保
				for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
				{
					m_aInfo[nNum].aKeyInfo[nCntKey].aKey.resize(GetNumModel());
				}
			}

			if (line.find("FRAME") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].nFrame;
			}

			if (line.find("POS") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosX;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosY;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fPosZ;
			}
			if (line.find("ROT") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotX;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotY;
				value_Input >> m_aInfo[nNum].aKeyInfo[nKey].aKey[nCntModel].fRotZ;
			}

			if (line.find("END_KEY") != string::npos)
			{
				nCntModel++;

				nCntModel = Clamp(nCntModel, 0, pLoader->GetNumModel() - 1);
			}

			if (line.find("END_KEYSET") != string::npos)
			{
				nKey++;
				nCntModel = NULL;
			}
			if (line.find("END_MOTIONSET") != string::npos)
			{
				nKey = NULL;

				if (nNum <= nNumMotion - 1)
				{
					nNum++;

					SetNumMotion(nNum);
				}
				else
				{
				}


				break;
			}
		}
	}
}

//===================================================
// inputから数値を取り出処理
//===================================================
istringstream CLoderText::SetInputvalue(string input)
{
	// 先頭の = を消す
	input.erase(0, input.find_first_not_of(" = "));

	// inputから数値を取り出す準備
	istringstream value_Input(input);

	return value_Input;
}
