//===================================================
//
// キャラクター3D [character.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "character3D.h"
#include "textureManager.h"
#include"manager.h"
#include"renderer.h"
#include"math.h"
#include"shadowS.h"

using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CCharacter3D::CCharacter3D()
{
	m_pos = VEC3_NULL;
	m_pRot = nullptr;
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_nLife = NULL;
	m_state = STATE::STATE_NORMAL;
	m_fSpeed = NULL;
	m_ShadowScal = D3DXVECTOR3(2.0f,1.0f,2.0f);
}

//===================================================
// デストラクタ
//===================================================
CCharacter3D::~CCharacter3D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CCharacter3D::Init(void)
{
	// 位置、向きの生成
	m_pRot = new CRotation;

	// 影の生成
	m_pShadowS = CShadowS::Create(m_pos, &m_ShadowScal);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CCharacter3D::Uninit(void)
{
	// 向きの破棄
	if (m_pRot != nullptr)
	{
		delete m_pRot;
		m_pRot = nullptr;
	}

	// 影の破棄
	if (m_pShadowS != nullptr)
	{
		m_pShadowS = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CCharacter3D::Update(void)
{
	// 状態の遷移
	switch (m_state)
	{
	case STATE_NORMAL:
		break;
	case STATE_MOVE:
		break;
	case STATE_ACTION:

		// 状態のカウンターを減らす
		m_nCounterState--; 

		// カウンターが0になったら
		if (m_nCounterState <= 0)
		{
			// 状態をノーマルに戻す
			m_state = STATE_NORMAL;
		}

		break;
	case STATE_DAMAGE:

		// 状態のカウンターを減らす
		m_nCounterState--; 

		// カウンターが0になったら
		if (m_nCounterState <= 0)
		{
			// 状態をノーマルに戻す
			m_state = STATE_NORMAL;
		}

		break;
	default:
		break;
	}

	if (m_pShadowS != nullptr)
	{
		m_pShadowS->SetPosition(m_pos);
	}
}

//===================================================
// 描画処理
//===================================================
void CCharacter3D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きの取得
	D3DXVECTOR3 rot = m_pRot->Get();

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//===================================================
// キャラクターの設定処理
//===================================================
void CCharacter3D::SetCharacter(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal)
{
	m_nLife = nLife;
	m_fSpeed = fSpeed;
	m_ShadowScal = ShadowScal;
}

//===================================================
// キャラクターのヒット処理
//===================================================
bool CCharacter3D::Hit(int nDamage)
{
	// 体力を減らす
	m_nLife -= nDamage;

	if (m_nLife <= 0)
	{
		m_state = STATE_DEATH;

		return false;
	}

	return true;
}

//===================================================
// 生きているかどうか
//===================================================
bool CCharacter3D::GetAlive(void)
{
	// 死んでいるなら
	if (m_nLife <= 0)
	{
		return false;
	}

	return true;
}

//===================================================
// 状態の設定処理
//===================================================
void CCharacter3D::SetState(const STATE state, const int nTime)
{
	m_state = state;
	m_nCounterState = nTime;
}

//===================================================
// 影の消去
//===================================================
void CCharacter3D::DeleteShadow(void)
{
	// 影の破棄
	if (m_pShadowS != nullptr)
	{
		m_pShadowS->Uninit();
		m_pShadowS = nullptr;
	}
}
