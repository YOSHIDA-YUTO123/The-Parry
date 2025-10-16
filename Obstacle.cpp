//==============================================
//
// 障害物 [obstacle.cpp]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// インクルードファイル
//**********************************************
#include "Obstacle.h"
#include "objectX.h"
#include "modelManager.h"
#include "manager.h"
#include "obstaclemanager.h"
#include "effect.h"
#include"Collider.h"
#include"meshfield.h"
#include"math.h"
#include "game.h"
#include"tutorial.h"
#include"transform.h"
#include"shadowS.h"
#include "opening.h"
#include "explosion.h"
#include "sound.h"
#include "Collision.h"
#include "velocity.h"

//**********************************************
// 定数宣言
//**********************************************
constexpr float TWO_PI = D3DX_PI * 2.0f; // 2PI

//==============================================
// コンストラクタ
//==============================================
CObstacle::CObstacle(const TYPE type)
{
	m_type = type;
	m_pMove = nullptr;
	m_pAABB = nullptr;
	m_CenterPos = Const::VEC3_NULL;
	m_posOld = Const::VEC3_NULL;
}

//==============================================
// デストラクタ
//==============================================
CObstacle::~CObstacle()
{
}

//==============================================
// 初期化処理
//==============================================
HRESULT CObstacle::Init(void)
{
	m_pMove = std::make_unique<CVelocity>();

	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	// 種類の設定
	CObject::SetType(CObject::TYPE::TYPE_OBSTACLE);

	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// AABBの生成
	m_pAABB = CColliderAABB::Create(m_CenterPos, m_posOld, Size);

	// 障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// 障害物マネージャーが取得出来たら
	if (pObstacleManager != nullptr)
	{
		pObstacleManager->AddObstacle(this);
	}

    return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CObstacle::Uninit(void)
{
	CObjectX::Uninit();
}

//==============================================
// 更新処理
//==============================================
void CObstacle::Update(void)
{
	// フィールドへのポインタ
	CMeshField* pField = nullptr;

	// 現在のモードの取得
	CScene::MODE mode = CManager::GetMode();

	if (mode == CScene::MODE_TUTORIAL)
	{
		// フィールドの取得
		pField = CTutorial::GetField();
	}
	else if (mode == CScene::MODE_OPENING)
	{
		// フィールドの取得
		pField = COpening::GetField();
	}
	else if (mode == CScene::MODE_GAME)
	{
		// フィールドの取得
		pField = CGame::GetField();
	}

	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();
	D3DXVECTOR3 Size = CObjectX::GetSize();

	if (m_pMove != nullptr)
	{
		// 移動量
		pos += m_pMove->Get();
	}

	// 高さ
	float fHeight = 0.0f;

	// 地面と当たったら
	if (pField != nullptr && pField->Collision(pos, &fHeight))
	{
		// 地面の高さに合わせる
		pos.y = fHeight;
	}

	// 前回の位置の設定
	m_posOld = m_CenterPos;

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// 位置の設定処理
	m_pAABB->UpdateData(m_CenterPos, m_posOld);
	
	// 重力の設定
	m_pMove->Gravity(-Const::MAX_GRABITY);

	// いちの設定
	CObjectX::SetPosition(pos);
}

//==============================================
// 描画処理
//==============================================
void CObstacle::Draw(void)
{
	// 描画処理
	CObjectX::Draw();
}

//==============================================
// コライダーの生成
//==============================================
void CObstacle::CreateCollider(void)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// AABBの生成
	m_pAABB = CColliderAABB::Create(m_CenterPos, m_posOld, Size);
}

//==============================================
// コンストラクタ
//==============================================
CSpikeTrap::CSpikeTrap() : CObstacle(TYPE_SPIKE_TRAP)
{
	m_nDamageFace = NULL;
}

//==============================================
// デストラクタ
//==============================================
CSpikeTrap::~CSpikeTrap()
{
}

