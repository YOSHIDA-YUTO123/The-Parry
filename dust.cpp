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
#include"math.h"

//***************************************************
// マクロ定義
//***************************************************
#define SHADOW_SIZE_000 (50.0f)	// 影の大きさ000
#define SHADOW_SIZE_001 (25.0f)	// 影の大きさ001
#define SHADOW_SIZE_002 (10.0f)	// 影の大きさ002

#define SHADOW_MAX_HEIGHT (700.0f)  // 影が見える最大の高さ
#define SHADOW_A_LEVEL (0.9f)       // 影のアルファ値のオフセット

//===================================================
// コンストラクタ
//===================================================
CRubble::CRubble(int nPriority) : CObject(nPriority)
{
	m_nLife = NULL;
	m_pObjectX = nullptr;
	ZeroMemory(&m_pos, sizeof(m_pos));
	ZeroMemory(&m_rot, sizeof(m_rot));
	ZeroMemory(&m_move, sizeof(m_move));
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

	// 優先順位の取得
	int nPriority = pDust->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pDust != nullptr)
	{
		// 自分のポインタの解放
		pDust->Release();

		// nullにする
		pDust = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	if (pDust == nullptr) return nullptr;

	switch (nType)
	{
	case TYPE::TYPE_ONE:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust000.x");
		pDust->m_fShadowSize = SHADOW_SIZE_000;
		break;
	case TYPE::TYPE_TWO:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust001.x");
		pDust->m_fShadowSize = SHADOW_SIZE_001;
		break;
	case TYPE::TYPE_THREE:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust002.x");
		pDust->m_fShadowSize = SHADOW_SIZE_002;
		break;
	default:
		break;
	}
	

	pDust->Init();
	pDust->m_nLife = nLife;
	pDust->m_nMaxLife = nLife;
	pDust->m_fDecAlv = 1.0f / nLife;

	pDust->m_move.Set(dir);

	pDust->m_pos.Set(pos);

	return pDust;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CRubble::Init(void)
{
	// 影の生成
	m_pShadow = CShadow::Create(m_pos.Get(), m_fShadowSize, m_fShadowSize, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CRubble::Uninit(void)
{
	// オブジェクトXの破棄
	if (m_pObjectX != nullptr)
	{
		m_pObjectX = nullptr;
	}

	// 影の破棄
	m_pShadow->Uninit();

	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CRubble::Update(void)
{
	CMeshField* pMesh = CManager::GetMeshField();

	//// 更新処理
	//m_pObjectX->Update();

	// 位置の更新処理
	m_pos.UpdatePosition(m_move.Get());

	// 位置の取得
	D3DXVECTOR3 pos = m_pos.Get();

	float fHeight = 0.0f;

	// 地面の当たり判定
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;

		// 地面の法線の取得
		D3DXVECTOR3 nor = pMesh->GetNor();

		// バウンドの設定
		m_move.Bound(nor);
	}

	D3DXVECTOR3 up = pMesh->GetNor(); // 上方向ベクトル

	D3DXVECTOR3 dir = m_move.Get();

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

	// 向きの設定
	m_rot.Set(rot);

	// 重力の設定
	m_move.Gravity(-MAX_GRABITY);

	// 影の更新処理
	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 				// 地面の法線ベクトルの取得
		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // 上方向ベクトルの作成

		// 地面の角度に合わせた角度を設定
		m_pShadow->SetFieldAngle(FieldNor, PlayerRay);

		// 影の設定処理
		m_pShadow->Update(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), m_fShadowSize, m_fShadowSize, SHADOW_MAX_HEIGHT, SHADOW_A_LEVEL);
	}

	if (m_pObjectX != nullptr)
	{
		// 位置の設定処理
		m_pObjectX->GetPosition()->Set(pos);

		// 向きの設定処理
		m_pObjectX->GetRotaition()->Set(rot);
	}

	// 位置の設定処理
	m_pos.Set(pos);

	m_nLife--;

	if (m_nLife <= 0)
	{
		// オブジェクトの破棄
		m_pObjectX->Uninit();

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

	if (m_pObjectX != nullptr)
	{
		// 描画処理(透明度設定)
		m_pObjectX->Draw(fRate);
	}
}
