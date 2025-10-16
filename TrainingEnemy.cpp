//================================================
//
// 練習用の敵 [TrainingEnemy.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "TrainingEnemy.h"
#include"motion.h"
#include"transform.h"
#include"player.h"
#include"tutorial.h"
#include"Collider.h"
#include"statebase.h"
#include"math.h"
#include"TrainingEnemyState.h"
#include"manager.h"
#include"renderer.h"
#include "Collision.h"
#include"GameCamera.h"
#include "impact.h"
#include "effect.h"
#include "ParticleSpark.h"
#include"Orbit.h"
#include "game.h"
#include"slow.h"
#include "sound.h"

using namespace Const;// 名前空間Constを使用
using namespace std;  // 名前空間stdを使用
using namespace math; // 名前空間mathを使用

//================================================
// コンストラクタ
//================================================
CTrainingEnemy::CTrainingEnemy() : CCharacter3D(TYPE_TRAININGENEMY)
{
	m_pOrbit = nullptr;
	m_pCapsule = nullptr;
	m_pMachine = nullptr;
	D3DXMatrixIdentity(&m_SwordMtx);
}

//================================================
// デストラクタ
//================================================
CTrainingEnemy::~CTrainingEnemy()
{
}

//================================================
// 生成処理
//================================================
CTrainingEnemy* CTrainingEnemy::Create(const D3DXVECTOR3 pos)
{
	CTrainingEnemy* pEnemy = new CTrainingEnemy;
	
	// 位置の取得
	pEnemy->SetPosition(pos);

	// 初期化処理
	if (FAILED(pEnemy->Init()))
	{
		pEnemy->Uninit();
		pEnemy = nullptr;
		return nullptr;
	}
	return pEnemy;
}

//================================================
// 初期化処理
//================================================
HRESULT CTrainingEnemy::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter3D::Init()))
	{
		return E_FAIL;
	}

	// モーションのロード
	CCharacter3D::LoadMotion("motionTrainingEnemy.txt", MOTIONTYPE_MAX);
	CCharacter3D::SetCharacter(9999, 0.0f, D3DXVECTOR3(5.0f, 1.0f, 5.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// 向きの設定
	CCharacter3D::SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));
	CCharacter3D::SetRotDest(D3DX_PI);

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();
	D3DXVECTOR3 headPos = CCharacter3D::GetModelPos(MODEL_HEAD);

	// カプセルの生成
	m_pCapsule = CColliderCapsule::Create(pos, headPos, 55.0f, 0);

	// 状態マシーンの生成
	m_pMachine = make_unique<CStateMachine>();

	// 初期状態の設定
	ChangeState(make_shared<CTrainingEnemyIdle>());

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CTrainingEnemy::Uninit(void)
{
	m_pCapsule = nullptr;
	m_pMachine = nullptr;

	// 終了処理
	CCharacter3D::Uninit();
}

//================================================
// 更新処理
//================================================
void CTrainingEnemy::Update(void)
{
	// モーションの取得
	CMotion* pMotion = CCharacter3D::GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// カメラの取得
	CGameCamera* pCamera = CTutorial::GetCamera();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();
	D3DXVECTOR3 headPos = CCharacter3D::GetModelPos(MODEL_HEAD);

	// ダメージ状態じゃないなら
	if (m_pCapsule != nullptr && !IsDamage(pMotion))
	{
		// コライダーのデータの取得
		CColliderCapsule::Data data = m_pCapsule->GetData();

		// データの更新
		data.StartPos = pos;
		data.EndPos = headPos;

		// コライダーの更新
		m_pCapsule->UpdateData(data);

		// 当たり判定
		pPlayer->CollisionCapsule(m_pCapsule.get());

		// 攻撃の判定処理
		CollisionPlayerAttack();
	}

	if (m_pMachine != nullptr)
	{
		// 状態の更新処理
		m_pMachine->Update();
	}

	// 更新処理
	CCharacter3D::Update();

	// モーションの更新処理
	CCharacter3D::UpdateMotion();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetModelPos(CPlayer::MODEL_CHEST);

	// 胸の位置
	D3DXVECTOR3 chestpos = CCharacter3D::GetModelPos(MODEL_CHEST);

	// カメラがnullじゃないなら
	pCamera->Rockon(PlayerPos, chestpos,400.0f);
	
}

