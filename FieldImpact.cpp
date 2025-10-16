//================================================
//
// フィールドの衝撃波の処理をするクラス [FieldImpact.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "FieldImpact.h"
#include "transform.h"
#include "Collider.h"
#include "Collision.h"
#include "dust.h"
#include "sound.h"
#include "manager.h"
#include "player.h"
#include "game.h"
#include "impact.h"
#include "meshfield.h"
#include "math.h"
#include "velocity.h"

//************************************************
// 名前空間
//************************************************
using namespace math;		// 名前空間mathを使用する

//************************************************
// 定数宣言
//************************************************
namespace
{
	constexpr int NUM_SIRCLE = 5;				// メッシュサークルを出す数
	constexpr float AUDIO_DISTANCE = 2000.0f;	// 音の聞こえる距離
}

//================================================
// コンストラクタ
//================================================
CMeshFieldImpact::CMeshFieldImpact()
{
	ZeroMemory(&m_Config, sizeof(m_Config));
	ZeroMemory(&m_Info, sizeof(m_Info));
	m_pMove = nullptr;
}

//================================================
// デストラクタ
//================================================
CMeshFieldImpact::~CMeshFieldImpact()
{
}

//================================================
// 生成処理
//================================================
CMeshFieldImpact* CMeshFieldImpact::Create(Config config)
{
	// 衝撃波の生成
	CMeshFieldImpact* pImpact = new CMeshFieldImpact;

	// 移動量の生成
	pImpact->m_pMove = new CVelocity;

	// 当たり判定の生成
	pImpact->m_pSphere = CColliderSphere::Create(config.pos, config.fRadius);

	// 方向ベクトルにする
	D3DXVec3Normalize(&config.dir, &config.dir);

	// yは考慮しない
	config.dir.y = 0.0f;

	// 移動量の設定
	pImpact->m_pMove->Set(config.dir * config.fSpeed);

	// 設定処理
	pImpact->m_Config = config;

	return pImpact;
}

//================================================
// 終了処理
//================================================
void CMeshFieldImpact::Uninit(void)
{
	// 移動量の破棄
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}
}

//================================================
// 更新処理
//================================================
bool CMeshFieldImpact::Update(CMeshField* pMeshField, const int nNumVtx)
{
	// nullじゃないなら
	if (m_pMove != nullptr)
	{
		// 移動量の更新処理
		m_Config.pos += m_pMove->Get();
	}

	// 吹っ飛び量を選出
	float dir = Random(5.0f, 20.0f);
	float Jump = Random(25.0f, 40.0f);

	// ランダムな角度の選出
	float fAngle = Random(-314.0f, 314.0f) * 0.01f;

	// 方向に応じた吹っ飛び量を計算
	float fMoveX = sinf(fAngle) * dir;
	float fMoveZ = cosf(fAngle) * dir;

	// 寿命を選出
	int nLife = rand() % 120 + 60;

	// 瓦礫を生成
	CRubble::Create(m_Config.pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, CRubble::TYPE_THREE);

	// nullじゃないなら
	if (m_pSphere != nullptr)
	{
		// 位置の更新処理
		m_pSphere->SetPosition(m_Config.pos);
	}

	// インパクトを出すタイミングを求める
	int SetImpact = m_Config.nTime / NUM_SIRCLE;

	// いちばん最初に出す、
	if (SetImpact != 0 && (m_Info.nCounter % SetImpact == 0 || m_Info.nCounter == 0))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// プレイヤーの取得
			CPlayer* pPlayer = CGame::GetPlayer();

			// 位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 距離を求める
			float fDistance = GetDistance(playerPos - m_Config.pos);

			// ボリュームの設定
			float fRateVolume = 1.0f - (fDistance / AUDIO_DISTANCE);

			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WARK003, fRateVolume);
		}

		// 角度を求める
		float rotY = atan2f(m_pMove->Get().x, m_pMove->Get().z);

		// サークルの生成
		auto pCircle = CMeshCircle::Create(m_Config.Circlecol, m_Config.pos, 10.0f, 50.0f, 32);

		// サークルの設定
		pCircle->SetCircle(50.0f, 10.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, rotY, 0.0f));
	}

	// 波のカウンターを進める
	m_Info.nCounter++;


	// 頂点数分調べる
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// 頂点の位置の取得
		D3DXVECTOR3 vtxPos = pMeshField->GetVtxPos(nCnt);

		// nullじゃないなら
		if (m_pSphere != nullptr)
		{
			// コライダーの作成
			CColliderSphere spere = m_pSphere->CreateCollider(vtxPos, 50.0f);

			// 円と円の判定
			if (CCollisionSphere::Collision(&spere, m_pSphere.get()))
			{
				//頂点カラーの設定
				pMeshField->SetColor(D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f), nCnt);

				vtxPos.y += (m_Config.fHeight - vtxPos.y) * 0.3f;
			}
			else
			{
				// 目的の高さに近づける
				vtxPos.y += (0.0f - vtxPos.y) * 0.05f;
			}

			pMeshField->SetVtxPos(vtxPos, nCnt);
		}
	}

	// 最大値だったら
	if (m_Info.nCounter >= m_Config.nTime)
	{
		// カウンターをリセット
		m_Info.nCounter = 0;

		return false;
	}

	return true;
}

//================================================
// 当たり判定
//================================================
bool CMeshFieldImpact::Collision(const D3DXVECTOR3 pos, const float fRadius, const OBJ myObj, D3DXVECTOR3* pFirstPos, D3DXVECTOR3* pImpactPos)
{
	// 位置を保存
	D3DXVECTOR3 NewPos = pos;

	// y座標を考慮しない
	NewPos.y = 0.0f;

	// コライダーの作成
	CColliderSphere sphere = m_pSphere->CreateCollider(NewPos, fRadius);

	// 当たり判定
	if (CCollisionSphere::Collision(&sphere, m_pSphere.get()) && myObj != m_Config.ObjType)
	{
		if (pFirstPos != nullptr)
		{
			// 発射地点を設定
			*pFirstPos = m_Config.FirstPos;
		}

		if (pImpactPos != nullptr)
		{
			// 衝撃波の位置を設定
			*pImpactPos = m_Config.pos;
		}
		return true;
	}

	return false;
}

//================================================
// 設定処理
//================================================
void CMeshFieldImpact::Reset(D3DXVECTOR3 dir, const OBJ obj, const D3DXVECTOR3 FirstPos, const D3DXCOLOR Circlecol)
{
	m_Info.nCounter = 0; 				 // カウンターをリセット
	m_Config.FirstPos = FirstPos; 		 // 発射地点を設定
	m_Config.ObjType = obj;				 // 発射したオブジェクトを設定
	D3DXVec3Normalize(&dir, &dir);		 // 方向の正規化
	m_pMove->Set(dir * m_Config.fSpeed); // 移動量を設定
	m_Config.Circlecol = Circlecol;		 // サークルの色
}