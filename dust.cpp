//===================================================
//
// 瓦礫 [dust.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "dust.h"
#include "meshfield.h"
#include "manager.h"
#include "math.h"
#include "game.h"
#include "opening.h"
#include "tutorial.h"
#include "transform.h"
#include "shadow.h"
#include "explosion.h"
#include "velocity.h"

constexpr float SHADOW_SIZE_000 = 50.0f;	 // 影の大きさ000
constexpr float SHADOW_SIZE_001 = 25.0f;	 // 影の大きさ001
constexpr float SHADOW_SIZE_002 = 10.0f;	 // 影の大きさ002
constexpr float SHADOW_MAX_HEIGHT = 700.0f;  // 影が見える最大の高さ
constexpr float SHADOW_A_LEVEL = 0.9f;       // 影のアルファ値のオフセット

//===================================================
// コンストラクタ
//===================================================
CRubble::CRubble(int nPriority) : CObjectX(nPriority)
{
	m_nLife = NULL;
	m_pMove = nullptr;
	m_nMaxLife = NULL;
	m_fDecAlv = NULL;
	m_fShadowSize = NULL;
}

//===================================================
// デストラクタ
//===================================================
CRubble::~CRubble()
{
}

//===================================================
// 生成処理
//===================================================
CRubble* CRubble::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 dir, const int nLife,int nType)
{
	CRubble* pDust = nullptr;

	// 瓦礫の生成
	pDust = new CRubble;

	if (pDust == nullptr) return nullptr;

	pDust->SetPosition(pos);

	// ファイルパス
	static const char* pModelPath[TYPE_MAX] =
	{
		"data/MODEL/dust/dust000.x",
		"data/MODEL/dust/dust001.x",
		"data/MODEL/dust/dust002.x",
		"data/MODEL/dust/dust004.x",
		"data/MODEL/dust/dust005.x",
		"data/MODEL/dust/dust006.x",
		"data/MODEL/dust/dust007.x",
	};

	switch (nType)
	{
	case TYPE::TYPE_ONE:
		pDust->m_fShadowSize = SHADOW_SIZE_000;
		break;
	case TYPE::TYPE_TWO:
		pDust->m_fShadowSize = SHADOW_SIZE_001;
		break;
	case TYPE::TYPE_THREE:
		pDust->m_fShadowSize = SHADOW_SIZE_002;
		break;
	case TYPE::TYPE_FOUR:
		pDust->m_fShadowSize = SHADOW_SIZE_002;
		break;
	default:
		break;
	}
	
	// モデルの読み込み
	pDust->LoadModel(pModelPath[nType]);

	// 初期化に失敗したら
	if (FAILED(pDust->Init()))
	{
		// 終了処理
		pDust->Uninit();
		pDust = nullptr;

		return nullptr;
	}
	pDust->m_nLife = nLife;
	pDust->m_nMaxLife = nLife;
	pDust->m_fDecAlv = 1.0f / nLife;

	pDust->m_pMove->Set(dir);

	return pDust;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CRubble::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	// 移動量の生成
	m_pMove = std::make_unique<CVelocity>();

	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	// 影の生成
	m_pShadow = CShadow::Create(pos, m_fShadowSize, m_fShadowSize, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CRubble::Uninit(void)
{
	if (m_pShadow != nullptr)
	{
		// 影の破棄
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

	// 終了処理
	CObjectX::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CRubble::Update(void)
{
	// 現在のモードの取得
	CScene::MODE mode = CManager::GetMode();

	// フィールドの取得
	CMeshField* pMesh = nullptr;

	switch (mode)
	{
	case CScene::MODE_GAME:
		pMesh = CGame::GetField();
		break;
	case CScene::MODE_TUTORIAL:
		pMesh = CTutorial::GetField();
		break;
	case CScene::MODE_OPENING:
		pMesh = COpening::GetField();
		break;
	default:
		break;
	}

	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	if (m_pMove != nullptr)
	{
		// 位置の更新処理
		pos += m_pMove->Get();
	}

	float fHeight = 0.0f;

	// 地面の当たり判定
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;

		// 地面の法線の取得
		D3DXVECTOR3 nor = pMesh->GetNor();

		if (m_pMove != nullptr)
		{
			// バウンドの設定
			m_pMove->Bound(nor);
		}
	}

	D3DXVECTOR3 up = pMesh->GetNor(); // 上方向ベクトル

	D3DXVECTOR3 dir = m_pMove->Get();

	D3DXVec3Normalize(&dir, &dir);

	D3DXVECTOR3 axis;
	D3DXVec3Cross(&axis, &up, &dir);

	float dot = D3DXVec3Dot(&up, &dir);
	float fAngle = acosf(dot);

	D3DXMATRIX mtxRot; // 計算用回転行列

	// 任意の軸から回転行列を求める
	D3DXMatrixRotationAxis(&mtxRot, &axis, fAngle);

	// 回転行列をオイラー角に変換
	D3DXVECTOR3 rot = math::MatrixToEulerXYZ(mtxRot);

	// 重力の設定
	m_pMove->Gravity(-Const::MAX_GRABITY);

	// 影の更新処理
	if (m_pShadow != nullptr)
	{
		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 				// 地面の法線ベクトルの取得
		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 上方向ベクトルの作成

		// 地面の角度に合わせた角度を設定
		m_pShadow->SetFieldAngle(FieldNor, PlayerRay);

		// 影の設定処理
		m_pShadow->Update(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), m_fShadowSize, m_fShadowSize, SHADOW_MAX_HEIGHT, SHADOW_A_LEVEL);
	}

	// 位置の設定処理
	CObjectX::SetPosition(pos);

	// 向きの設定
	CObjectX::SetRotation(rot);
	
	m_nLife--;

	if (m_nLife <= 0)
	{
		Uninit();
	}
}

//===================================================
// 描画処理
//===================================================
void CRubble::Draw(void)
{
	// 割合を求める
	float fRate = m_nLife / (float)m_nMaxLife;

	// 影の描画処理
	if (m_pShadow != nullptr)
	{
		m_pShadow->Draw();
	}

	// 描画処理
	CObjectX::Draw(fRate);
}

//===================================================
// コンストラクタ
//===================================================
CRubbleManager::CRubbleManager()
{
}

//===================================================
// デストラクタ
//===================================================
CRubbleManager::~CRubbleManager()
{
}

//===================================================
// インパクト用の瓦礫の生成
//===================================================
void CRubbleManager::SetImpact(const D3DXVECTOR3 pos, const int nLife, const int nNum, const D3DXVECTOR2 move)
{
	// 総数分回す
	for (int nCnt = 0; nCnt < nNum; nCnt++)
	{
		// 角度の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / nNum * nCnt;

		// 移動量X,z
		int nMoveXZ = static_cast<int>(move.x);
		float fMoveXZHafe = move.x * 0.5f;

		// 移動量Y
		int nMoveY = static_cast<int>(move.y);
		float fMoveY = move.y * 0.5f;

		// 吹っ飛び量を選出
		float dir = rand() % nMoveXZ + fMoveXZHafe;
		float Jump = rand() % nMoveY + fMoveY;

		// 方向に応じた吹っ飛び量を計算
		float fMoveX = sinf(fAngle) * dir;
		float fMoveZ = cosf(fAngle) * dir;

		int nLifeHafe = nLife / 2;

		// 寿命を選出
		int nRubbleLife = rand() % nLife + nLifeHafe;

		// 種類
		int nType = rand() % CRubble::TYPE_FOUR;

		// 瓦礫を生成
		CRubble::Create(pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nRubbleLife, nType);
	}

}

//===================================================
// 爆発の設定
//===================================================
void CRubbleManager::SetExplosionTNT(const D3DXVECTOR3 pos, const int nLife, const int nNum, const D3DXVECTOR2 move)
{
	for (int nCnt = 0; nCnt < nNum; nCnt++)
	{
		// 角度の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / nNum * nCnt;

		// 移動量X,z
		int nMoveXZ = static_cast<int>(move.x);
		float fMoveXZHafe = move.x * 0.5f;

		// 移動量Y
		int nMoveY = static_cast<int>(move.y);
		float fMoveY = move.y * 0.5f;

		// 吹っ飛び量を選出
		float dir = rand() % nMoveXZ + fMoveXZHafe;
		float Jump = rand() % nMoveY + fMoveY;

		// 方向に応じた吹っ飛び量を計算
		float fMoveX = sinf(fAngle) * dir;
		float fMoveZ = cosf(fAngle) * dir;

		int nLifeHafe = nLife / 2;

		// 寿命を選出
		int nRubbleLife = rand() % nLife + nLifeHafe;

		// ランダムな値の選出
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(CRubble::TYPE_THREE, CRubble::TYPE_SEVEN);

		auto type = static_cast<CRubble::TYPE>(dist(gen));

		// 瓦礫を生成
		CRubble::Create(pos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nRubbleLife, type);
	}
}