//================================================
// 描画処理
//================================================
void CTrainingEnemy::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_SwordMtx);

	// 親の位置、向きの設定
	D3DXVECTOR3 ParentPos = CCharacter3D::GetModelPos(MODEL_WEPON);
	D3DXVECTOR3 ParentRot = CCharacter3D::GetModelRot(MODEL_WEPON);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, ParentRot.y, ParentRot.x, ParentRot.z);
	D3DXMatrixMultiply(&m_SwordMtx, &m_SwordMtx, &mtxRot);

	// 大きさの取得
	D3DXVECTOR3 Size = CCharacter3D::GetModelSize(MODEL_WEPON);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, 0.0f, Size.y, 0.0f);
	D3DXMatrixMultiply(&m_SwordMtx, &m_SwordMtx, &mtxTrans);

	// 親のワールドマトリックスの取得
	pDevice->GetTransform(D3DTS_WORLD, &mtxParent);

	// 親のワールドマトリックスと掛け合わせる
	D3DXMatrixMultiply(&m_SwordMtx, &m_SwordMtx, &mtxParent);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_SwordMtx);
}

//================================================
// 軌跡の設定処理
//================================================
void CTrainingEnemy::Orbit(const int nSegH, const D3DXCOLOR col)
{
	// 武器の先端の位置
	D3DXVECTOR3 WeponTop = GetPositionFromMatrix(m_SwordMtx);

	D3DXVECTOR3 WeponBottom = CCharacter3D::GetModelPos(15);

	// 軌跡の生成
	if (m_pOrbit == nullptr)
	{
		m_pOrbit = CMeshOrbit::Create(WeponTop, WeponBottom, nSegH, col);
	}

	// 軌跡が使われていて寿命が無かったら
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->SetPosition(WeponTop, WeponBottom);
	}
}

//================================================
// 軌跡の破棄
//================================================
void CTrainingEnemy::DeleteOrbit(void)
{
	if (m_pOrbit != nullptr)
	{
		m_pOrbit->Uninit();
		m_pOrbit = nullptr;
	}
}

//================================================
// 範囲内にいるかどうか
//================================================
bool CTrainingEnemy::CheckDistance(float fRange)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerpos = pPlayer->GetPosition();

	// 敵の位置の取得
	D3DXVECTOR3 enemypos = CCharacter3D::GetPosition();

	// 距離の取得
	float distance = GetDistance(playerpos - enemypos);

	// 一定の範囲に入ったら
	if (distance <= fRange)
	{
		return true;
	}
	return false;
}

//================================================
// 状態の変更
//================================================
void CTrainingEnemy::ChangeState(std::shared_ptr<CTrainingEnemyState> pNewState)
{
	if (pNewState != nullptr)
	{
		// 自分のポインタを渡す
		pNewState->SetOwner(this);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の変更
		m_pMachine->Change(pNewState);
	}
}

//================================================
// 攻撃の結果の取得
//================================================
CTrainingEnemy::RESULT CTrainingEnemy::GetAttackResult(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return RESULT_NONE;

	// プレイヤーのモーションの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// プレイヤーのモーションの取得
	int playerMotionType = pPlayerMotion->GetBlendType();

	// 武器が当たったら
	if (CollisionPlayer())
	{
		if (playerMotionType == pPlayer->MOTIONTYPE_REVENGE)
		{
			// パリィした
			return RESULT_SPREVENGE;
		}
		else if (playerMotionType != pPlayer->MOTIONTYPE_PARRY)
		{
			// パリィできるか判定
			const bool bParry = pPlayer->IsParry(pos);

			// パリィできた
			if (bParry)
			{
				// パリィした
				return RESULT_PARRY;
			}
			// 回避だったら
			else if (playerMotionType == pPlayer->MOTIONTYPE_AVOID || pPlayerMotion->GetType() == pPlayer->MOTIONTYPE_AVOID)
			{
				// 回避した
				return RESULT_AVOID;
			}
			// カウンター失敗した
			else if (bParry == false)
			{
				// 当たった
				return RESULT_HIT;
			}
		}
	}

	return RESULT_NONE;
}

//================================================
// プレイヤーの攻撃の判定
//================================================
void CTrainingEnemy::CollisionPlayerAttack(void)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// プレイヤーのモーションの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// パリィの成功度の取得
	int nSuccess = pPlayer->SuccessParry();

	// パリィモーションの蹴りになったら
	if (pPlayerMotion->IsEventFrame(38, 38, pPlayer->MOTIONTYPE_ROUNDKICK))
	{
		// プレイヤーの右足の位置
		D3DXVECTOR3 playerFootR = pPlayer->GetModelPos(CPlayer::MODEL_FOOTR);

		// 右手の円
		CColliderSphere FootRSphere = CColliderSphere::CreateCollider(playerFootR, 250.0f);

		// 手が当たったら
		if (CCollisionCapsule::CollisionSphere(m_pCapsule.get(), &FootRSphere))
		{
			// ダメ―ジモーションの設定
			SelectDamageMotion(nSuccess, playerFootR);
		}
	}
}

