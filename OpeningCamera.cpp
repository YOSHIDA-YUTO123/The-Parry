//===================================================
//
// オープニングカメラ [openingcamera.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "OpeningCamera.h"
#include "opening.h"
#include "FadeEffect.h"
#include "manager.h"
#include "light.h"
#include "LoadManager.h"
#include <memory>
#include "OpningEnemy.h"
#include "fade.h"
#include "tutorial.h"
#include "sound.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
COpeningCamera::COpeningCamera()
{
	m_bDestPos = false;
	m_state = STATE_NORMAL;
	m_nKey = NULL;
	m_nextKey = NULL;
	m_nCounter = NULL;
	m_nShakeRange = NULL;
	m_nShakeTime = NULL;
	m_bTutorial = false;
	m_bReset = false;
	m_move = VEC3_NULL;
	m_bFlash = false;
	m_fCounter = NULL;
	m_fMoveShakeSpeed = NULL;
}

//===================================================
// デストラクタ
//===================================================
COpeningCamera::~COpeningCamera()
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->StopSound();
	}

	// 値のクリア
	m_Info.akey_info.clear();
}

//===================================================
// 初期化処理
//===================================================
HRESULT COpeningCamera::Init(void)
{
	// 初期化処理
	CCamera::Init();

	// カメラの設定処理
	CCamera::SetCamera(D3DXVECTOR3(5200.53f, 178.72f, 5.11f), D3DXVECTOR3(340.91f, 144.57f, -4.34f), D3DXVECTOR3(1.67f, 1.60f, 0.0f));
	
	// 移動量の設定
	SetMove(D3DXVECTOR3(3.0f, 0.0f, 0.0f));

	// カメラのキーのロード
	Load();

	m_fMoveShakeSpeed = 0.15f;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void COpeningCamera::Uninit(void)
{
	// 終了処理
	CCamera::Uninit();
}

//===================================================
// 更新処理
//===================================================
void COpeningCamera::Update(void)
{
#ifdef _DEBUG

	//// マウスの視点移動
	//CCamera::MouseView();

#endif // _DEBUG

	auto pFadeEffect = COpening::GetFadeEffect();

	// 視点の取得
	D3DXVECTOR3 posV = CCamera::GetPosV();
	D3DXVECTOR3 posR = CCamera::GetPosR();

	// 位置の更新
	posV += m_move;
	posR += m_move;

	// ライトの取得
	CLight* pLight = CManager::GetLight();

	if (pFadeEffect != nullptr)
	{
		// カメラが特定の位置まで来たら
		if (posV.x >= FADEOUT_POS && !m_bFlash)
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			if (pFade != nullptr && m_bTutorial)
			{
				pFade->SetFade(make_unique<CTutorial>(), D3DXCOLOR(1.0f,1.0f,1.0f,0.0f));
				return;
			}
			// フェードする
			pFadeEffect->SetState(pFadeEffect->FADE_OUT);

			// ライトをリセット
			pLight->Init();

			m_bFlash = true;
		}

		if (m_bFlash && pFadeEffect->GetState() == pFadeEffect->FADE_NONE && !m_bReset)
		{
			m_bReset = true;

			pFadeEffect->SetState(pFadeEffect->FADE_IN);

			// ライトの設定処理
			pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, 0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
			pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, -0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));

			posV = D3DXVECTOR3(0.0f, 200.0f, -1500.0f);
			posR = VEC3_NULL;

			// 敵の生成
			COpeningEnemy::Create(D3DXVECTOR3(0.0f, 2000.0f, -100.0f), 0.0f);

			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_BIRDSOUND);

				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WIND);
			}

			// 移動量の設定
			m_move = D3DXVECTOR3(0.0f, 0.0f, 3.0f);
		}
	}

	// リセットしたら
	if (m_bReset)
	{
		if (posV.z >= -1200.0f && !m_bDestPos)
		{
			// 目的の位置に到達した
			m_bDestPos = true;

			m_state = STATE_ANIM;

			m_move = VEC3_NULL;
		}

		// 最後まで行ったら
		if (m_nKey >= m_Info.nNumKey - 1)
		{
			// 追従状態にする
			m_state = STATE_TRACKING;
		}

		// キーの更新処理
		UpdateKey(&posR);
	}

	// 移動時の揺れの更新
	UpdateMoveShake(&posV);

	// 位置の設定
	CCamera::SetPosV(posV);
	CCamera::SetPosR(posR);

	// 揺れの更新
	UpdateShake();
}

//===================================================
// 追従処理
//===================================================
void COpeningCamera::SetTracking(const D3DXVECTOR3 posRDest)
{
	// 状態が追従じゃないなら
	if (m_state != STATE_TRACKING) return;

	//// 
	//SetDestPosR(posRDest);

	D3DXVECTOR3 posV = CCamera::GetPosV();

	//// Y軸の向きの取得
	//float rotY = CCamera::GetRotaition().y;

	//// 距離の取得
	//float fDistance = CCamera::GetDistance();

	// カメラの位置の補間
	CCamera::LerpPos(posRDest, posV, 0.1f);
}

