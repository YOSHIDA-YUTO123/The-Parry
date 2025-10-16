//===================================================
//
// オープニングの敵 [OpeningEnemy.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "OpningEnemy.h"
#include "enemy.h"
#include "transform.h"
#include "opening.h"
#include "meshfield.h"
#include "OpeningCamera.h"
#include "motion.h"
#include "dust.h"
#include "manager.h"
#include "renderer.h"
#include "fade.h"
#include "game.h"
#include "sound.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
COpeningEnemy::COpeningEnemy() : CCharacter3D(TYPE_OPENING_ENEMY)
{
	m_nNextSceneCnt = NULL;
	m_bLanding = true;
	m_move = VEC3_NULL;
}

//===================================================
// デストラクタ
//===================================================
COpeningEnemy::~COpeningEnemy()
{
}

//===================================================
// 生成処理
//===================================================
COpeningEnemy* COpeningEnemy::Create(const D3DXVECTOR3 pos, const float fAngle)
{
	COpeningEnemy* pEnemy = nullptr;

	// 敵オブジェクトの生成
	pEnemy = new COpeningEnemy;

	if (pEnemy == nullptr) return nullptr;

	pEnemy->SetPosition(pos);
	pEnemy->Init();
	pEnemy->SetRotation(D3DXVECTOR3(0.0f,fAngle,0.0f));

	return pEnemy;
}

//===================================================
// 初期化処理
//===================================================
HRESULT COpeningEnemy::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter3D::Init()))
	{
		return E_FAIL;
	}

	// モーションロード処理
	CCharacter3D::LoadMotion("motionEnemy000.txt", CEnemy::MOTIONTYPE_MAX);

	// キャラクターの設定処理
	CCharacter3D::SetCharacter(-1, 0.0f, D3DXVECTOR3(10.0f, 1.0f, 10.0f), D3DXVECTOR3(100.0f, 400.0f, 100.0f));

	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return E_FAIL;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void COpeningEnemy::Uninit(void)
{
	// 終了処理
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void COpeningEnemy::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	// 位置の更新
	pos += m_move;

	// メッシュフィールドの取得
	CMeshField *pMeshField = COpening::GetField();

	// カメラの取得
	COpeningCamera* pCamera = COpening::GetCamera();

	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();

	// フェードの取得
	CFade* pFade = CManager::GetFade();

	// 取得できなかったら処理しない
	if (pMeshField == nullptr) return;

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 取得できなかったら処理しない
	if (pFade == nullptr) return;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	float fHeight = 0.0f;

	// 地面との当たり判定
	if (pMeshField != nullptr && pMeshField->Collision(pos, &fHeight))
	{
		if (!m_bLanding)
		{
			// 波の発生処理
			SetFieldWave(pMeshField,pos);

			// モーションの設定
			pMotion->SetMotion(CEnemy::MOTIONTYPE_LANDING, true, 10);

			// カメラの揺れの設定
			pCamera->SetShake(60, 20);

			m_bLanding = true;

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_IMPACT000);
			}
		}

		// 高さの設定
		pos.y = fHeight;
	}
	else
	{
		m_bLanding = false;
	}

	if (pCamera->GetState() == pCamera->STATE_TRACKING)
	{
		// 重力の設定
		m_move.y -= MAX_GRABITY;
	}
	else
	{
		// モーションの設定
		pMotion->SetMotion(CEnemy::MOTIONTYPE_JUMP, true, 1);
	}

	// 着地モーションが終わったら
	if (pMotion->GetType() == CEnemy::MOTIONTYPE_LANDING && pMotion->IsEndMotion())
	{
		// 叫びモーションに以降
		pMotion->SetMotion(CEnemy::MOTIONTYPE_ROAR, true, 15);
	}

	// 叫びの63f目になったら
	if (pMotion->IsEventFrame(63,63,CEnemy::MOTIONTYPE_ROAR))
	{
		if (pSound != nullptr)
		{
			// 咆哮
			pSound->Play(CSound::SOUND_LABEL_ROAR);
		}

		// カメラの揺れの設定
		pCamera->SetShake(180, 30);

		// レンダラーの取得
		CRenderer* pRenderer = CManager::GetRenderer();

		if (pRenderer != nullptr)
		{
			// ブラーをオン
			pRenderer->onEffect(0.9f);
		}
	}

	// 叫びが終わったら
	if (pMotion->GetType() == CEnemy::MOTIONTYPE_ROAR && pMotion->IsEndMotion())
	{
		// レンダラーの取得
		CRenderer* pRenderer = CManager::GetRenderer();

		if (pRenderer != nullptr)
		{
			// ブラーをオン
			pRenderer->offEffect();
		}

		pCamera->SetMove(D3DXVECTOR3(0.0f, 0.0f, 3.0f));
	}

	if (pMotion->GetType() == CEnemy::MOTIONTYPE_NEUTRAL)
	{
		m_nNextSceneCnt++;

		if (m_nNextSceneCnt >= 120)
		{
			// シーンの遷移
			pFade->SetFade(make_unique<CGame>());
		}
	}
	// モーションの更新
	CCharacter3D::UpdateMotion();

	// 更新処理
	CCharacter3D::Update();

	// 位置の設定
	CCharacter3D::SetPosition(pos);

	D3DXVECTOR3 modepos = GetModelPos(1);

	// カメラの追従
	pCamera->SetTracking(modepos);
}

//===================================================
// 描画処理
//===================================================
void COpeningEnemy::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();
}

//===================================================
// メッシュフィールドの波の処理
//===================================================
void COpeningEnemy::SetFieldWave(CMeshField* pMeshField,const D3DXVECTOR3 pos)
{
	// フィールドの波の設定
	CMeshFieldWave::Config config = { pos,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

	// 地面に波を発生させる
	pMeshField->SetWave(config);

	// 発生させる瓦礫の数
	const int NUM_RUBBLE = 16;

	// 瓦礫の数分出す
	for (int nCnt = 0; nCnt < NUM_RUBBLE; nCnt++)
	{
		// 分割に応じた方向を求める
		float fAngle = (D3DX_PI * 2.0f) / NUM_RUBBLE * nCnt;

		// 吹っ飛び量を選出
		float dir = rand() % 15 + 5.0f;
		float Jump = rand() % 15 + 25.0f;

		// 方向に応じた吹っ飛び量を計算
		float fMoveX = sinf(fAngle) * dir;
		float fMoveZ = cosf(fAngle) * dir;

		// 寿命を選出
		int nLife = rand() % 120 + 60;

		// 種類を選出
		int nType = rand() % CRubble::TYPE_MAX;

		// 瓦礫を生成
		CRubble::Create(pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, nType);
	}
}
