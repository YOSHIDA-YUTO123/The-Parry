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
#define SHADOW_SIZE (50.0f)		// 影の大きさ
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
	m_pShadow = nullptr;
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
CRubble* CRubble::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 dir, const int nLife)
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

	int random = rand() % TYPE::TYPE_MAX;

	switch (random)
	{
	case TYPE::TYPE_ONE:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust000.x");
		break;
	case TYPE::TYPE_TWO:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust001.x");
		break;
	case TYPE::TYPE_THREE:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust002.x");
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
// 瓦礫の生成処理(円形)
//===================================================
void CRubble::Creates(const int nNumDust,const D3DXVECTOR3 move, const D3DXVECTOR3 pos, const int nLife)
{
	for (int nCnt = 0; nCnt < nNumDust; nCnt++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nNumDust * nCnt;

		float dirX = sinf(fAngle) * move.x;
		float dirZ = cosf(fAngle) * move.z;

		CRubble::Create(pos, D3DXVECTOR3(dirX, move.y, dirZ), nLife);
	}
}

//===================================================
// 初期化処理
//===================================================
HRESULT CRubble::Init(void)
{
	// 影の生成
	m_pShadow = CShadow::Create(m_pos.Get(), SHADOW_SIZE, SHADOW_SIZE, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f));

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
		m_pObjectX->Uninit();
		m_pObjectX = nullptr;
	}

	// 影の破棄
	if (m_pShadow != nullptr)
	{
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

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

		// 地面の角度に合わせた角度を取得
		D3DXVECTOR3 Shadowrot = m_pShadow->GetFieldAngle(FieldNor, PlayerRay);

		// 影の設定処理
		m_pShadow->Setting(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT, SHADOW_A_LEVEL);

		// 影の向きの設定
		m_pShadow->GetRotaition()->Set(Shadowrot);
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
		Uninit();
	}
}

//===================================================
// 描画処理
//===================================================
void CRubble::Draw(void)
{
	if (m_pObjectX != nullptr)
	{
		// 描画処理
		m_pObjectX->Draw();
	}
}
