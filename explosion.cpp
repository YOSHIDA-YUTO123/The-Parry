//===================================================
//
// 爆発,煙 [explosion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "explosion.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "game.h"

using namespace Const;							// 名前空間Constを使用する

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CExplosionManager* CExplosionManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// コンストラクタ
//===================================================
CExplosion::CExplosion(int nPriority) : CBillboardAnimation(nPriority)
{
	m_fDestMoveY = NULL;
	m_col = WHITE;
	m_move = VEC3_NULL;
}

//===================================================
// デストラクタ
//===================================================
CExplosion::~CExplosion()
{
}

//===================================================
// 生成処理
//===================================================
CExplosion* CExplosion::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed, const TYPE type)
{
	// 爆発の生成
	CExplosion* pExplosion = new CExplosion;

	if (pExplosion == nullptr) return nullptr;

	pExplosion->m_col = col;
	pExplosion->m_type = type;
	pExplosion->SetAnim(nAnimSpeed, U, V, false);
	pExplosion->SetPosition(pos);
	pExplosion->SetSize(size);
	pExplosion->Init();
	pExplosion->SetColor(col);

	return pExplosion;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CExplosion::Init(void)
{
	// 初期化処理
	if (FAILED(CBillboardAnimation::Init()))
	{
		return E_FAIL;
	}

	// モデルのパス
	static const char* pTexturePath[TYPE_MAX] =
	{
		"data/TEXTURE/explosion/explosion001.png",
		"data/TEXTURE/explosion/explosion002.png",
		"data/TEXTURE/explosion/explosion.png",
		"data/TEXTURE/explosion/explosion003.png",
	};

	// テクスチャのIDの設定
	CBillboardAnimation::SetTextureID(pTexturePath[m_type]);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CExplosion::Uninit(void)
{
	// 終了処理
	CBillboardAnimation::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CExplosion::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CBillboardAnimation::GetPosition();

	// 移動量の減衰
	m_move.x += (0.0f - m_move.x) * 0.04f;
	m_move.z += (0.0f - m_move.z) * 0.04f;

	// 高さを目的の高さに近づける
	m_move.y += (m_fDestMoveY - m_move.y) * 0.1f;

	// 位置の更新
	pos += m_move;

	// 更新処理
	CBillboardAnimation::Update();

	// 位置の設定
	CBillboardAnimation::SetPosition(pos);
}

//===================================================
// 描画処理
//===================================================
void CExplosion::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 更新処理
	CBillboardAnimation::Draw();

	// Zテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//===================================================
// パラメータの設定
//===================================================
void CExplosion::SetParam(const D3DXVECTOR3 move)
{
	m_move.x = move.x;
	m_move.z = move.z;
	m_fDestMoveY = move.y;
}

//===================================================
// コンストラクタ
//===================================================
CExplosionManager::CExplosionManager()
{
	m_pos = VEC3_NULL;
	m_Size = VEC2_NULL;
	m_col = WHITE;
	m_nUV = 1;
	m_nHV = 1;
	m_nAnimSpeed = NULL;
}

//===================================================
// デストラクタ
//===================================================
CExplosionManager::~CExplosionManager()
{
}

//===================================================
// パラメータの設定
//===================================================
CExplosionManager* CExplosionManager::SetParam(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed)
{
	if (m_pInstance == nullptr)
	{
		// 自分の生成
		m_pInstance = new CExplosionManager;
		m_pInstance->m_pos = pos;
		m_pInstance->m_Size = size;
		m_pInstance->m_col = col;
		m_pInstance->m_nUV = U;
		m_pInstance->m_nHV = V;
		m_pInstance->m_nAnimSpeed = nAnimSpeed;
	}

	return m_pInstance;
}

//===================================================
// 生成処理
//===================================================
void CExplosionManager::Create(const CExplosion::TYPE type, const D3DXVECTOR2 move, const int nNum)
{
	if (m_pInstance == nullptr) return;

	// 総数分回す
	for (int nCnt = 0; nCnt < nNum; nCnt++)
	{
		// 一周を分割で割る
		float fAngle = (D3DX_PI * 2.0f) / nNum * nCnt;

		// ローカルに代入
		D3DXVECTOR3 pos = m_pInstance->m_pos;
		D3DXVECTOR2 size = m_pInstance->m_Size;
		int nAnimSpeed = m_pInstance->m_nAnimSpeed;

		D3DXCOLOR col = m_pInstance->m_col;
		int U = m_pInstance->m_nUV;
		int V = m_pInstance->m_nHV;

		// ランダムな大きさの選出
		float fSize = D3DXVec2Length(&size);
		float fRandomSize = static_cast<float>(rand() % static_cast<int>(fSize)) + fSize * 0.5f;

		int nRandomAnimSpeed = rand() % nAnimSpeed + (nAnimSpeed / 2);

		// 爆発の生成
		auto pExplosion = CExplosion::Create(pos, D3DXVECTOR2(fRandomSize, fRandomSize), col, U, V, nRandomAnimSpeed, type);

		if (pExplosion != nullptr)
		{
			// XとZの移動量
			float fMoveXZ = static_cast<float>(rand() % static_cast<int>(move.x) + move.x * 0.5f);
			float fMoveY = static_cast<float>(rand() % static_cast<int>(move.y) + move.y * 0.5f);

			// パラメータの設定
			pExplosion->SetParam(D3DXVECTOR3(sinf(fAngle) * fMoveXZ, fMoveY, cosf(fAngle) * fMoveXZ));
		}
	}

	// 自分の破棄
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