//==============================================
// 生成処理
//==============================================
CSpikeTrap* CSpikeTrap::Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot, const int nDamageFace)
{
	CSpikeTrap* pObstacle = nullptr;

	// 障害物の生成
	pObstacle = new CSpikeTrap;
		
	if (pObstacle == nullptr) return nullptr;
		
	// 初期化に失敗したら
	if (FAILED(pObstacle->Init()))
	{
		// 終了処理
		pObstacle->Uninit();
		pObstacle = nullptr;
	
		return nullptr;
	}
	
	// オブジェクト
	pObstacle->SetPosition(pos);
	pObstacle->SetRotation(rot);
	pObstacle->m_nDamageFace = nDamageFace;

	return pObstacle;
}

//==============================================
// 初期化処理
//==============================================
HRESULT CSpikeTrap::Init(void)
{
	// 初期化処理
	if (FAILED(CObstacle::Init()))
	{
		// 終了処理
		CObstacle::Release();

		return E_FAIL;
	}

	// モデルの読み込み
	CObjectX::LoadModel("data/MODEL/obstacle/spiketrap.x");

	// コライダーの生成	
	CreateCollider();

	return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CSpikeTrap::Uninit(void)
{
	// 終了処理
	CObstacle::Uninit();
}

//==============================================
// 更新処理
//==============================================
void CSpikeTrap::Update(void)
{
	// 更新処理
	CObstacle::Update();
}

//==============================================
// 描画処理
//==============================================
void CSpikeTrap::Draw(void)
{
	// 描画処理
	CObstacle::Draw();
}

//==============================================
// 当たり判定
//==============================================
bool CSpikeTrap::Collision(CColliderAABB *pCollider, D3DXVECTOR3* pushPos)
{
	// 自分のコライダーの取得
	auto pColliderAABB = CObstacle::GetCollider();

	// ダメージを食らう面
	int nDamageFace = 0;

	// 矩形の判定
	if (CCollisionAABB::Collision(pCollider, pColliderAABB, pushPos,&nDamageFace))
	{
		// ダメージを食らう面が同じだったら(またはすべて食らうなら)
		if (m_nDamageFace == nDamageFace || m_nDamageFace == -1)
		{
			return true;
		}
	}

	return false;
}

//==============================================
// コンストラクタ
//==============================================
CTNTBarrel::CTNTBarrel() : CObstacle(TYPE_TNT_BARREL)
{
	m_rotDest = Const::VEC3_NULL;
	m_StopPos = Const::VEC3_NULL;
	m_ShadowSize = Const::VEC3_NULL;
	m_pShadow = nullptr;
	m_nShakeTime = NULL;
	m_bLanding = false;
	D3DXQuaternionIdentity(&m_quat);
	D3DXMatrixIdentity(&m_mtxRot);
	m_axis = Const::VEC3_NULL;
	m_fDestValueRot = NULL;
	m_fValueRot = NULL;
	m_fCircumference = NULL;
	m_bFirstLanding = false;
}

//==============================================
// デストラクタ
//==============================================
CTNTBarrel::~CTNTBarrel()
{
}

//==============================================
// 生成処理
//==============================================
CTNTBarrel* CTNTBarrel::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 DestPos)
{
	CTNTBarrel* pObstacle = nullptr;

	// 障害物の生成
	pObstacle = new CTNTBarrel;

	if (pObstacle == nullptr) return nullptr;

	// オブジェクト
	pObstacle->SetPosition(pos);

	// 目的の位置
	pObstacle->m_DestPos = DestPos;

	// 初期化に失敗したら
	if (FAILED(pObstacle->Init()))
	{
		// 終了処理
		pObstacle->Uninit();
		pObstacle = nullptr;

		return nullptr;
	}

	// 方向ベクトルを求める
	float dir = math::GetTargetAngle(DestPos, pos);

	// 移動方向を設定
	pObstacle->SetRotation(D3DXVECTOR3(0.0f, dir, 0.0f));
	pObstacle->m_rotDest = (D3DXVECTOR3(0.0f, dir, 0.0f));

	return pObstacle;
}

