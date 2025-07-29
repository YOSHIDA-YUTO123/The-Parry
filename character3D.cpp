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
#include"model.h"
#include"motion.h"

using namespace math; // 名前空間mathを使用
using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CCharacter3D::CCharacter3D() : CObject(4)
{
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_pos = VEC3_NULL;
	m_pRot = nullptr;
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_nLife = NULL;
	m_state = STATE::STATE_NORMAL;
	m_fSpeed = NULL;
	m_ShadowScal = D3DXVECTOR3(2.0f,1.0f,2.0f);
	m_nHitStopTime = NULL;
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
	for (int nCnt = 0; nCnt < (int)m_apModel.size(); nCnt++)
	{
		// モデルの破棄
		if (m_apModel[nCnt] != nullptr)
		{
			// 終了処理
			m_apModel[nCnt]->Uninit();

			delete m_apModel[nCnt];

			m_apModel[nCnt] = nullptr;
		}
	}

	if (m_pMotion != nullptr)
	{
		// モーションの終了処理
		m_pMotion->Uninit();
	}

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

	// 自分自身の破棄
	CObject::Release();
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

	// モデルの描画
	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			// 描画処理
			m_apModel[nCnt]->Draw();
		}
	}
}

//===================================================
// モーションのロード
//===================================================
void CCharacter3D::LoadMotion(const char* pFileName,const int nNumMotion)
{
	// モーションのロード処理
	m_pMotion = CMotion::Load(pFileName, m_apModel, &m_nNumModel, nNumMotion, CMotion::LOAD_TEXT);
}

//===================================================
// モデルの位置の取得
//===================================================
D3DXVECTOR3 CCharacter3D::GetModelPos(const int nIdx)
{
	// モデルのマトリックスの取得
	D3DXMATRIX mtx = m_apModel[nIdx]->GetMatrixWorld();

	// ワールドマトリックスの位置の取得
	D3DXVECTOR3 modelPos = GetPositionFromMatrix(mtx);

	return modelPos;
}

//===================================================
// モデルの向きの取得
//===================================================
D3DXVECTOR3 CCharacter3D::GetModelRot(const int nIdx)
{
	// モデルの位置の取得
	D3DXVECTOR3 modelRot = m_apModel[nIdx]->GetRotaition();

	return modelRot;
}

//===================================================
// モデルの大きさの取得
//===================================================
D3DXVECTOR3 CCharacter3D::GetModelSize(const int nIdx)
{
	// モデルの大きさの取得
	D3DXVECTOR3 modelSize = m_apModel[nIdx]->GetSize();

	return modelSize;
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
// ヒットストップの設定処理
//===================================================
bool CCharacter3D::HitStop(void)
{
	// カウンターを減らす
	m_nHitStopTime--;

	// 0以下だったら
	if (m_nHitStopTime <= 0)
	{
		// ヒットストップしていない
		return false;
	}
	return true;
}

//===================================================
// モーションの更新処理
//===================================================
void CCharacter3D::UpdateMotion(void)
{
	if (m_pMotion != nullptr)
	{
		// モーションの更新処理
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}
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
