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
#include<string>
#include "CharacterManager.h"

//===================================================
// コンストラクタ
//===================================================
CCharacter3D::CCharacter3D() : CObject(4)
{
	m_Size = Const::VEC3_NULL;
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_pos = Const::VEC3_NULL;
	m_rot = Const::VEC3_NULL;
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_nLife = NULL;
	m_state = STATE::STATE_NORMAL;
	m_fSpeed = NULL;
	m_ShadowScal = D3DXVECTOR3(2.0f,1.0f,2.0f);
	m_nHitStopTime = NULL;
	m_fRotDest = NULL;
}

//===================================================
// コンストラクタ
//===================================================
CCharacter3D::CCharacter3D(const TYPE type) : CObject(4)
{
	m_type = type;
	m_Size = Const::VEC3_NULL;
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_pos = Const::VEC3_NULL;
	m_rot = Const::VEC3_NULL;
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_nLife = NULL;
	m_state = STATE::STATE_NORMAL;
	m_fSpeed = NULL;
	m_ShadowScal = D3DXVECTOR3(2.0f, 1.0f, 2.0f);
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

	// 差分を正規化
	NormalizeDiffRot(m_fRotDest - m_rot.y, &m_rot.y);

	// 目的の向きに近づける
	m_rot.y += (m_fRotDest - m_rot.y) * 0.1f;
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

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
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
// 描画処理(透明度設定)
//===================================================
void CCharacter3D::Draw(const float fAvl)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
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
			m_apModel[nCnt]->Draw(fAvl);
		}
	}
}

//===================================================
// 描画処理(マルチテクスチャ)
//===================================================
void CCharacter3D::DrawMT(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
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
			m_apModel[nCnt]->DrawMultTexture();
		}
	}
}

//===================================================
// モーションのロード
//===================================================
CMotion* CCharacter3D::LoadMotion(const char* pFileName,const int nNumMotion)
{
	// 省略用
	std::string string = "data/MOTION/";

	// 文字列を合成
	string += pFileName;

	// キャラクターのマネージャーの取得
	auto pCharacterManager = CCharacterManager::GetInstance();

	if (pCharacterManager == nullptr) return nullptr;

	// 自分の種類の取得
	TYPE type = GetType();

	// モーションのロード処理
	m_pMotion = CMotion::Load(string.c_str(), m_apModel, &m_nNumModel, nNumMotion, CMotion::LOAD_TEXT);

	// モーションの設定
	pCharacterManager->SetMotion(m_pMotion.get(), type);

	int nCnt = 0;

	// モデルの要素分調べる
	for (auto itr = m_apModel.begin(); itr != m_apModel.end(); ++itr)
	{
		// nullなら処理しない
		if ((*itr) == nullptr) continue;

		// モーションの設定
		pCharacterManager->SetModel((*itr), type, m_nNumModel,nCnt);

		nCnt++;
	}

	return m_pMotion.get();
}

//===================================================
// モデルの位置の取得
//===================================================
D3DXVECTOR3 CCharacter3D::GetModelPos(const int nIdx)
{
	// モデルのマトリックスの取得
	D3DXMATRIX mtx = m_apModel[nIdx]->GetMatrixWorld();

	// ワールドマトリックスの位置の取得
	D3DXVECTOR3 modelPos = math::GetPositionFromMatrix(mtx);

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
// 親のマトリックスの取得
//===================================================
D3DXMATRIX CCharacter3D::GetParent(const int nIdx) const
{
	D3DXMATRIX OutMtx; // 親のマトリックス

	// 親のマトリックスの取得
	OutMtx = m_apModel[nIdx]->GetMatrixWorld();

	return OutMtx;
}

//===================================================
// キャラクターの設定処理
//===================================================
void CCharacter3D::SetCharacter(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size)
{
	m_nLife = nLife;
	m_fSpeed = fSpeed;
	m_ShadowScal = ShadowScal;
	m_Size = Size;
}

//===================================================
// モデルのマルチテクスチャの設定
//===================================================
void CCharacter3D::SetModelMT(const char* pTextureName)
{
	// モデルの総数分調べる
	for (auto itr = m_apModel.begin(); itr != m_apModel.end(); ++itr)
	{
		// テクスチャの設定
		(*itr)->SetTextureMT(pTextureName);
	}
}

//===================================================
// 情報のコピー
//===================================================
void CCharacter3D::Copy(CCharacter3D* pCharacter)
{
	pCharacter->m_nNumModel = m_nNumModel;

	// モデルの要素分を調べる
	for (int nCnt = 0;nCnt < m_nNumModel;nCnt++)
	{
		// nullだったら処理しない
		if (m_apModel[nCnt] == nullptr) continue;

		// モデルの名前の取得
		const char* pModelName = m_apModel[nCnt]->GetModelName();

		// モデルの生成
		auto pModel = CModel::Create(pModelName);

		// 親のインデックスの取得
		int nParentIdx = m_apModel[nCnt]->GetParentID();

		// 情報のコピー
		m_apModel[nCnt]->Copy(pModel);

		if (nParentIdx != -1)
		{
			// 親のモデルの設定
			pModel->SetParent(pCharacter->m_apModel[nParentIdx], nParentIdx);
		}
		else
		{
			// 親のモデルの設定
			pModel->SetParent(nullptr, nParentIdx);
		}
		// モデルの読み込み
		pCharacter->m_apModel.push_back(pModel);
	}

	m_pMotion->GetInfo(pCharacter->m_pMotion.get());
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
// キャラクターの設定
//===================================================
void CCharacter3D::SetCharacter(void)
{
	// モーションの生成
	m_pMotion = std::make_unique<CMotion>();

	// キャラクターのマネージャーの取得
	auto pCharacterManager = CCharacterManager::GetInstance();

	if (pCharacterManager == nullptr) return;

	// モーションの取得
	pCharacterManager->GetMotion(m_pMotion.get(), m_type);

	// モデルのサイズの取得
	m_nNumModel = pCharacterManager->GetModelSize(m_type);

	// モデルのサイズの確保
	m_apModel.resize(m_nNumModel);

	// モデルの総数分調べる
	for (int nCnt = 0;nCnt < m_nNumModel;nCnt++)
	{
		// 親のインデックス
		int nParentIdx = -1;

		// モデルの取得
		pCharacterManager->GetModel(&m_apModel[nCnt],m_type,nCnt,&nParentIdx);

		if (nParentIdx != -1)
		{
			// 親のモデルの設定
			m_apModel[nCnt]->SetParent(m_apModel[nParentIdx], nParentIdx);
		}
		else
		{
			// 親のモデルの設定
			m_apModel[nCnt]->SetParent(nullptr, nParentIdx);
		}
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