//==============================================
// 初期化処理
//==============================================
HRESULT CTNTBarrel::Init(void)
{
	// 初期化処理
	if (FAILED(CObstacle::Init()))
	{
		// 終了処理
		Uninit();

		return E_FAIL;
	}

	// モデルの読み込み
	CObjectX::LoadModel("data/MODEL/obstacle/TNTBarrel.x");

	// コライダーの生成	
	CreateCollider();

	// 大きさの取得
	D3DXVECTOR3 Size = CObjectX::GetSize();

	// 半径を求める
	float fRadius = Size.x * 0.5f;

	// 円周を求める
	m_fCircumference = D3DX_PI * 2.0f * fRadius;

	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	m_ShadowSize = D3DXVECTOR3(7.0f, 1.0f, 7.0f);
	m_pShadow = CShadowS::Create(pos, &m_ShadowSize);

	// 目的の位置の中間の位置を求める
	m_StopPos = pos + (m_DestPos - pos) * 0.5f;

	return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CTNTBarrel::Uninit(void)
{
	// 影の破棄
	if (m_pShadow != nullptr)
	{
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

	// 終了処理
	CObstacle::Uninit();
}

//==============================================
// 更新処理
//==============================================
void CTNTBarrel::Update(void)
{
	// 更新処理
	CObstacle::Update();

	//// 回転量の減衰
	m_fValueRot += (0.0f - m_fValueRot) * 0.1f;

	// 位置の取得
	D3DXVECTOR3 pos = CObstacle::GetPosition();

	if (m_pShadow != nullptr)
	{
		m_pShadow->SetPosition(pos);
	}

	// 着地したときの揺れ
	LandingShake(pos);

	// 移動クラスの取得
	CVelocity* pMove = CObstacle::GetMove();

	if (pMove != nullptr)
	{
		// 移動量の取得
		D3DXVECTOR3 moveWk = pMove->Get();

		D3DXVECTOR3 dir = math::GetVector(m_StopPos, pos);

		moveWk.x = dir.x * 4.0f;
		moveWk.z = dir.z * 4.0f;

		D3DXVECTOR3 moveValue = moveWk;
		moveValue.y = 0.0f;
		
		float fMoveValue = D3DXVec3Length(&moveValue);

		m_fValueRot = (fMoveValue / m_fCircumference) * TWO_PI;

		// 移動量の設定
		pMove->Set(moveWk);	
	}

	// クォータニオンの設定処理
	SetQuaternion();

	// 向きの取得
	D3DXVECTOR3 rot = CObjectX::GetRotation();

	// 差分を正規化
	NormalizeDiffRot(m_rotDest.y - rot.y, &rot.y);

	// 目的の向きに近づける
	rot.y += (m_rotDest.y - rot.y) * 0.1f;

	// 向きの設定
	CObjectX::SetRotation(rot);
}

//==============================================
// 描画処理
//==============================================
void CTNTBarrel::Draw(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	//計算用のマトリックス
	D3DXMATRIX mtxTrans,mtxRot,mtxRotWk;

	D3DXVECTOR3 Size = CObjectX::GetSize();

	// 位置のマトリックスの作成
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y + Size.y * 0.5f , pos.z);

	D3DXVECTOR3 rot = GetRotation();
	
	// 回転行列の作成
	D3DXMatrixRotationYawPitchRoll(&mtxRotWk, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxRot, &mtxRotWk, &m_mtxRot);
	
	// マトリックスの設定
	CObjectX::SetUpMatrix(mtxRot, mtxTrans);

	// 描画の設定
	CObjectX::SetUpDraw();
}

//==============================================
// 当たり判定
//==============================================
bool CTNTBarrel::Collision(CColliderAABB* pCollider, D3DXVECTOR3* pushPos)
{
	// 自分のコライダーの取得
	auto pColliderAABB = CObstacle::GetCollider();

	// 矩形の判定
	if (CCollisionAABB::Collision(pCollider, pColliderAABB, pushPos))
	{
		return true;
	}

	return false;
}

//==============================================
// クォータニオンの設定処理
//==============================================
void CTNTBarrel::SetQuaternion(void)
{
	// 上方向ベクトルの作成
	D3DXVECTOR3 VecUp = { 0.0f,1.0f,0.0f };

	// 移動クラスの取得
	CVelocity* pMove = CObstacle::GetMove();

	// 取得できなかったら処理しない
	if (pMove == nullptr) return;

	// 進行ベクトル
	D3DXVECTOR3 VecMove = pMove->Get();
	
	D3DXVec3Normalize(&VecMove, &VecMove);

	// 移動していないなら
	if (D3DXVec3Length(&VecMove) == 0.0f)
	{
		return;
	}

	// 回転軸の作成
	D3DXVec3Cross(&m_axis, &VecUp, &VecMove);

	// 初期姿勢だったら
	if (m_quat.x == 0.0f && m_quat.y == 0.0f && m_quat.z == 0.0f && m_quat.w == 1.0f)
	{
		// 回転軸における指定の回転角からクォータニオンを作成
		D3DXQuaternionRotationAxis(&m_quat, &m_axis, m_fValueRot);

		// クォータニオンから回転マトリックスを作成
		D3DXMatrixRotationQuaternion(&m_mtxRot, &m_quat);
	}
	else
	{
		// 計算用クォータニオン
		D3DXQUATERNION quat, quatNew;

		// 回転軸における指定の回転角からクォータニオンを作成
		D3DXQuaternionRotationAxis(&quat, &m_axis, m_fValueRot);

		// 前の姿勢と今の姿勢の合成
		D3DXQuaternionMultiply(&quatNew, &m_quat, &quat);

		// 保存
		m_quat = quatNew;

		// クォータニオンから回転マトリックスを作成
		D3DXMatrixRotationQuaternion(&m_mtxRot, &quatNew);
	}
}

//==============================================
// 着地したときの処理
//==============================================
void CTNTBarrel::LandingShake(const D3DXVECTOR3 pos)
{
	// フィールドの取得
	CMeshField* pMeshField = CGame::GetField();

	// 取得できなかったら処理しない
	if (pMeshField == nullptr) return;
	
	float fHeight = 0.0f;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 地面の着地していたら
	if (pMeshField->Collision(pos, &fHeight))
	{
		// 着地したら
		if (!m_bLanding)
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_BARRIEL);
			}
			// 爆発の生成
			auto pExplotion = CExplosionManager::SetParam(pos, D3DXVECTOR2(70.0f, 70.0f), D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.8f), 4, 3, 5);

			if (pExplotion != nullptr)
			{
				// 爆発の生成
				pExplotion->Create(CExplosion::TYPE_SMOKE, D3DXVECTOR2(10.0f, 1.0f), 4);
			}

			// 揺れる時間
			m_nShakeTime = 30;

			// 着地した
			m_bLanding = true;
		}
	}
	

	if (m_nShakeTime > 0 && m_bLanding)
	{
		// 最初の着地が終わってたら処理しない
		if (m_bFirstLanding) return;

		// 揺れの時間を減らす
		m_nShakeTime--;

		// 現在の向きの取得
		D3DXVECTOR3 rot = m_rotDest;

		float fCounter = static_cast<float>(m_nShakeTime) * 0.1f;

		float sin = sinf(fCounter);
		rot.z = sin * 0.5f;

		// 向きの設定
		CObjectX::SetRotation(rot);
		m_rotDest = rot;

		if (m_nShakeTime <= 0)
		{
			m_bFirstLanding = true;
		}
	}
	else
	{
		// 向きのリセット
		m_rotDest.z = 0.0f;
	}
}