//===================================================
// 揺れの設定
//===================================================
void COpeningCamera::SetShake(const int nShakeTime, const int nRange)
{
	m_nShakeTime = nShakeTime;
	m_nShakeRange = nRange;
}

//===================================================
// 移動量の設定
//===================================================
void COpeningCamera::SetMove(const D3DXVECTOR3 move)
{
	m_state = STATE_MOVE;
	m_move = move;
}

//===================================================
// ロード処理
//===================================================
void COpeningCamera::Load(void)
{
	// ファイルをロードする
	ifstream File("data/CAMERA/camera_key.txt");
	string line;
	string input;

	int nKey = 0; // キー

	if (File.is_open() == true)
	{
		// ロードマネージャーの生成
		auto pLoadManager = make_unique<CLoadManager>();

		// 最後の行になるまで読み込む
		while (getline(File, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			if (line.find("NUM_KEY") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoadManager->SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_Info.nNumKey;

				// サイズの確保
				m_Info.akey_info.resize(m_Info.nNumKey);
			}

			if (line.find("FRAME") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoadManager->SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_Info.akey_info[nKey].nFrame;
			}

			if (line.find("POS") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoadManager->SetInputvalue(input);

				// 数値を代入する
				value_Input >> m_Info.akey_info[nKey].DestPosR.x;
				value_Input >> m_Info.akey_info[nKey].DestPosR.y;
				value_Input >> m_Info.akey_info[nKey].DestPosR.z;
			}

			if (line.find("END_KEYSET") != string::npos)
			{
				// キーを増やす
				nKey++;
			}
		}

		// 破棄
		pLoadManager.reset();

		// ファイルを閉じる
		File.close();
		File.clear();
	}
}

//===================================================
// キーの更新処理
//===================================================
void COpeningCamera::UpdateKey(D3DXVECTOR3 *pPosR)
{
	if (m_state != STATE_ANIM) return;

	// 次のキーを進める
	m_nextKey = (m_nKey + 1) % m_Info.nNumKey;

	// 次のキーまでの差分を求める
	D3DXVECTOR3 Diff = m_Info.akey_info[m_nextKey].DestPosR - m_Info.akey_info[m_nKey].DestPosR;

	// 割合を求める
	float fRateFrame = m_nCounter / static_cast<float>(m_Info.akey_info[m_nKey].nFrame);

	// 補間処理
	pPosR->x = m_Info.akey_info[m_nKey].DestPosR.x + Diff.x * fRateFrame;
	pPosR->y = m_Info.akey_info[m_nKey].DestPosR.y + Diff.y * fRateFrame;
	pPosR->z = m_Info.akey_info[m_nKey].DestPosR.z + Diff.z * fRateFrame;

	// カウンターがフレームを超えたら
	if (m_nCounter >= m_Info.akey_info[m_nKey].nFrame)
	{
		m_nKey = (m_nKey + 1) % m_Info.nNumKey;

		m_nCounter = NULL;
	}
	else
	{
		m_nCounter++;
	}
}

//===================================================
// カメラの揺れの更新処理
//===================================================
void COpeningCamera::UpdateShake(void)
{
	if (m_nShakeTime <= 0) return;

	m_nShakeTime--;

	// 注視点
	D3DXVECTOR3 posR = CCamera::GetPosR();

	D3DXVECTOR3 posRWk = posR;

	// 揺れの最大
	int nRangeMax = m_nShakeRange * 2;
	float fRangeMin = static_cast<float>(m_nShakeRange);
	
	posRWk.x = posR.x + static_cast<float>(rand() % nRangeMax) - fRangeMin;
	posRWk.y = posR.y + static_cast<float>(rand() % nRangeMax) - fRangeMin;
	posRWk.z = posR.z + static_cast<float>(rand() % nRangeMax) - fRangeMin;

	// 注視点の設定
	CCamera::SetPosR(posRWk);
}

//===================================================
// 移動状態時のカメラの揺れの更新
//===================================================
void COpeningCamera::UpdateMoveShake(D3DXVECTOR3 *pPosV)
{
	// 状態が移動じゃないなら処理しない
	if (m_state != STATE_MOVE) return;

	// 時間を加算
	m_fCounter += m_fMoveShakeSpeed;

	// 今の位置
	float fPosVNow = pPosV->y;

	// サインカーブ
	float Sinf = sinf(m_fCounter);

	// 揺れを設定
	pPosV->y = fPosVNow + (Sinf * 2.0f);

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (Sinf <= -0.99f)
	{
		if (pSound != nullptr)
		{
			pSound->Play(CSound::SOUND_LABEL_WARK000);
		}
	}
}