//================================================
// プレイヤーとの当たり判定
//================================================
bool CTrainingEnemy::CollisionPlayer(void)
{
	// 位置の取得
	D3DXVECTOR3 sword_buttom = CCharacter3D::GetModelPos(MODEL_WEPON);
	D3DXVECTOR3 sword_Top = GetPositionFromMatrix(m_SwordMtx);

	// コライダーの作成
	CColliderCapsule capsule = CColliderCapsule::CreateCollider(sword_buttom, sword_Top,50.0f);

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// プレイヤーの取得
	if (pPlayer == nullptr) return false;

	// プレイヤーとの当たり判定
	if (pPlayer->CollisionCapsule(&capsule,false))
	{
		return true;
	}

	return false;
}

//===================================================
// ダメージ状態かどうか
//===================================================
bool CTrainingEnemy::IsDamage(CMotion* pMotion)
{
	// nullだったら処理しない
	if (pMotion == nullptr) return false;

	// モーションの種類
	int nMotionType = pMotion->GetBlendType();

	// モーションの種類がダメージだったら
	if (nMotionType == MOTIONTYPE_DAMAGE) return true;

	// モーションの種類が中ダメージだったら
	if (nMotionType == MOTIONTYPE_DAMAGES) return true;

	// モーションの種類が小ダメージだったら
	if (nMotionType == MOTIONTYPE_DAMAGELS) return true;

	return false;
}

//===================================================
// どのダメージモーションが出るか判定
//===================================================
void CTrainingEnemy::SelectDamageMotion(int success, const D3DXVECTOR3 ImpactPos)
{
	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 成功度の遷移
	switch (success)
	{
	case CPlayer::PARRY_MISS:
		break;
	case CPlayer::PARRY_WEAK:
	{
		// ダメージ状態の生成
		auto pDamageState = make_shared<CTrainingEnemyDamage>();

		// ダメージの種類の設定
		pDamageState->SetType(CTrainingEnemyDamage::TYPE_DAMAGELS);

		// 状態の変更
		ChangeState(pDamageState);

		D3DXVECTOR3 pos = GetPosition();

		// パーティクルの生成
		CParticleSpark* pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(255, 127, 80, 255));
		pSpark->SetParticle(15.0f, 60, 150, 1, -180);

		pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(106, 90, 205, 255));
		pSpark->SetParticle(15.0f, 60, 150, 1, -180);

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), ImpactPos, 30.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(15.0f, 1.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));
	}
	break;
	case CPlayer::PARRY_NORMAL:
	{
		// ダメージ状態の生成
		auto pDamageState = make_shared<CTrainingEnemyDamage>();

		// ダメージの種類の設定
		pDamageState->SetType(CTrainingEnemyDamage::TYPE_DAMAGES);

		// 状態の変更
		ChangeState(pDamageState);

		D3DXVECTOR3 pos = GetPosition();

		// パーティクルの生成
		CParticleSpark* pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(255, 127, 80, 255));
		pSpark->SetParticle(15.0f, 60, 150, 1, -180);

		pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(106, 90, 205, 255));
		pSpark->SetParticle(15.0f, 60, 150, 1, -180);

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), ImpactPos, 30.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(15.0f, 1.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));
	}
	break;
	case CPlayer::PARRY_PARFECT:
	{
		// スローモーションの取得
		CSlow* pSlow = CManager::GetSlow();

		// ダメージ状態の生成
		auto pDamageState = make_shared<CTrainingEnemyDamage>();

		// ダメージの種類の設定
		pDamageState->SetType(CTrainingEnemyDamage::TYPE_DAMAGE);

		// 状態の変更
		ChangeState(pDamageState);

		D3DXVECTOR3 pos = GetPosition();

		// パーティクルの生成
		CParticleSpark* pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(255, 127, 80, 255));
		pSpark->SetParticle(15.0f, 60, 150, 1, -180);

		pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(106, 90, 205, 255));
		pSpark->SetParticle(15.0f, 60, 150, 1, -180);

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// ボスまでの角度を取得
		float fAngle = GetTargetAngle(pos, PlayerPos);

		// 向きの設定
		pPlayer->SetAngle(fAngle + D3DX_PI);

		// インパクトを生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), ImpactPos, 30.0f, 50.0f);

		// サークルの設定処理
		pCircle->SetCircle(15.0f, 1.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		if (pSlow != nullptr)
		{
			// スローモーション
			pSlow->Start(60, 12);
		}
	}
	break;
	default:
		break;
	}
}